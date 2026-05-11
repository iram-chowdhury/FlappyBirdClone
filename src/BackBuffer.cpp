#include "BackBuffer.h"

#include <algorithm>

BackBuffer::~BackBuffer() {
    if (memory) {
        VirtualFree(memory, 0, MEM_RELEASE);
        memory = nullptr;
    }
}

void BackBuffer::Resize(int newWidth, int newHeight) {
    width = std::max(1, newWidth);
    height = std::max(1, newHeight);
    pitch = width * 4;

    if (memory) {
        VirtualFree(memory, 0, MEM_RELEASE);
        memory = nullptr;
    }

    info = {};
    info.bmiHeader.biSize = sizeof(info.bmiHeader);
    info.bmiHeader.biWidth = width;
    info.bmiHeader.biHeight = -height;
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = 32;
    info.bmiHeader.biCompression = BI_RGB;

    memory = VirtualAlloc(nullptr, static_cast<size_t>(pitch) * height, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
}

void BackBuffer::Present(HDC dc) {
    StretchDIBits(dc, 0, 0, width, height, 0, 0, width, height, memory, &info, DIB_RGB_COLORS, SRCCOPY);
}
