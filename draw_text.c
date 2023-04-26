#include <draw_text.h>

void printBitmap(FT_Bitmap bitmap){
    for(int y = 0; y<bitmap.rows; y++){
        for(int x = 0; x<bitmap.width; x++){
            printf("%02x", bitmap.buffer[x+y*bitmap.width]);
        }
        printf("\n");
    }
}

GLuint compileTextShader(){
    //Load shaders
    GLchar * vertexShaderSource = readShaderFile("shaders/glyph.vs");
    if(vertexShaderSource == NULL) fprintf(stderr, "Can't read shaders/glyph.vs\n");
    GLuint vertexShader = createShader(vertexShaderSource, GL_VERTEX_SHADER);
    if(vertexShader == 0) fprintf(stderr, "Can't create shaders/glyph.vs\n");    

    GLchar * fragmentShaderSource = readShaderFile("shaders/glyph.fs");
    if(fragmentShaderSource == NULL) fprintf(stderr, "Can't read shaders/glyph.fs\n");
    GLuint fragmentShader = createShader(fragmentShaderSource, GL_FRAGMENT_SHADER);
    if(fragmentShader == 0) fprintf(stderr, "Can't create shaders/glyph.fs\n");

    //Create shader program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    glBindFragDataLocation(shaderProgram, 0, "outColor");

    glLinkProgram(shaderProgram);

    //* check compilation errors
    {
        GLint status;
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &status);
        if(!status){
            char buffer[512];
            glGetProgramInfoLog(shaderProgram, 512, NULL, buffer);
            fprintf(stderr, "Linking Shader program error : %s\n", buffer);
        }
    }

    glDeleteShader(vertexShader);
    free(vertexShaderSource);

    glDeleteShader(fragmentShader);
    free(fragmentShaderSource);

    return shaderProgram;
}


