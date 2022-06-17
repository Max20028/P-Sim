//Basic Windows Headers
#include <windows.h>
#include <windowsx.h>
#include <initguid.h> //Make COM happy with mingw

#ifndef UNICODE
#define UNICODE
#endif

#include <cstdio>

#include "game.hpp"

//Only importing this for the WM_SIZE message for windowed mode. The answer to remove this may be to make screen unresizable.
#include "Core/RobotMap.h"

int ClientWidth = INITIAL_SCREEN_WIDTH;
int ClientHeight = INITIAL_SCREEN_HEIGHT;

double countsPerSecond = 0.0;
__int64 CounterStart = 0;

int frameCount = 0;
int fps = 0;

__int64 frameTimeOld = 0;
double frameTime;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void StartTimer();
double GetTime();
double GetFrameTime();

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    // Register the window class.
    const wchar_t CLASS_NAME[]  = L"T-Physics-Sim";
    
    WNDCLASS wc = { };

    wc.lpfnWndProc   = WindowProc;
    wc.hInstance     = hInstance;
    wc.lpszClassName = CLASS_NAME;
    

    RegisterClass(&wc);

    // Create the window.
    

    HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        L"Thomas-Physics-Sim",    // Window text
        WS_OVERLAPPEDWINDOW,            // Window style

        // Position and Size
        //This should be overwritten once directx stuff is called
        300, 300, 680, 680,

        NULL,       // Parent window    
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
        );

    if (hwnd == NULL)
    {
        printf("hwnd == NULL");
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    startGame(hwnd, hInstance);

    // Run the message loop.

    printf("Initialization Complete. Starting Message Loop\n");
    MSG msg = { };
    while (true) {
        //Message Loop
        //Note: Was a while loop, where >0, but tutorial used if. I assume this means it can work a max of 1 input per frame
        if(PeekMessage (&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage (&msg);
            DispatchMessage (&msg);

            //Exit Condition
            if(msg.message == WM_QUIT) {
                break;
            }
        }
        //This block is used to count frames since 1 second timer started ie frames per second
        frameCount++;
        if(GetTime() > 1.0f) {
            fps = frameCount;
            frameCount = 0;
            StartTimer();
        }
        //Returns time since GetFrameTime was last called
        frameTime = GetFrameTime();
        updateGame(frameTime, fps);

    }

    //Clean D3d
    endGame(hwnd);

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_SIZE:
        ClientWidth  = LOWORD(lParam);
        ClientHeight = HIWORD(lParam);
        return 0;

    case WM_DESTROY:
        {
            PostQuitMessage(0);
            return 0;
        } break;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void StartTimer() {
    LARGE_INTEGER frequencyCount;
    QueryPerformanceFrequency(&frequencyCount);

    countsPerSecond = double(frequencyCount.QuadPart);

    QueryPerformanceCounter(&frequencyCount);
    CounterStart = frequencyCount.QuadPart;
}
double GetTime() {
    LARGE_INTEGER currentTime;
    QueryPerformanceCounter(&currentTime);
    return double(currentTime.QuadPart-CounterStart)/countsPerSecond;
}
double GetFrameTime() {
    LARGE_INTEGER currentTime;
    __int64 tickCount;
    QueryPerformanceCounter(&currentTime);

    tickCount = currentTime.QuadPart-frameTimeOld;
    frameTimeOld = currentTime.QuadPart;

    if(tickCount < 0.0f) tickCount = 0.0f;

    return float(tickCount)/countsPerSecond;
}
