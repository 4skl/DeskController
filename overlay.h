#ifndef HEADER_OVERLAY
#define HEADER_OVERLAY

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <load_shader.h>
#include <common.h>
#include <draw_text.h>

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

void drawOverlayKnob();
GLuint compileOverlayScroll();
void createOverlayKnob(UsableShaderData* shaderData);

//void drawOverlay(GLFWwindow* overlayWindow, OverlaySettings* settings, UsableShaderData shaders[], GLuint shadersCount);
DrawableText createDrawableTextWheelUsingAtlas(wchar_t * text, Atlas* atlas, float x, float y, float sx, float sy, float sizew, float sizeh);

#endif