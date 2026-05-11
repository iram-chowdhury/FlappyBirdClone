# Flappy Bird Clone v0.3 

A tiny Flappy Bird-style game written in C++ with a handmade Windows game loop and software renderer.

No external game engine is used. The project talks directly to the Win32 API, draws into a custom 32-bit back buffer, and handles its own timing, input, collision, scoring, and pipe spawning.

High scores are saved locally per Windows username in `build\data\profiles.txt`.

## Controls

UP/DOWN     = move menu selection
ENTER       = choose option
ESC         = pause / back
SPACE/CLICK = flap only while Playing


## v0.3 Updates 
- Added a main menu 
- Added a pause menu using "ESC" on the keyboard 
- Added a settings menu where you can change background colour 
- Added new background colors (Currently blue, red, pink, purple, black, green, yellow) 

## v0.2 Updates 
- Added the option to change background colour using "B" on the keyboard (Red or blue) 

