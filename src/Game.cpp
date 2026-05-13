#include "Game.h"
#include "Audio.h"
#include "WindowsConfig.h"

#include <windows.h>

#include <algorithm>
#include <cmath>

void Game::SetBestScore(int value) {
    bestScore = std::max(0, value);
}

uint32_t Game::RandomU32() {
    rng ^= rng << 13;
    rng ^= rng >> 17;
    rng ^= rng << 5;
    return rng;
}

float Game::RandomRange(float minValue, float maxValue) {
    float t = static_cast<float>(RandomU32() & 0xFFFFu) / 65535.0f;
    return minValue + (maxValue - minValue) * t;
}

void Game::Reset(int width, int height) {
    bird = {width * 0.32f, height * 0.45f};
    birdVelocity = 0.0f;
    score = 0;
    started = false;
    gameOver = false;
    groundY = height - 64.0f;
    scroll = 0.0f;

    float spacing = 260.0f;
    float firstX = width + 120.0f;
    for (int i = 0; i < 3; ++i) {
        pipes[i].x = firstX + i * spacing;
        pipes[i].gapY = RandomRange(155.0f, groundY - 150.0f);
        pipes[i].scored = false;
    }
}

void Game::Flap() {
    if (gameOver) {
        Reset(static_cast<int>(bird.x / 0.32f), static_cast<int>(groundY + 64.0f));
    }
    started = true;
    birdVelocity = -365.0f;
}

bool Game::Update(float dt, int, int height) {
    bool highScoreChanged = false;

    groundY = height - 64.0f;
    if (!started || gameOver) {
        bird.y += std::sinf(GetTickCount64() * 0.004f) * 0.18f;
        return false;
    }

    constexpr float gravity = 980.0f;
    constexpr float pipeSpeed = 165.0f;
    constexpr float pipeWidth = 68.0f;
    constexpr float gapSize = 170.0f;

    birdVelocity += gravity * dt;
    bird.y += birdVelocity * dt;
    scroll = std::fmod(scroll + pipeSpeed * dt, 48.0f);

    float rightmost = 0.0f;
    for (const Pipe& pipe : pipes) {
        rightmost = std::max(rightmost, pipe.x);
    }

    Rect birdBody = {bird.x - 18.0f, bird.y - 16.0f, 36.0f, 32.0f};
    if (birdBody.y < 0.0f || birdBody.y + birdBody.h > groundY) {
        gameOver = true;
        PlayHitSound();
    }

    for (Pipe& pipe : pipes) {
        pipe.x -= pipeSpeed * dt;
        if (pipe.x + pipeWidth < 0.0f) {
            pipe.x = rightmost + 260.0f;
            rightmost = pipe.x;
            pipe.gapY = RandomRange(135.0f, groundY - 145.0f);
            pipe.scored = false;
        }

        Rect top = {pipe.x, 0.0f, pipeWidth, pipe.gapY - gapSize * 0.5f};
        Rect bottom = {pipe.x, pipe.gapY + gapSize * 0.5f, pipeWidth, groundY - (pipe.gapY + gapSize * 0.5f)};
        if (Intersects(birdBody, top) || Intersects(birdBody, bottom)) {
            gameOver = true;
            PlayHitSound();
        }

        if (!pipe.scored && pipe.x + pipeWidth < bird.x) {
            pipe.scored = true;
            ++score;
            PlayScoreSound();
            if (score > bestScore) {
                bestScore = score;
                highScoreChanged = true;
            }
        }
    }

    return highScoreChanged;
}

void Game::NextBackground() {

    if (background == BackgroundColor::Blue) {
        background = BackgroundColor::Red;
    }
    else if (background == BackgroundColor::Red) {
        background = BackgroundColor::Pink;
    }
    else if (background == BackgroundColor::Pink) {
        background = BackgroundColor::Purple;
    }
    else if (background == BackgroundColor::Purple) {
        background = BackgroundColor::Black;
    }
    else if (background == BackgroundColor::Black) {
        background = BackgroundColor::Green;
    }
    else if (background == BackgroundColor::Green) {
        background = BackgroundColor::Yellow;
    }
    else {
        background = BackgroundColor::Blue;
    }
}

void Game::PreviousBackground() {

    if (background == BackgroundColor::Blue) {
        background = BackgroundColor::Yellow;
    }
    else if (background == BackgroundColor::Yellow) {
        background = BackgroundColor::Green;
    }
    else if (background == BackgroundColor::Green) {
        background = BackgroundColor::Black;
    }
    else if (background == BackgroundColor::Black) {
        background = BackgroundColor::Purple;
    }
    else if (background == BackgroundColor::Purple) {
        background = BackgroundColor::Pink;
    }
    else if (background == BackgroundColor::Pink) {
        background = BackgroundColor::Red;
    }
    else {
        background = BackgroundColor::Blue;
    }
}