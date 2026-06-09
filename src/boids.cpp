#include <algorithm>
#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>
#include <wooting-usb.h>
#include <wooting-rgb-sdk.h>
#include <plugin.h>
#include <hidapi/hidapi.h>
#include <cassert>
#include <format>
#include "algorithms/boids/flock.h"



boid::Flock* createFlock (){
    int numBoids = 200;

    boid::Flock* flock = new boid::Flock();
    for (int i = 0; i < 200; ++i) {
        float x = static_cast<float>(rand() % 1500);
        float y = static_cast<float>(rand() % 600);
        int type = i % 2;  // Alternate between type 0 and type 1
        flock->addBoid(boid::Boid(x, y, 0.0f, 0.0f, type));
    }
    return flock;
}

uint8_t keyboardArray[6][21][3];
void convertFlockToKeyboard(boid::Flock* flock) {
    std::fill(&keyboardArray[0][0][0], &keyboardArray[0][0][0] + sizeof(keyboardArray) / sizeof(uint8_t), 0);

    const auto& boids = flock->getBoids();
    for (const auto& b : boids) {
        int xIndex = static_cast<int>(b.position.x / 1500.0f * 16);
        int yIndex = static_cast<int>(b.position.y / 600.0f * 6);
        if (xIndex >= 0 && xIndex < 16 && yIndex >= 0 && yIndex < 6) {

            if (b.getType() == 0) {
                int newRedValue = (int) keyboardArray[yIndex][xIndex][0] + 50;
                newRedValue = std::min(newRedValue, 255);
                keyboardArray[yIndex][xIndex][0] = newRedValue;
            } else {
                int newBlueValue = (int) keyboardArray[yIndex][xIndex][2] + 50;
                newBlueValue = std::min(newBlueValue, 255);
                keyboardArray[yIndex][xIndex][2] = newBlueValue;
            }
            

        }
    }

    // Special handle for rightside keys
    for (int rgb = 0; rgb < 3; ++rgb) {
        keyboardArray[5][16][rgb] = keyboardArray[5][15][rgb]; // Right arrow
        keyboardArray[5][15][rgb] = keyboardArray[5][14][rgb]; // Down arrow
        keyboardArray[5][14][rgb] = keyboardArray[5][13][rgb]; // Left arrow
        keyboardArray[4][15][rgb] = keyboardArray[4][15][rgb]; // Up arrow

        keyboardArray[0][16][rgb] = keyboardArray[0][15][rgb]; // Up arrow
        keyboardArray[1][16][rgb] = keyboardArray[1][15][rgb]; // Up arrow
        keyboardArray[2][16][rgb] = keyboardArray[2][15][rgb]; // Up arrow
    }
}

// TODO: Remove manual HID implementation and use WOOTING_USB when possible
#define WOOTING_V3_REPORT_SIZE 2046 + 1
#define WOOTING_RGB_ROWS 6
#define WOOTING_RGB_COLS 21
bool hidSendRgbBufferUpdate() {
    uint8_t report_buffer[WOOTING_V3_REPORT_SIZE] = {0};
    report_buffer[0] = 5;
    report_buffer[1] = 0xD1;
    report_buffer[2] = 0xDA;
    report_buffer[3] = WOOTING_RAW_COLORS_REPORT;

    uint16_t rgb_buffer_matrix[WOOTING_RGB_ROWS][WOOTING_RGB_COLS] = {0x0f}; // This should be filled with the RGB values for each key
    memcpy(&report_buffer[4], rgb_buffer_matrix,
            WOOTING_RGB_ROWS * WOOTING_RGB_COLS * sizeof(uint16_t));
    
            
    hid_device_info* devs = hid_enumerate(0x31E3, 0x1402);
    hid_device* keyboard_handle;
    for (auto* d = devs; d; d = d->next)
    {
        if (d->interface_number == 2) {
            keyboard_handle = hid_open_path(d->path);
            break;
        }
    }

    int report_size =
        hid_write(keyboard_handle, report_buffer, WOOTING_V3_REPORT_SIZE);
    if (report_size == WOOTING_V3_REPORT_SIZE) {
        std::cout << "Successfully sent V3 buffer...\n";
        return true;
    } else {
        std::cout << std::format("Got report size: {}, expected: {}, disconnecting..\n",
                report_size, WOOTING_V3_REPORT_SIZE);
        if (report_size == -1) {
            wprintf(L"hid_write failed: %ls\n", hid_error(keyboard_handle));
        }
        return false;
    }
}


const int BUFFER_SIZE = 8;
unsigned short codeBuffer[BUFFER_SIZE];
float analogBuffer[BUFFER_SIZE];

void runSimulation(std::atomic_bool& running) {
    constexpr float width = 1600.0f;
    constexpr float height = 600.0f;

    constexpr float FPS_TARGET = 15.0f;

    constexpr auto frameBudget = std::chrono::duration<double, std::milli>(1000.0 / FPS_TARGET);

    boid::Flock* flock = createFlock();

    while (running.load(std::memory_order_relaxed)) {
        const auto start = std::chrono::steady_clock::now();
        
        // TODO: implement analog attractors
        int keyLength = wooting_analog_read_full_buffer(codeBuffer, analogBuffer, BUFFER_SIZE);
        // for (int i = 0; i < keyLength; i++) {
        //     std::cout << "Keycode: " << codeBuffer[i] << ", Analog Value: " << analogBuffer[i] << std::endl;
        // }

        flock->tick(width, height);
        convertFlockToKeyboard(flock);
        wooting_rgb_array_set_full((uint8_t*) keyboardArray);
        if (!wooting_rgb_array_update_keyboard()) {
            std::cout << "Failed to update keyboard" << std::endl;
        }
        
        const auto elapsed = std::chrono::steady_clock::now() - start;
        if (elapsed > frameBudget) {
            std::cerr << "Warning: simulation tick took "
                      << std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count()
                      << " ms, which exceeds the allotted "
                      << std::chrono::duration_cast<std::chrono::milliseconds>(frameBudget).count()
                      << " ms budget." << std::endl;
        } else {
            std::this_thread::sleep_for(frameBudget - elapsed);
        }
    }
}

int main() {
    wooting_rgb_reset_rgb();
    wooting_rgb_reset();
    wooting_rgb_close();

    bool findKeyboard = wooting_rgb_kbd_connected();
    assert(findKeyboard && "ERROR: Keyboard not found");
    
    int analogInitialised = wooting_analog_initialise();
    assert(analogInitialised >= 0 && "ERROR: Analog SDK failed to initialise");
    wooting_analog_set_keycode_mode(1);

    std::cout << "Keyboard detected " << wooting_rgb_kbd_connected() << std::endl;
    uint8_t deviceCount = wooting_usb_device_count();
    std::cout << "Number of devices " << (int) deviceCount << std::endl;

    std::atexit([](){
        wooting_rgb_reset_rgb();
        wooting_rgb_reset();
        wooting_rgb_close();
    });

    std::atomic_bool running{true};
    std::thread simulationThread(runSimulation, std::ref(running));

    std::cout << "Simulation running on a background thread. Press Enter to stop." << std::endl;
    std::cin.get();

    running.store(false, std::memory_order_relaxed);
    if (simulationThread.joinable()) {
        simulationThread.join();
    }

    wooting_rgb_reset_rgb();
    wooting_rgb_reset();
    wooting_analog_uninitialise();
    return 0;
}
