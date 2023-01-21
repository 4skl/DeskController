#ifndef HEADER_SEND_INPUTS
#define HEADER_SEND_INPUTS
// todo add unix version of it
#include <Windows.h>
#include <stdio.h>

void sendChar(wchar_t character);
void sendKey(WORD keycode);
void sendKeyEnd(WORD keycode);

#endif