#pragma once

#include "BackBuffer.h"
#include "Common.h"

struct Renderer {
    BackBuffer* buffer = nullptr;

    void Clear(uint32_t color);
    void RectFill(Rect rect, uint32_t color);
    void CircleFill(Vec2 center, float radius, uint32_t color);
};
