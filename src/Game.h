#pragma once

#include "Common.h"

#include <cstdint>

enum class BackgroundColor {
    Blue,
    Red,
    Pink,
    Purple,
    Black,
    Green,
    Yellow
};

enum class AppScreen {
    MainMenu,
    UserSelect,
    Settings,
    Playing,
    Paused,
    GameOver
};

struct Pipe {
    float x = 0.0f;
    float gapY = 0.0f;
    bool scored = false;
};

struct Game {
    Vec2 bird = {160.0f, 260.0f};
    float birdVelocity = 0.0f;
    Pipe pipes[3] = {};
    int score = 0;
    int bestScore = 0;
    bool started = false;
    bool gameOver = false;
    uint32_t rng = 0xC0FFEEu;
    BackgroundColor background = BackgroundColor::Blue;
    AppScreen screen = AppScreen::MainMenu;
    int selectedMenuItem = 0;
    int selectedProfileIndex = 0;

    float groundY = 560.0f;
    float scroll = 0.0f;

    void SetBestScore(int value);
    void Reset(int width, int height);
    void Flap();
    bool Update(float dt, int width, int height);
    void NextBackground();
    void PreviousBackground();

private:
    uint32_t RandomU32();
    float RandomRange(float minValue, float maxValue);
};


