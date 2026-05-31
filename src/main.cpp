#include <iostream>
#include <wooting-usb.h>
#include <wooting-rgb-sdk.h>
#include <hidapi/hidapi.h>
#include <cassert>

// TODO: Implement BOIDs algorithm to simulate flocking behavior on device

int main() {
    
    bool findKeyboard = wooting_rgb_kbd_connected();
    assert(findKeyboard && "ERROR: Keyboard not found");
    std::cout << "Keyboard detected" << std::endl;

    wooting_rgb_direct_set_key(0, 0, 255, 0, 0);
    
    
    std::cout << "Press Enter to exit and reset Keyboard" << std::endl;
    std::cin.get();
    wooting_rgb_reset_rgb();
    return 0;
}
