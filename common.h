#ifndef HEADER_COMMON
#define HEADER_COMMON

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h> 

struct ColorRGBf {
    float r;
    float g;
    float b;
};
typedef struct ColorRGBf ColorRGBf;

enum Side {TOP, BOTTOM, LEFT, RIGHT};
typedef enum Side Side;

struct UsableShaderData {
    GLuint shaderProgram;
    GLuint vao;
    void (*drawFunction)();
};
typedef struct UsableShaderData UsableShaderData;

#endif