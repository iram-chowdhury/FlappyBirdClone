# Flappy Bird Clone v0.7

A tiny Flappy Bird-style game written in C++ with a handmade Windows game loop and software renderer.

No external game engine is used. The project talks directly to the Win32 API, draws into a custom 32-bit back buffer, and handles its own timing, input, collision, scoring, and pipe spawning.

High scores are saved locally per Windows username in `build\data\profiles.txt`.

## Controls

UP/DOWN     = move menu selection

LEFT/RIGHT = in settings screen changes background colour

ENTER       = choose option

ESC         = pause / back

SPACE/CLICK = flap only while Playing

R = in user select allows you to rename user

D = in user select allows you to delete user

N = in user select allows you to create user

M = in settings to toggle sound

## v0.7 Updates
- Added full mouse hover support for: Main Menu, Pause Menu & User Select
- Added full mouse click support for: Main Menu, Pause Menu, Settings & User Select 
- Added live cursor-based menu selection
- Added clickable background selection in settings
- Added sound mute system that can be toggled on or off in settings or using mouse click (also has a dynamic sound on / off display) 
- Improved overall UI responsiveness & menu navigation
- Menu interaction system now supports both keyboard & mouse support yippee

## v0.6 Updates 
- Added sound effects to the game (Flap, Score, Die & Menu Select)
- Sounds were obtained from https://pixabay.com/
- Used Audacity to convert from .mp3 to .wav so I could use PlaySoundW

## v0.5 Updates 
- Added rename functionality in user select allowing user input (R in user select) 

## v0.4 Updates
| Bug Fixes |
- Pause actually pauses the game
- Continue resumes from the same position
- End returns to main menu 

| Features |
- Added local users with the option to create & delete users (no longer using Windows username) 
UP / DOWN navigates through profiles, ENTER selects active profile, N creates a new user, D deletes user & ESC returns back to main menu
- Added a seperate high score for each local user that is tracked in data/profiles.txt 
- Makes sure that deleting selected profile stops you from deleting if there's only one profile left 


## v0.3 Updates 
- Added a main menu 
- Added a pause menu using "ESC" on the keyboard 
- Added a settings menu where you can change background colour instead 
- Added new background colors (Currently blue, red, pink, purple, black, green, yellow) 

## v0.2 Updates 
- Added the option to change background colour using "B" on the keyboard (Red or blue) 

