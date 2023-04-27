# DeskController
Maping a keyboard and a mouse on a gamepad, using GLFW, with an intuitive overlay

This project replace [this one](https://github.com/4skl/GameControllerWindowsAdapter), with a cleaner code, build, various gamepad support and more.
Windows and Unix support is targetted, for now, only Windows is supported.

# Disclaimer
The code is at it's begining, so nothing fully functional yet, please wait and notice me if you are interested in it !

Curent level of prototyping : ![desk controller demo gif](https://github.com/4skl/DeskController/blob/main/devlog/WheelAndMouseDemo.gif?raw=true)

# Functionalities
- Write text using wheel (and others entries : ctrl, backspace, escape,...)

- Move, click and scroll mouse

# Future Improvments

- [ ] Colorize when a keystroke is sent
- [ ] Show when special keys are pressed (ctrl, alt, shift, ...)
- [ ] Add alt and other keys support
- [ ] Add middle click mouse support
- [ ] Moving colors overlay
- [ ] File for settings (colors, sizes, ...)
- [ ] Add a way to customize the mapping from a file/overlay
- [ ] Create a way to add new functionalities (like a macro system)

# Build & Depencies

You need glew, opengl, glfw and gdi.

## Compiling on Windows (tested with MinGW-w64) :
> make

Run :
> ./DeskController.exe
