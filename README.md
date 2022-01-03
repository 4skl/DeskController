# DeskController
Maping a keyboard and a mouse on a gamepad, using GLFW, with an intuitive overlay

This project replace [this one](https://github.com/4skl/GameControllerWindowsAdapter), with a cleaner code, build, various gamepad support and more.
Windows and Unix support is targetted, for now, only Windows is supported.

# Disclaimer
The code is at it's begining, so nothing functional yet, please wait and notice me if you are interested in it !

# Build & Depencies

You need glew, opengl, glfw and gdi.

## Compiling on Windows (tested with MinGW-w64) :
> gcc main.c -lglew32 -lglfw3 -lgdi32 -lopengl32 -o DeskController

*makefile will be added soon
