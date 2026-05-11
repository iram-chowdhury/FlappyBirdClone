# Flappy Bird Cont.

A tiny Flappy Bird-style game written in C++ with a handmade Windows game loop and software renderer.

No external game engine is used. The project talks directly to the Win32 API, draws into a custom 32-bit back buffer, and handles its own timing, input, collision, scoring, and pipe spawning.

High scores are saved locally per Windows username in `build\data\profiles.txt`.

## Build

### Visual Studio

Open `FlappyBirdCont.sln`, not just the folder.

Visual Studio should show `Flappy Bird Cont.` as the startup project. Press the green Local Windows Debugger button to build and run.

If Visual Studio says the project cannot load, install the `Desktop development with C++` workload from the Visual Studio Installer.

### PowerShell

```powershell
.\build.ps1
```

The executable will be created at:

```text
build\FlappyBirdCont.exe
```

## Controls

- Space, Up Arrow, or left mouse click: flap / start / retry
- Escape: quit

## Source Layout

- `src\main.cpp`: Windows startup, window messages, and the main loop
- `src\Game.*`: bird, pipes, collision, scoring, and game state
- `src\GameRenderer.*`: all game drawing and on-screen text
- `src\BackBuffer.*` and `src\Renderer.*`: software drawing support
- `src\Profile.*`: local player profile and high-score saving
- `src\Common.h`: shared small types like rectangles, vectors, and colors

## What Is Handmade

- Fixed-window Win32 app setup
- Game loop and frame timing
- Software pixel back buffer
- Rectangle and circle drawing
- Input handling
- Pipe movement and respawning
- Collision and scoring
