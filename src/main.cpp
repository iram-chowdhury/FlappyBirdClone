#include "WindowsConfig.h"

#include "BackBuffer.h"
#include "Game.h"
#include "GameRenderer.h"
#include "Profile.h"

#include <windows.h>
#include <mmsystem.h>
#include <vector>

#include <algorithm>

static BackBuffer gBackBuffer;
static Game gGame;
static ProfileStore gProfileStore;
static PlayerProfile gProfile;
static bool gRunning = true;
static bool gInitialized = false;
static std::vector<PlayerProfile> gProfiles;

static void SaveHighScoreIfNeeded() {
    if (gGame.bestScore > gProfile.bestScore) {
        gProfile.bestScore = gGame.bestScore;
        gProfileStore.SaveProfile(gProfile);
    }
}

static void CreateNewProfile() {
    int number = static_cast<int>(gProfiles.size()) + 1;

    PlayerProfile newProfile;
    newProfile.name = L"Player " + std::to_wstring(number);
    newProfile.bestScore = 0;

    gProfiles.push_back(newProfile);
    gProfileStore.SaveProfile(newProfile);

    gGame.selectedProfileIndex = static_cast<int>(gProfiles.size()) - 1;
}

static void DeleteSelectedProfile() {
    if (gProfiles.size() <= 1) {
        return;
    }

    int index = gGame.selectedProfileIndex;

    gProfileStore.DeleteProfile(gProfiles[index].name);

    gProfiles.erase(gProfiles.begin() + index);

    if (gGame.selectedProfileIndex >= static_cast<int>(gProfiles.size())) {
        gGame.selectedProfileIndex = static_cast<int>(gProfiles.size()) - 1;
    }

    gProfile = gProfiles[gGame.selectedProfileIndex];
    gGame.SetBestScore(gProfile.bestScore);
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

    case WM_KEYDOWN: {
        if (gGame.screen == AppScreen::MainMenu) {
            if (wParam == VK_UP) {
                --gGame.selectedMenuItem;
                if (gGame.selectedMenuItem < 0) {
                    gGame.selectedMenuItem = 2;
                }
            }
            else if (wParam == VK_DOWN) {
                ++gGame.selectedMenuItem;
                if (gGame.selectedMenuItem > 2) {
                    gGame.selectedMenuItem = 0;
                }
            }
            else if (wParam == VK_RETURN) {
                if (gGame.selectedMenuItem == 0) {
                    gGame.Reset(gBackBuffer.width, gBackBuffer.height);
                    gGame.screen = AppScreen::Playing;
                }
                else if (gGame.selectedMenuItem == 1) {
                    gGame.screen = AppScreen::UserSelect;
                }
                else if (gGame.selectedMenuItem == 2) {
                    gGame.screen = AppScreen::Settings;
                }
            }
            break;
        }

        if (gGame.screen == AppScreen::Paused) {

            if (wParam == VK_UP) {
                --gGame.selectedMenuItem;

                if (gGame.selectedMenuItem < 0) {
                    gGame.selectedMenuItem = 2;
                }
            }

            else if (wParam == VK_DOWN) {
                ++gGame.selectedMenuItem;

                if (gGame.selectedMenuItem > 2) {
                    gGame.selectedMenuItem = 0;
                }
            }

            else if (wParam == VK_RETURN) {

                if (gGame.selectedMenuItem == 0) {
                    gGame.screen = AppScreen::Playing;
                }

                else if (gGame.selectedMenuItem == 1) {
                    gGame.screen = AppScreen::Settings;
                }

                else if (gGame.selectedMenuItem == 2) {
                    gGame.screen = AppScreen::MainMenu;

                    gGame.Reset(gBackBuffer.width, gBackBuffer.height);
                    gGame.selectedMenuItem = 0;
                }
            }

            break;
        }

        if (gGame.screen == AppScreen::Settings) {

            if (wParam == VK_LEFT) {
                gGame.PreviousBackground();
            }

            else if (wParam == VK_RIGHT) {
                gGame.NextBackground();
            }

            else if (wParam == VK_ESCAPE) {
                gGame.screen = AppScreen::MainMenu;
            }

            break;
        }

        if (gGame.screen == AppScreen::Playing &&
            (wParam == VK_SPACE || wParam == VK_UP)) {
            gGame.Flap();
        }
        else if (wParam == VK_ESCAPE) {
            if (gGame.screen == AppScreen::Playing) {
                gGame.screen = AppScreen::Paused;
                gGame.selectedMenuItem = 0;
            }
            else if (gGame.screen == AppScreen::Paused) {
                gGame.screen = AppScreen::Playing;
            }
        }
        if (gGame.screen == AppScreen::UserSelect) {

            if (wParam == VK_UP) {

                --gGame.selectedProfileIndex;

                if (gGame.selectedProfileIndex < 0) {
                    gGame.selectedProfileIndex =
                        static_cast<int>(gProfiles.size()) - 1;
                }
            }

            else if (wParam == VK_DOWN) {

                ++gGame.selectedProfileIndex;

                if (gGame.selectedProfileIndex >=
                    static_cast<int>(gProfiles.size())) {

                    gGame.selectedProfileIndex = 0;
                }
            }

            else if (wParam == VK_RETURN) {

                if (!gProfiles.empty()) {

                    gProfile =
                        gProfiles[gGame.selectedProfileIndex];

                    gGame.SetBestScore(gProfile.bestScore);

                    gGame.screen = AppScreen::MainMenu;
                    gGame.selectedMenuItem = 0;
                }
            }

            else if (wParam == VK_ESCAPE) {

                gGame.screen = AppScreen::MainMenu;
                gGame.selectedMenuItem = 0;
            }

            else if (wParam == 'N') {
                CreateNewProfile();
            }

            else if (wParam == 'D') {
                DeleteSelectedProfile();
            }

            break;
        }
    } break;

    case WM_LBUTTONDOWN:
        if (gGame.screen == AppScreen::Playing) {
            gGame.Flap();
        }
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
    gProfiles = gProfileStore.LoadProfiles();

    if (!gProfiles.empty()) {
        gProfile = gProfiles[0];
    }
    else {
        gProfile.name = L"Player 1";
        gProfile.bestScore = 0;
    }

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

        if (gGame.screen == AppScreen::Playing) {
            if (gGame.Update(dt, gBackBuffer.width, gBackBuffer.height)) {
                SaveHighScoreIfNeeded();
            }
        }

        HDC dc = GetDC(window);
        RenderGame(dc, gBackBuffer, gGame, gProfile, gProfiles);
        ReleaseDC(window, dc);

        Sleep(1);
    }

    SaveHighScoreIfNeeded();
    timeEndPeriod(1);
    return 0;
}
