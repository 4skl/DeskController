#include <load_shader.h>

GLchar * readShaderFile(const char * file_path){ //efficient ?
    FILE *f = fopen(file_path, "r");
    if(f == NULL) return NULL;
    fseek(f, 0, SEEK_END);
    size_t sz = ftell(f);
    rewind(f);
    GLchar * shaderSource = (GLchar *) malloc(sz*sizeof(GLchar));
    size_t rsz = fread(shaderSource, sizeof(GLchar), sz, f);
    /*if(sz == rsz)*/ return shaderSource;
    fprintf(stderr, "Can read only %i/%i; error %i for file %s\n", rsz, sz, ferror(f), file_path);
    return NULL;
}

GLuint createShader(const GLchar * shaderSource, GLenum shaderType){
    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &shaderSource, NULL);
    glCompileShader(shader);

    //* check compilation errors
    {
        GLint status;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
        if(!status){
            char buffer[512];
            glGetShaderInfoLog(shader, 512, NULL, buffer);
            fprintf(stderr, "Shader compilation error : %s\n", buffer);
        }
    }
    return shader;
}