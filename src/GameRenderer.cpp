#include "GameRenderer.h"

#include "Common.h"
#include "Renderer.h"

#include <cwchar>
#include <string>

static void DrawTextLine(BackBuffer& backBuffer, HDC dc, const wchar_t* text, int x, int y, int size, COLORREF color,
                         UINT format = DT_LEFT) {
    HFONT font = CreateFontW(size, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                             CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI");
    HFONT oldFont = static_cast<HFONT>(SelectObject(dc, font));
    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, color);
    RECT rect = {x, y, backBuffer.width - x, y + size + 16};
    DrawTextW(dc, text, -1, &rect, format | DT_SINGLELINE);
    SelectObject(dc, oldFont);
    DeleteObject(font);
}

void RenderGame(HDC windowDC, BackBuffer& backBuffer, const Game& game, const PlayerProfile& profile) {
    Renderer renderer{&backBuffer};
    renderer.Clear(RGB32(117, 203, 231));

    int w = backBuffer.width;
    int h = backBuffer.height;
    float groundY = game.groundY;

    renderer.RectFill({0, groundY, static_cast<float>(w), h - groundY}, RGB32(219, 184, 104));
    renderer.RectFill({0, groundY, static_cast<float>(w), 10.0f}, RGB32(91, 178, 88));

    for (int i = -1; i < w / 48 + 2; ++i) {
        float x = i * 48.0f - game.scroll;
        renderer.RectFill({x, groundY + 10.0f, 24.0f, 8.0f}, RGB32(196, 154, 82));
    }

    constexpr float pipeWidth = 68.0f;
    constexpr float gapSize = 170.0f;
    for (const Pipe& pipe : game.pipes) {
        float topHeight = pipe.gapY - gapSize * 0.5f;
        float bottomY = pipe.gapY + gapSize * 0.5f;
        renderer.RectFill({pipe.x, 0.0f, pipeWidth, topHeight}, RGB32(53, 176, 77));
        renderer.RectFill({pipe.x - 6.0f, topHeight - 24.0f, pipeWidth + 12.0f, 24.0f}, RGB32(45, 152, 67));
        renderer.RectFill({pipe.x, bottomY, pipeWidth, groundY - bottomY}, RGB32(53, 176, 77));
        renderer.RectFill({pipe.x - 6.0f, bottomY, pipeWidth + 12.0f, 24.0f}, RGB32(45, 152, 67));
        renderer.RectFill({pipe.x + 8.0f, 0.0f, 9.0f, topHeight - 26.0f}, RGB32(98, 215, 113));
        renderer.RectFill({pipe.x + 8.0f, bottomY + 28.0f, 9.0f, groundY - bottomY - 28.0f}, RGB32(98, 215, 113));
    }

    renderer.CircleFill(game.bird, 18.0f, RGB32(255, 211, 67));
    renderer.CircleFill({game.bird.x + 11.0f, game.bird.y - 7.0f}, 5.0f, RGB32(255, 255, 255));
    renderer.CircleFill({game.bird.x + 13.0f, game.bird.y - 7.0f}, 2.2f, RGB32(35, 34, 42));
    renderer.RectFill({game.bird.x + 14.0f, game.bird.y - 1.0f, 13.0f, 7.0f}, RGB32(237, 124, 47));
    renderer.RectFill({game.bird.x - 19.0f, game.bird.y + 1.0f, 13.0f, 8.0f}, RGB32(241, 177, 42));

    backBuffer.Present(windowDC);

    wchar_t scoreText[64];
    std::swprintf(scoreText, 64, L"%d", game.score);
    DrawTextLine(backBuffer, windowDC, scoreText, 0, 20, 42, RGB(255, 255, 255), DT_CENTER);

    std::wstring playerText = L"Player " + profile.name;
    DrawTextLine(backBuffer, windowDC, playerText.c_str(), 18, 12, 18, RGB(35, 79, 92));

    wchar_t bestText[64];
    std::swprintf(bestText, 64, L"Best %d", game.bestScore);
    DrawTextLine(backBuffer, windowDC, bestText, 18, 34, 20, RGB(35, 79, 92));

    if (!game.started) {
        DrawTextLine(backBuffer, windowDC, L"Space / Click to flap", 0, h / 2 - 28, 28, RGB(255, 255, 255), DT_CENTER);
    } else if (game.gameOver) {
        DrawTextLine(backBuffer, windowDC, L"Game Over", 0, h / 2 - 58, 42, RGB(255, 255, 255), DT_CENTER);
        DrawTextLine(backBuffer, windowDC, L"Space / Click to retry", 0, h / 2 - 8, 24, RGB(255, 255, 255), DT_CENTER);
    }
}
