#ifndef HEADER_COMMON
#define HEADER_COMMON

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h> 

#define PI 3.141592653589793
#define SQRT_2 1.4142135623730951
#define radians(v) (v / 180.0 * PI)


struct Dim2D {
    float width;
    float height;
};
typedef struct Dim2D Dim2D;

struct ColorRGBf {
    float r;
    float g;
    float b;
};
typedef struct ColorRGBf ColorRGBf;

struct ColorRGBAf {
    float r;
    float g;
    float b;
    float a;
};
typedef struct ColorRGBAf ColorRGBAf;

enum Side {TOP, BOTTOM, LEFT, RIGHT};
typedef enum Side Side;

struct UsableShaderData {
    GLuint shaderProgram;
    GLuint vao;
    void (*drawFunction)();
};
typedef struct UsableShaderData UsableShaderData;

struct UsableShaderDataInput {
    GLuint shaderProgram;
    GLuint vao;
    void (*drawFunction)(void*);
};
typedef struct UsableShaderDataInput UsableShaderDataInput;

struct Character {
    unsigned int textureID;  // ID handle of the glyph texture
    Dim2D   size;       // Size of glyph
    Dim2D   bearing;    // Offset from baseline to left/top of glyph
    unsigned int advance;    // Offset to advance to next glyph
};
typedef struct Character Character;

struct CharacterList {
    Character * characters;
    unsigned int count;
};
typedef struct CharacterList CharacterList;

#endif