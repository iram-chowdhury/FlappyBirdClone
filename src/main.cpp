#include "WindowsConfig.h"

#include "BackBuffer.h"
#include "Game.h"
#include "GameRenderer.h"
#include "Profile.h"
#include "Audio.h"

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
    case WM_MOUSEMOVE: {

        gGame.mouseX = LOWORD(lParam);
        gGame.mouseY = HIWORD(lParam);

        if (gGame.screen == AppScreen::MainMenu) {

            int mouseY = gGame.mouseY;

            if (mouseY >= 250 && mouseY <= 330) {
                gGame.selectedMenuItem = 0; // Start 
            }
            else if (mouseY >= 330 && mouseY <= 360) {
                gGame.selectedMenuItem = 1; // User 
            }
            else if (mouseY >= 360 && mouseY <= 400) {
                gGame.selectedMenuItem = 2; // Settings
            }
        }

        if (gGame.screen == AppScreen::Paused) {

            int mouseY = gGame.mouseY;

            if (mouseY >= 250 && mouseY <= 330) {
                gGame.selectedMenuItem = 0; // Continue
            }
            else if (mouseY >= 330 && mouseY <= 360) {
                gGame.selectedMenuItem = 1; // Settings
            }
            else if (mouseY >= 360 && mouseY <= 400) {
                gGame.selectedMenuItem = 2; // End
            }
        }

        if (gGame.screen == AppScreen::UserSelect && !gGame.renamingProfile) {

            int startY = gBackBuffer.height / 2 - 70;

            for (int i = 0; i < static_cast<int>(gProfiles.size()); ++i) {
                int rowTop = startY + i * 34;
                int rowBottom = rowTop + 34;

                if (gGame.mouseY >= rowTop && gGame.mouseY <= rowBottom) {
                    gGame.selectedProfileIndex = i;
                    break;
                }
            }
        }

    } break;

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
                PlayMenuSelectSound();

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
                PlayMenuSelectSound();

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

            else if (wParam == 'M') {
                gGame.soundMuted = !gGame.soundMuted;
                SetSoundMuted(gGame.soundMuted);
            }

            break;
        }

        if (gGame.screen == AppScreen::Playing &&
            (wParam == VK_SPACE || wParam == VK_UP)) {
            gGame.Flap();
            PlayFlapSound();
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

            if (gGame.renamingProfile) {

                if (wParam == VK_RETURN) {
                    PlayMenuSelectSound();

                    if (!gGame.renameBuffer.empty()) {

                        std::wstring oldName =
                            gProfiles[gGame.selectedProfileIndex].name;

                        std::wstring newName =
                            gGame.renameBuffer;

                        gProfileStore.RenameProfile(oldName, newName);

                        gProfiles[gGame.selectedProfileIndex].name =
                            newName;

                        if (gProfile.name == oldName) {
                            gProfile.name = newName;
                        }
                    }

                    gGame.renamingProfile = false;
                    gGame.renameBuffer.clear();
                }

                else if (wParam == VK_ESCAPE) {

                    gGame.renamingProfile = false;
                    gGame.renameBuffer.clear();
                }

                else if (wParam == VK_BACK) {

                    if (!gGame.renameBuffer.empty()) {
                        gGame.renameBuffer.pop_back();
                    }
                }

                else if (wParam >= 'A' && wParam <= 'Z') {

                    gGame.renameBuffer.push_back(
                        static_cast<wchar_t>(wParam));
                }

                else if (wParam >= '0' && wParam <= '9') {

                    gGame.renameBuffer.push_back(
                        static_cast<wchar_t>(wParam));
                }

                else if (wParam == VK_SPACE) {

                    gGame.renameBuffer.push_back(L' ');
                }

                break;
            }

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
                PlayMenuSelectSound();

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

            else if (wParam == 'R') {

                gGame.renamingProfile = true;

                gGame.renameBuffer =
                    gProfiles[gGame.selectedProfileIndex].name;
            }

            break;
        }
    } break;

    case WM_LBUTTONDOWN:
        if (gGame.screen == AppScreen::Playing) {
            gGame.Flap();
            PlayFlapSound();
        }

        if (gGame.screen == AppScreen::MainMenu) {

            PlayMenuSelectSound();

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

            break;
        }

        if (gGame.screen == AppScreen::Paused) {

            PlayMenuSelectSound();

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

            break;
        }

        if (gGame.screen == AppScreen::UserSelect && !gGame.renamingProfile) {

            PlayMenuSelectSound();

            int startY = gBackBuffer.height / 2 - 70;

            for (int i = 0; i < static_cast<int>(gProfiles.size()); ++i) {
                int rowTop = startY + i * 34;
                int rowBottom = rowTop + 34;

                if (gGame.mouseY >= rowTop && gGame.mouseY <= rowBottom) {
                    gGame.selectedProfileIndex = i;
                    gProfile = gProfiles[i];
                    gGame.SetBestScore(gProfile.bestScore);
                    gGame.screen = AppScreen::MainMenu;
                    gGame.selectedMenuItem = 0;
                    break;
                }
            }

            break;
        }

        if (gGame.screen == AppScreen::Settings) {

            PlayMenuSelectSound();

            int mouseX = gGame.mouseX;
            int mouseY = gGame.mouseY;

            int centerY = gBackBuffer.height / 2;

            // Background row
            if (mouseY >= centerY - 55 && mouseY <= centerY - 5) {

                if (mouseX < gBackBuffer.width / 2) {
                    gGame.PreviousBackground();
                }
                else {
                    gGame.NextBackground();
                }
            }

            // Sound row
            else if (mouseY >= centerY && mouseY <= centerY + 45) {

                gGame.soundMuted = !gGame.soundMuted;
                SetSoundMuted(gGame.soundMuted);
            }

            // Back row
            else if (mouseY >= centerY + 80 && mouseY <= centerY + 125) {

                gGame.screen = AppScreen::MainMenu;
            }

            break;
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
