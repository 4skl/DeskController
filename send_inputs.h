#ifndef HEADER_SEND_INPUTS
#define HEADER_SEND_INPUTS

#include <stdio.h>
// todo add unix version of it
#include <Windows.h>

// Keyboards
void sendChar(wchar_t character);
void sendKey(WORD keycode);
void sendKeyEnd(WORD keycode);

// Mouse
void mouseTo(int x, int y); // Absolute move
void mouseMove(int x, int y);

void mouseDown(int button);
void mouseUp(int button);

void mouseClick(int button);

void mouseScroll(int amount);

#endif