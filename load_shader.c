#include <load_shader.h>

GLchar * readShaderFile(const char * file_path){ //efficient ?
    FILE *f = fopen(file_path, "rb");
    if(f == NULL) return NULL;
    fseek(f, 0L, SEEK_END);
    size_t sz = ftell(f);
    rewind(f);
    GLchar * shaderSource = (GLchar *) malloc((sz+1)*sizeof(char)); //? char or GLchar ?
    size_t buf_size = 8192;
    size_t rsz = 0;
    while(rsz<sz){
        size_t trsz = fread(shaderSource+rsz, sizeof(GLchar), sz-rsz > buf_size ? buf_size : sz-rsz, f);
        if(trsz == 0) break;
        rsz += trsz;
    }
    shaderSource[sz] = '\0'; //mandatory ?
    if(sz == rsz) return shaderSource;
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
            return 0;
        }
    }
    return shader;
}