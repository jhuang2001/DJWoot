#include <windows.h>
#include <cmath>
#include <chrono>
#include <iostream>
#include "../../algorithms/boids/flock.h"
#include "../../algorithms/boids/boid.h"
#include "../../algorithms/boids/vector2.h"

const int WINDOW_WIDTH = 1500;
const int WINDOW_HEIGHT = 600;
const int NUM_BOIDS = 200;

boid::Flock* g_flock = nullptr;
bool g_running = true;
HBRUSH g_blueBrush = nullptr;
HBRUSH g_redBrush = nullptr;
HBRUSH g_orangeBrush = nullptr;
HBRUSH g_yellowBrush = nullptr;
HBRUSH g_greenBrush = nullptr;
HBRUSH g_bgBrush = nullptr;
HBRUSH g_purpleBrush = nullptr;
HPEN g_redPen = nullptr;
HPEN g_orangePen = nullptr;
HPEN g_yellowPen = nullptr;
HPEN g_greenPen = nullptr;
HPEN g_bluePen = nullptr;
HPEN g_purplePen = nullptr;
HDC g_memDC = nullptr;
HBITMAP g_memBitmap = nullptr;
HDC g_screenDC = nullptr;
float MARGIN = 0.0f;
float FPS_TARGET = 24.0f;

void DrawTriangleBoid(HDC hdc, float x, float y, float vx, float vy, HBRUSH brush) {
    // Calculate rotation angle from velocity
    float angle = std::atan2(vy, vx);
    float size = 5.0f;

    // Triangle points (pointing in direction of velocity)
    POINT points[3];
    points[0].x = static_cast<int>(x + size * std::cos(angle));
    points[0].y = static_cast<int>(y + size * std::sin(angle));

    points[1].x = static_cast<int>(x + size * std::cos(angle + 2.5f));
    points[1].y = static_cast<int>(y + size * std::sin(angle + 2.5f));

    points[2].x = static_cast<int>(x + size * std::cos(angle - 2.5f));
    points[2].y = static_cast<int>(y + size * std::sin(angle - 2.5f));

    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);
    HPEN oldPen = (HPEN)SelectObject(hdc, GetStockObject(NULL_PEN));
    Polygon(hdc, points, 3);
    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);
}

void DrawBoidVectors(HDC hdc, const boid::Boid& boid) {
    
    // Draw separation force in red
    HPEN pen = g_redPen;
    HPEN oldPen = (HPEN)SelectObject(hdc, pen);
    Vector2 sep = boid.getSeparationForce();
    MoveToEx(hdc, static_cast<int>(boid.position.x), static_cast<int>(boid.position.y), NULL);
    LineTo(hdc, static_cast<int>(boid.position.x + sep.x), static_cast<int>(boid.position.y + sep.y));
    
    // Draw alignment force in orange
    pen = g_orangePen; SelectObject(hdc, pen);
    Vector2 align = boid.getAlignmentForce();
    MoveToEx(hdc, static_cast<int>(boid.position.x), static_cast<int>(boid.position.y), NULL);
    LineTo(hdc, static_cast<int>(boid.position.x + align.x), static_cast<int>(boid.position.y + align.y));

    // Draw cohesion force in yellow
    pen = g_yellowPen; SelectObject(hdc, pen);
    Vector2 coh = boid.getCohesionForce();
    MoveToEx(hdc, static_cast<int>(boid.position.x), static_cast<int>(boid.position.y), NULL);
    LineTo(hdc, static_cast<int>(boid.position.x + coh.x), static_cast<int>(boid.position.y + coh.y));

    // Draw edge force in purple
    pen = g_purplePen; SelectObject(hdc, pen);
    Vector2 edge = boid.getEdgeForce();
    MoveToEx(hdc, static_cast<int>(boid.position.x), static_cast<int>(boid.position.y), NULL);
    LineTo(hdc, static_cast<int>(boid.position.x + edge.x), static_cast<int>(boid.position.y + edge.y));

    // Draw total force in green
    pen = g_greenPen; SelectObject(hdc, pen);
    Vector2 total = sep + align + coh + edge;
    MoveToEx(hdc, static_cast<int>(boid.position.x), static_cast<int>(boid.position.y), NULL);
    LineTo(hdc, static_cast<int>(boid.position.x + total.x), static_cast<int>(boid.position.y + total.y));

    SelectObject(hdc, oldPen);
}

