#include "Renderer.h"

#include <algorithm>
#include <cmath>

void Renderer::Clear(uint32_t color) {
    uint32_t* pixels = static_cast<uint32_t*>(buffer->memory);
    for (int y = 0; y < buffer->height; ++y) {
        for (int x = 0; x < buffer->width; ++x) {
            pixels[y * buffer->width + x] = color;
        }
    }
}

void Renderer::RectFill(Rect rect, uint32_t color) {
    int minX = std::max(0, static_cast<int>(std::floor(rect.x)));
    int minY = std::max(0, static_cast<int>(std::floor(rect.y)));
    int maxX = std::min(buffer->width, static_cast<int>(std::ceil(rect.x + rect.w)));
    int maxY = std::min(buffer->height, static_cast<int>(std::ceil(rect.y + rect.h)));

    uint32_t* pixels = static_cast<uint32_t*>(buffer->memory);
    for (int y = minY; y < maxY; ++y) {
        for (int x = minX; x < maxX; ++x) {
            pixels[y * buffer->width + x] = color;
        }
    }
}

void Renderer::CircleFill(Vec2 center, float radius, uint32_t color) {
    int minX = std::max(0, static_cast<int>(std::floor(center.x - radius)));
    int minY = std::max(0, static_cast<int>(std::floor(center.y - radius)));
    int maxX = std::min(buffer->width, static_cast<int>(std::ceil(center.x + radius)));
    int maxY = std::min(buffer->height, static_cast<int>(std::ceil(center.y + radius)));
    float radiusSq = radius * radius;

    uint32_t* pixels = static_cast<uint32_t*>(buffer->memory);
    for (int y = minY; y < maxY; ++y) {
        for (int x = minX; x < maxX; ++x) {
            float dx = (x + 0.5f) - center.x;
            float dy = (y + 0.5f) - center.y;
            if (dx * dx + dy * dy <= radiusSq) {
                pixels[y * buffer->width + x] = color;
            }
        }
    }
}
