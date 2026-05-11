#pragma once

#include "BackBuffer.h"
#include "Game.h"
#include "Profile.h"

#include "WindowsConfig.h"

#include <windows.h>

void RenderGame(HDC windowDC, BackBuffer& backBuffer, const Game& game, const PlayerProfile& profile);