void Render(HWND hwnd) {
    if (!g_memDC || !g_flock) return;

    RECT rect;
    GetClientRect(hwnd, &rect);

    // Clear background
    FillRect(g_memDC, &rect, g_bgBrush);

    // Draw boids
    const auto& boids = g_flock->getBoids();
    for (const auto& b : boids) {
        DrawTriangleBoid(g_memDC, b.position.x, b.position.y, b.velocity.x, b.velocity.y, b.getType() == 0 ? g_blueBrush : g_redBrush);

        if (b.isDebug()) {
            DrawBoidVectors(g_memDC, b);
        }
    }

    // Blit to screen
    BitBlt(g_screenDC, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, g_memDC, 0, 0, SRCCOPY);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            BeginPaint(hwnd, &ps);
            Render(hwnd);
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_CLOSE:
            g_running = false;
            DestroyWindow(hwnd);
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    const char CLASS_NAME[] = "BoidVisualizerWindow";

    WNDCLASSA wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    RegisterClassA(&wc);

    // Adjust window size to account for borders and title bar
    RECT windowRect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

    HWND hwnd = CreateWindowExA(
        0,
        CLASS_NAME,
        "Boid Visualizer",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_WIDTH, windowRect.bottom - windowRect.top,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (hwnd == NULL) {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    // Create resources
    g_screenDC = GetDC(hwnd);
    g_memDC = CreateCompatibleDC(g_screenDC);
    g_memBitmap = CreateCompatibleBitmap(g_screenDC, WINDOW_WIDTH, WINDOW_HEIGHT);
    SelectObject(g_memDC, g_memBitmap);

    g_redBrush = CreateSolidBrush(RGB(255, 0, 0));
    g_orangeBrush = CreateSolidBrush(RGB(255, 165, 0));
    g_yellowBrush = CreateSolidBrush(RGB(255, 255, 0));
    g_greenBrush = CreateSolidBrush(RGB(0, 255, 0));
    g_blueBrush = CreateSolidBrush(RGB(0, 100, 200));
    g_purpleBrush = CreateSolidBrush(RGB(128, 0, 128));
    g_bgBrush = CreateSolidBrush(RGB(0,0,0));
    g_redPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
    g_orangePen = CreatePen(PS_SOLID, 1, RGB(255, 165, 0));
    g_yellowPen = CreatePen(PS_SOLID, 1, RGB(255, 255, 0));
    g_greenPen = CreatePen(PS_SOLID, 1, RGB(0, 255, 0));
    g_bluePen = CreatePen(PS_SOLID, 1, RGB(0, 100, 200));
    g_purplePen = CreatePen(PS_SOLID, 1, RGB(128, 0, 128));
    SelectObject(g_memDC, g_bluePen);

    // create one debug boid
    g_flock = new boid::Flock();
    float debugX = static_cast<float>(rand() % WINDOW_WIDTH);
    float debugY = static_cast<float>(rand() % WINDOW_HEIGHT);
    g_flock->addBoid(boid::Boid(debugX, debugY, 0.0f, 0.0f, 0, true));

    for (int i = 1; i < NUM_BOIDS; ++i) {
        float x = static_cast<float>(rand() % WINDOW_WIDTH);
        float y = static_cast<float>(rand() % WINDOW_HEIGHT);
        g_flock->addBoid(boid::Boid(x, y, 0, 0, i%2));
    }

    // Main loop
    auto lastTime = std::chrono::high_resolution_clock::now();
    const float targetDeltaTime = 1.0f / FPS_TARGET;

    while (g_running) {
        MSG msg = {};
        while (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                g_running = false;
                break;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (!g_running) break;

        // Update flock
        if (g_flock) {
            g_flock->tick(static_cast<float>(WINDOW_WIDTH-2*MARGIN), static_cast<float>(WINDOW_HEIGHT-2*MARGIN));
        }

        // Render
        Render(hwnd);

        // Frame rate limiting
        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> deltaTime = currentTime - lastTime;
        if (deltaTime.count() < targetDeltaTime) {
            Sleep(static_cast<DWORD>((targetDeltaTime - deltaTime.count()) * 1000.0f));
            // std::cout << "Frame time: " << deltaTime.count() << " seconds. " << "Used " << (deltaTime.count()/targetDeltaTime)*100.0f << "% of target frame time." << std::endl;
        } else {
            std::cout << "Warning: Frame took " << deltaTime.count() << " seconds, which is slower than target " << targetDeltaTime << " seconds." << std::endl;
        }
        lastTime = std::chrono::high_resolution_clock::now();
    }

    // Cleanup
    delete g_flock;
    DeleteObject(g_memBitmap);
    DeleteDC(g_memDC);
    ReleaseDC(hwnd, g_screenDC);
    DeleteObject(g_redBrush);
    DeleteObject(g_orangeBrush);
    DeleteObject(g_yellowBrush);
    DeleteObject(g_greenBrush);
    DeleteObject(g_blueBrush);
    DeleteObject(g_purpleBrush);
    DeleteObject(g_bgBrush);
    DeleteObject(g_redPen);
    DeleteObject(g_orangePen);
    DeleteObject(g_yellowPen);
    DeleteObject(g_greenPen);
    DeleteObject(g_bluePen);
    DeleteObject(g_purplePen);

    return 0;
}

