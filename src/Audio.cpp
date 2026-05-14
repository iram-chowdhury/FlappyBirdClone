#include "Audio.h"

#include <windows.h>
#include <mmsystem.h>
#include <string>

#pragma comment(lib, "winmm.lib")

static bool gSoundMuted = false;

void SetSoundMuted(bool muted) {
    gSoundMuted = muted;
}

bool IsSoundMuted() {
    return gSoundMuted;
}

static std::wstring GetExeDirectory() {
    wchar_t path[MAX_PATH] = {};
    GetModuleFileNameW(nullptr, path, MAX_PATH);

    std::wstring fullPath = path;
    size_t slash = fullPath.find_last_of(L"\\/");

    if (slash == std::wstring::npos) {
        return L".";
    }

    return fullPath.substr(0, slash);
}

static void PlaySimpleSound(const wchar_t* filename) {
    if (gSoundMuted) {
        return;
    }

    std::wstring path = GetExeDirectory() + L"\\data\\sounds\\" + filename;

    BOOL success = PlaySoundW(
        path.c_str(),
        nullptr,
        SND_FILENAME | SND_ASYNC | SND_NODEFAULT
    );

    if (!success) {
        OutputDebugStringW(L"Sound failed to play:O ");
        OutputDebugStringW(path.c_str());
        OutputDebugStringW(L"\n");
    }
}

void PlayFlapSound() {
    PlaySimpleSound(L"flap.wav");
}

void PlayScoreSound() {
    PlaySimpleSound(L"score.wav");
}

void PlayHitSound() {
    PlaySimpleSound(L"hit.wav");
}

void PlayMenuMoveSound() {
    PlaySimpleSound(L"menu_move.wav");
}

void PlayMenuSelectSound() {
    PlaySimpleSound(L"menu_select.wav");
}