Atlas createTextAtlas(unsigned long start, unsigned long end, const char* font_path, int width_px, int height_px){ //end excluded
    /* Initialize the FreeType library */
    FT_Library  library;
    FT_Face face;
    
    int error = FT_Init_FreeType( &library );
    if ( error )
    {
        printf("Freetype lib init error\n");
        //return error;
    }

    error = FT_New_Face(library, font_path, 0, &face);
    if ( error == FT_Err_Unknown_File_Format )
    {
        printf("Freetype lib font file format init error\n");
    }
    else if ( error )
    {
        printf("Freetype lib font init error\n");
        //return error;
    }
    
    if(FT_Select_Charmap(face , ft_encoding_unicode)){
        printf("Freetype encoding error\n");
    }

    error = FT_Set_Pixel_Sizes(
        face,   // handle to face object
        width_px,      // pixel_width
        height_px );   // pixel_height
    
    if(error){
        printf("FreeType set pixel size error\n");
        //return error;
    }


    /* Calculate atlas dimensions */
    int w = 0;
    int h = 0;

    for(unsigned long i = start; i<end; i++){
        if(FT_Load_Char(face, i, FT_LOAD_RENDER)) {
            fprintf(stderr, "Loading character %c failed!\n", i);
            continue;
        }
        /*printf("bitmap : \n");
        printBitmap(face->glyph->bitmap);*/

        w += face->glyph->bitmap.width;
        h = MAX(h, face->glyph->bitmap.rows);
    }

    /* Create the atlas texture */
    GLuint tex;
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    // set texture options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, 0);

    AtlasCharacter* characters = (AtlasCharacter*) malloc(sizeof(AtlasCharacter)*(end-start));

    int x = 0;
    for(unsigned long i = 0; i<end-start; i++){
        if(FT_Load_Char(face, i+start, FT_LOAD_RENDER)){
            printf("error loading %c (%x)\n", i+start, i+start);
        }else{
            glTexSubImage2D(GL_TEXTURE_2D, 0, x, 0, face->glyph->bitmap.width, face->glyph->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

            characters[i].ax = face->glyph->advance.x >> 6;
            characters[i].ay = face->glyph->advance.y >> 6;

            characters[i].bw = face->glyph->bitmap.width;
            characters[i].bh = face->glyph->bitmap.rows;

            characters[i].bl = face->glyph->bitmap_left;
            characters[i].bt = face->glyph->bitmap_top;

            characters[i].tx = (float)x / w;

            x += face->glyph->bitmap.width;
        }
    }

    FT_Done_Face(face);
    FT_Done_FreeType(library);

    return (Atlas) {characters, w, h, tex, end-start, start};
}

DrawableText createDrawableTextUsingAtlas(wchar_t * text, Atlas* atlas, float x, float y, float sx, float sy){

    AtlasCharacter* characters = atlas->characters;
    float atlas_width = (float) atlas->width;
    float atlas_height = (float) atlas->height;

    unsigned int text_len = wcslen(text);

    unsigned int vertices_count = 4*4*text_len;
    unsigned int elements_count = 2*3*text_len;

    //printf("Text len %i gonna create %i vertices and %i elements (%i + %i)\n", text_len, vertices_count, elements_count, sizeof(GLfloat)*vertices_count, sizeof(GLuint)*elements_count);

    GLfloat* vertices = (GLfloat*) malloc(sizeof(GLfloat)*vertices_count);
    //printf("Vertices OK\n");
    GLuint* elements = (GLuint*) malloc(sizeof(GLuint)*elements_count);
    //printf("Elements OK\n");

    for(unsigned int i = 0; i<text_len;i++){
        if(text[i] < atlas->start || text[i] > atlas->count+atlas->start) continue;
        //printf("calculate %c vertices\n", text[i]);
        unsigned int ie = i*2*3;
        unsigned int iv = i*4*4;

        elements[ie] = 0+i*4;
        elements[ie+1] = 1+i*4;
        elements[ie+2] = 2+i*4;
        elements[ie+3] = 2+i*4;
        elements[ie+4] = 3+i*4;
        elements[ie+5] = 0+i*4;

        AtlasCharacter ac = characters[text[i]-atlas->start];
        //printf("bl : %f, bt : %f, bw : %f, bh : %f\n", ac.bl, ac.bt, ac.bw, ac.bh);
        float xpos = x + ac.bl * sx;
        float ypos = -y - (ac.bh - ac.bt) * sy;

        float w = ac.bw * sx;
        float h = ac.bh * sy;

        /* Advance the cursor to the start of the next character */
        x += ac.ax * sx;
        y += ac.ay * sy; //? character in line so ? => perhaps in link with the minus sign of ypos above ¯\_(ツ)_/¯

        float l = xpos;
        float t = (ypos + h);
        float r = (xpos + w);
        float b = ypos;

        //TL
        vertices[iv] = l;
        vertices[iv+1] = t;
        vertices[iv+2] = ac.tx;
        vertices[iv+3] = 0;

        //BL
        vertices[iv+4] = l;
        vertices[iv+5] = b; //! not normalized
        vertices[iv+6] = ac.tx;
        vertices[iv+7] = ac.bh/atlas_height;
        
        //BR
        vertices[iv+8] = r; //! not normalized
        vertices[iv+9] = b; //!
        vertices[iv+10] = ac.tx + ac.bw/atlas_width;;
        vertices[iv+11] = ac.bh/atlas_height;

        //TR
        vertices[iv+12] = r;
        vertices[iv+13] = t;
        vertices[iv+14] = ac.tx + ac.bw/atlas_width;;
        vertices[iv+15] = 0;
    }

    //Create shader program
    GLint shaderProgram = compileTextShader();

    //Upload shader data
    GLuint vao, vbo, ebo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glBindVertexArray(vao);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*elements_count, elements, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vertices_count, vertices, GL_STATIC_DRAW); //? dynamic draw ?
    
    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 4*sizeof(GLfloat), (GLvoid*)0);

    GLint texAttrib = glGetAttribLocation(shaderProgram, "texcoord");
    glEnableVertexAttribArray(texAttrib);
    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 4*sizeof(GLfloat), (GLvoid*)(2*sizeof(GLfloat)));

    glBindVertexArray(0); //?

    return (DrawableText) {elements, elements_count, vertices, vertices_count, atlas->tex, shaderProgram, vao};
}

void drawText(DrawableText * draw, ColorRGBAf color){
    glUseProgram(draw->shaderProgram);
    glBindVertexArray(draw->vao);
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(draw->shaderProgram, "text"), GL_TEXTURE0);
    glUniform4f(glGetUniformLocation(draw->shaderProgram, "textColor"), color.r, color.g, color.b, color.a);
    glBindTexture(GL_TEXTURE_2D, draw->atlasTex);
    glDrawElements(GL_TRIANGLES, draw->elements_count, GL_UNSIGNED_INT, 0);
}

/* using now wchar_t instead of long, better
unsigned long * charToULong(const char * text){
    unsigned int text_len = 0;
    while(text[text_len]) text_len++;

    unsigned long * out = (unsigned long *) malloc(sizeof(unsigned long)*(text_len+1));
    for(unsigned int i = 0; i<text_len; i++){
        out[i] = (unsigned long) text[i];
    }
    out[text_len] = 0;
    return out;
}

unsigned long * wcharToULong(const short * text){
    unsigned int text_len = 0;
    while(text[text_len]) text_len++;

    unsigned long * out = (unsigned long *) malloc(sizeof(unsigned long)*(text_len+1));
    for(unsigned int i = 0; i<text_len; i++){
        out[i] = (unsigned long) text[i];
    }
    out[text_len] = 0;
    return out;
}
*/