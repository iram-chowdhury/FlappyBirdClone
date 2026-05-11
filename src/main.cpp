#include "WindowsConfig.h"

#include "BackBuffer.h"
#include "Game.h"
#include "GameRenderer.h"
#include "Profile.h"

#include <windows.h>
#include <mmsystem.h>

#include <algorithm>

static BackBuffer gBackBuffer;
static Game gGame;
static ProfileStore gProfileStore;
static PlayerProfile gProfile;
static bool gRunning = true;
static bool gInitialized = false;

static void SaveHighScoreIfNeeded() {
    if (gGame.bestScore > gProfile.bestScore) {
        gProfile.bestScore = gGame.bestScore;
        gProfileStore.SaveProfile(gProfile);
    }
}

static LRESULT CALLBACK WindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_SIZE: {
            RECT rect;
            GetClientRect(window, &rect);
            gBackBuffer.Resize(rect.right - rect.left, rect.bottom - rect.top);
            if (!gInitialized) {
                gGame.SetBestScore(gProfile.bestScore);
                gGame.Reset(gBackBuffer.width, gBackBuffer.height);
                gInitialized = true;
            }
        } break;
        case WM_KEYDOWN:
            if (wParam == VK_SPACE || wParam == VK_UP) {
                gGame.Flap();
            } else if (wParam == VK_ESCAPE) {
                gRunning = false;
                PostQuitMessage(0);
            }
            else if (wParam == 'B') {
                if (gGame.background == BackgroundColor::Blue) {
                    gGame.background = BackgroundColor::Red;
                }
                else {
                    gGame.background = BackgroundColor::Blue;
                }
            }
            break;
        case WM_LBUTTONDOWN:
            gGame.Flap();
            break;
        case WM_CLOSE:
        case WM_DESTROY:
            SaveHighScoreIfNeeded();
            gRunning = false;
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProcW(window, message, wParam, lParam);
    }
    return 0;
}

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int) {
    gProfile = gProfileStore.LoadActiveProfile();

    timeBeginPeriod(1);

    WNDCLASSW windowClass = {};
    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = WindowProc;
    windowClass.hInstance = instance;
    windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
    windowClass.lpszClassName = L"FlappyBirdContWindowClass";

    RegisterClassW(&windowClass);

    HWND window = CreateWindowExW(0, windowClass.lpszClassName, L"Flappy Bird Clone by Aryan",
                                  WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
                                  CW_USEDEFAULT, CW_USEDEFAULT, 540, 720,
                                  nullptr, nullptr, instance, nullptr);
    if (!window) {
        timeEndPeriod(1);
        return 1;
    }

    ShowWindow(window, SW_SHOW);

    LARGE_INTEGER frequency;
    LARGE_INTEGER lastCounter;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&lastCounter);

    while (gRunning) {
        MSG message;
        while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&message);
            DispatchMessageW(&message);
        }

        LARGE_INTEGER counter;
        QueryPerformanceCounter(&counter);
        float dt = static_cast<float>(counter.QuadPart - lastCounter.QuadPart) / static_cast<float>(frequency.QuadPart);
        lastCounter = counter;
        dt = std::min(dt, 1.0f / 20.0f);

        if (gGame.Update(dt, gBackBuffer.width, gBackBuffer.height)) {
            SaveHighScoreIfNeeded();
        }

        HDC dc = GetDC(window);
        RenderGame(dc, gBackBuffer, gGame, gProfile);
        ReleaseDC(window, dc);

        Sleep(1);
    }

    SaveHighScoreIfNeeded();
    timeEndPeriod(1);
    return 0;
}
