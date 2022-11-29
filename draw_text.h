#ifndef HEADER_DRAW_TEXT
#define HEADER_DRAW_TEXT

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <common.h>
#include <load_shader.h>
#include <ft2build.h>
#include FT_FREETYPE_H

static GLuint glyphShaderProgram = 0;
/*void printBitmap(FT_Bitmap bitmap);
int loadAndCreateShaderProgram();*/
//int drawText(unsigned long* text, const char* font_path, int width_px, int height_px, int x, int y, ColorRGBAf color);
void drawTexturesText(unsigned long* text, const char* font_path, int width_px, int height_px, int x, int y, ColorRGBAf color);
CharacterList genTextCharacters(unsigned long* text, const char* font_path, int width_px, int height_px);
void drawText(void *);
GLuint compileTextShader();
void createText(UsableShaderDataInput* shaderData);

#endif