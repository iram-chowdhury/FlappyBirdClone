#pragma once

#include "WindowsConfig.h"

#include <windows.h>

struct BackBuffer {
    BITMAPINFO info = {};
    void* memory = nullptr;
    int width = 0;
    int height = 0;
    int pitch = 0;

    ~BackBuffer();

    void Resize(int newWidth, int newHeight);
    void Present(HDC dc);
};
