#ifndef HEADER_OVERLAY
#define HEADER_OVERLAY

#include <load_shader.h>
#include <common.h>
#include <GLFW/glfw3.h>

struct OverlaySettings{
    uint8_t widthPercent;
    uint8_t heightPercent;
    ColorRGBf backgroundColor;
    ColorRGBf foregroundColor;
    Side side;
};
typedef struct OverlaySettings OverlaySettings;

OverlaySettings defaultSettings = {
    .widthPercent=25, 
    .heightPercent=10, 
    .backgroundColor={.r=1, .g=0.90, .b=0.28},
    .foregroundColor={.r=0.45, .g=0.38, .b=1},
    .side=TOP
};

GLFWwindow* createOverlay(GLFWmonitor* monitor, OverlaySettings* settings);

#endif