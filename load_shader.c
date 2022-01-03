#include <load_shader.h>

char * readShaderFile(const char * file_path){ //efficient ?
    FILE *f = fopen(file_path, "r");
    if(f == NULL) return NULL;
    fseek(f, 0, SEEK_END);
    size_t sz = ftell(f);
    rewind(f);
    char * shaderSource = (char *) malloc(sz*sizeof(char));
    if(sz == fread(shaderSource, sz, sizeof(char), f)) return shaderSource;
    return NULL;
}