#ifndef HEADER_LOAD_SHADER
#define HEADER_LOAD_SHADER

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>

GLchar * readShaderFile(const char * file_path);
GLuint createShader(const GLchar * shaderSource, GLenum shaderType);

#endif