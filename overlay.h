#ifndef HEADER_OVERLAY
#define HEADER_OVERLAY

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <load_shader.h>
#include <common.h>

struct OverlaySettings{
    uint8_t sizeFactor;
    ColorRGBf backgroundColor;
    ColorRGBf foregroundColor;
    Side side;
};
typedef struct OverlaySettings OverlaySettings;

GLFWwindow* createOverlayWindow(GLFWmonitor* monitor, OverlaySettings* settings);

void drawOverlayBackground();
GLuint compileOverlayBackground();
void createOverlayBackground(UsableShaderData* shaderData);

void drawOverlayWheel();
GLuint compileOverlayWheel();
void createOverlayWheel(UsableShaderData* shaderData);

void drawOverlayScroll();
GLuint compileOverlayScroll();
void createOverlayScroll(UsableShaderData* shaderData);

void drawOverlay(GLFWwindow* overlayWindow, OverlaySettings* settings, UsableShaderData shaders[], GLuint shadersCount);

#endif