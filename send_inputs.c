#include "send_inputs.h"

// Keyboard

void sendChar(wchar_t character){
    
    INPUT inputs[2] = {};
    ZeroMemory(inputs, sizeof(inputs));

    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wScan = character; 
    inputs[0].ki.dwFlags = KEYEVENTF_UNICODE;

    inputs[1].type = INPUT_KEYBOARD; 
    inputs[1].ki.wScan = character; 
    inputs[1].ki.dwFlags = KEYEVENTF_KEYUP  | KEYEVENTF_UNICODE;

    UINT uSent = SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
    if (uSent != ARRAYSIZE(inputs))
    {
        printf("SendInput failed: 0x%x\n", HRESULT_FROM_WIN32(GetLastError()));
    } 
}

void sendKey(WORD keycode){
    INPUT inputs[1] = {};
    ZeroMemory(inputs, sizeof(inputs));

    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = keycode;

    UINT uSent = SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
    if (uSent != ARRAYSIZE(inputs))
    {
        printf("SendInput failed: 0x%x\n", HRESULT_FROM_WIN32(GetLastError()));
    } 
}

void sendKeyEnd(WORD keycode){
    INPUT inputs[1] = {};
    ZeroMemory(inputs, sizeof(inputs));

    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = keycode;
    inputs[0].ki.dwFlags = KEYEVENTF_KEYUP;

    UINT uSent = SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
    if (uSent != ARRAYSIZE(inputs))
    {
        printf("SendInput failed: 0x%x\n", HRESULT_FROM_WIN32(GetLastError()));
    } 
}

void mouseTo(int x, int y){
    INPUT inputs[1] = {};
    ZeroMemory(inputs, sizeof(inputs));

    inputs[0].type = INPUT_MOUSE;
    inputs[0].mi.dx = x;
    inputs[0].mi.dy = y;
    inputs[0].mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;

    UINT uSent = SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
    if (uSent != ARRAYSIZE(inputs))
    {
        printf("SendInput failed: 0x%x\n", HRESULT_FROM_WIN32(GetLastError()));
    }     
}

void mouseMove(int x, int y){
    INPUT inputs[1] = {};
    ZeroMemory(inputs, sizeof(inputs));

    inputs[0].type = INPUT_MOUSE;
    inputs[0].mi.dx = x;
    inputs[0].mi.dy = y;
    inputs[0].mi.dwFlags = MOUSEEVENTF_MOVE;

    UINT uSent = SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
    if (uSent != ARRAYSIZE(inputs))
    {
        printf("SendInput failed: 0x%x\n", HRESULT_FROM_WIN32(GetLastError()));
    }     
}

void mouseDown(int button){
    INPUT inputs[1] = {};
    ZeroMemory(inputs, sizeof(inputs));

    inputs[0].type = INPUT_MOUSE;
    switch(button){
        case VK_LBUTTON: inputs[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;break;
        case VK_RBUTTON: inputs[0].mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;break;
        case VK_MBUTTON: inputs[0].mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;break;
    }

    UINT uSent = SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
    if (uSent != ARRAYSIZE(inputs))
    {
        printf("SendInput failed: 0x%x\n", HRESULT_FROM_WIN32(GetLastError()));
    }
}

void mouseUp(int button){
    INPUT inputs[1] = {};
    ZeroMemory(inputs, sizeof(inputs));

    inputs[0].type = INPUT_MOUSE;
    switch(button){
        case VK_LBUTTON: inputs[0].mi.dwFlags = MOUSEEVENTF_LEFTUP;break;
        case VK_RBUTTON: inputs[0].mi.dwFlags = MOUSEEVENTF_RIGHTUP;break;
        case VK_MBUTTON: inputs[0].mi.dwFlags = MOUSEEVENTF_MIDDLEUP;break;
    }

    UINT uSent = SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
    if (uSent != ARRAYSIZE(inputs))
    {
        printf("SendInput failed: 0x%x", HRESULT_FROM_WIN32(GetLastError()));
    }
}

void mouseClick(int button){
    mouseDown(button);
    mouseUp(button);    
}

void mouseScroll(int amount){
    INPUT inputs[1] = {};
    ZeroMemory(inputs, sizeof(inputs));

    inputs[0].type = INPUT_MOUSE;
    inputs[0].mi.dwFlags = MOUSEEVENTF_WHEEL;
    inputs[0].mi.mouseData = amount;

    UINT uSent = SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
    if (uSent != ARRAYSIZE(inputs))
    {
        printf("SendInput failed: 0x%x", HRESULT_FROM_WIN32(GetLastError()));
    }
}