#ifndef HEADER_DRAW_TEXT
#define HEADER_DRAW_TEXT

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <common.h>
#include <load_shader.h>
#include <ft2build.h>
#include FT_FREETYPE_H

struct AtlasCharacter {
    float ax; // advance.x
    float ay; // advance.y

    float bw; // bitmap.width;
    float bh; // bitmap.rows;

    float bl; // bitmap_left;
    float bt; // bitmap_top;

    float tx; // x offset of glyph in texture coordinates
};
typedef struct AtlasCharacter AtlasCharacter;

struct Atlas {
    AtlasCharacter* characters;
    unsigned int width;
    unsigned int height;
    GLuint tex;
    unsigned int count;
    unsigned long start;
};
typedef struct Atlas Atlas;

struct DrawableText {
    GLuint* elements;
    unsigned int elements_count;
    GLfloat* vertices;
    unsigned int vertices_count;
    GLuint atlasTex;
    GLuint shaderProgram;
    GLuint vao;
};
typedef struct DrawableText DrawableText;

void printBitmap(FT_Bitmap bitmap);
GLuint compileTextShader();
//void createText(UsableShaderDataInput* shaderData);
Atlas createTextAtlas(unsigned long start, unsigned long end, const char* font_path, int width_px, int height_px);
DrawableText createDrawableTextUsingAtlas(wchar_t * text, Atlas* atlas, float x, float y, float sx, float sy);
void drawText(DrawableText * draw, ColorRGBAf color);
/* useless
unsigned long * charToULong(const char * text);
unsigned long * wcharToULong(const short * text);
*/

#endif