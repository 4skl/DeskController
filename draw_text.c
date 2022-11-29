#include <draw_text.h>
/*
void printBitmap(FT_Bitmap bitmap){
    for(int y = 0; y<bitmap.rows; y++){
        for(int x = 0; x<bitmap.width; x++){
            printf("%02x", bitmap.buffer[x+y*bitmap.width]);
        }
        printf("\n");
    }
}

int loadAndCreateShaderProgram(){
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
    glyphShaderProgram = glCreateProgram();
    glAttachShader(glyphShaderProgram, vertexShader);
    glAttachShader(glyphShaderProgram, fragmentShader);

    glBindFragDataLocation(glyphShaderProgram, 0, "outColor");

    glLinkProgram(glyphShaderProgram);

    //* check compilation errors
    {
        GLint status;
        glGetProgramiv(glyphShaderProgram, GL_LINK_STATUS, &status);
        if(!status){
            char buffer[512];
            glGetProgramInfoLog(glyphShaderProgram, 512, NULL, buffer);
            fprintf(stderr, "Linking Shader program error : %s\n", buffer);
        }
    }

    //not needed, but delete shaders as them have been linked into the program :
    glDeleteShader(vertexShader);
    free(vertexShaderSource);

    glDeleteShader(fragmentShader);
    free(fragmentShaderSource);

    return glyphShaderProgram;
}*/

//int drawText(unsigned long* text, const char* font_path, int width_px, int height_px, int x, int y, ColorRGBAf color){
    /* FreeType test */
    /* Initialize the FreeType library */
/*    FT_Library  library;
    FT_Face face;
    
    int error = FT_Init_FreeType( &library );
    if ( error )
    {
        printf("Freetype lib init error\n");
        return error;
    }

    error = FT_New_Face(library, font_path, 0, &face);
    if ( error == FT_Err_Unknown_File_Format )
    {
        printf("Freetype lib font file format init error\n");
    }
    else if ( error )
    {
        printf("Freetype lib font init error\n");
        return error;
    }

    //printf("Num gly : %i\n", face->num_glyphs);
    /*error = FT_Set_Char_Size(
        face,    // handle to face object
        0,       // char_width in 1/64 of points
        16*64,   // char_height in 1/64 of points
        dpix,     // horizontal device resolution
        dpiy );   // vertical device resolution
    
    if(error){
        printf("FreeType set char size error\n");
    }//

    error = FT_Set_Pixel_Sizes(
        face,   // handle to face object
        width_px,      // pixel_width
        height_px );   // pixel_height
    
    if(error){
        printf("FreeType set pixel size error\n");
        return error;
    }

    /* Trying draw bitmaps */
    /*int text_len = 0;
    while(text[text_len]) text_len++;

    Character* characters = malloc(sizeof(Character)*text_len); //? -1; alloc only pointer ?

    // set texture options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    for(int i = 0; i<text_len; i++){
        unsigned int glyph_index = FT_Get_Char_Index( face,  text[i]);

        error = FT_Load_Glyph(
            face,          // handle to face object
            glyph_index,   // glyph index
            FT_LOAD_DEFAULT );  // https://freetype.org/freetype2/docs/reference/ft2-base_interface.html#ft_load_xxx
            
        if(error){
            printf("FreeType load glyph error\n");
            return error;
        }

        //We suppoose not FT_GLYPH_FORMAT_BITMAP for face−>glyph−>format
        error = FT_Render_Glyph( face->glyph,   // glyph slot
                            FT_RENDER_MODE_NORMAL ); // render mode https://freetype.org/freetype2/docs/reference/ft2-base_interface.html#ft_render_mode

        if(error){
            printf("FreeType render glyph error\n");
            return error;
        }

        printBitmap(face->glyph->bitmap);
        
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );
        Character character = {
            texture, 
            {.width=face->glyph->bitmap.width, .height=face->glyph->bitmap.rows},
            {.width=face->glyph->bitmap_left, .height=face->glyph->bitmap_top},
            face->glyph->advance.x
        };
        characters[i] = character;
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); //remove 4 bit alignment

    FT_Done_Face(face);
    FT_Done_FreeType(library);


    // opengl draw

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW); //6 vertices ? not 4
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    if(glyphShaderProgram == 0){
        loadAndCreateShaderProgram();
    }

    // Start rendering
    glUseProgram(glyphShaderProgram);

    glUniform4f(glGetUniformLocation(glyphShaderProgram, "textColor"), color.r, color.g, color.b, color.a);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);
    float scale = 1;// ?
    for(int i = 0; i<text_len; i++){
        Character ch = characters[i];

        float xpos = x + ch.bearing.width * scale;
        float ypos = y - (ch.size.height - ch.bearing.height) * scale;

        float w = ch.size.width * scale;
        float h = ch.size.height * scale;
        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },            
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }           
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.textureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); 
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    free(characters);
    return 0;
}*/

void drawText(void * in){
    CharacterList* cl = (CharacterList*) in;
    glDrawElements(GL_TRIANGLES, 2*3, GL_UNSIGNED_INT, 0);
}

CharacterList genTextCharacters(unsigned long* text, const char* font_path, int width_px, int height_px){
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

    error = FT_Set_Pixel_Sizes(
        face,   // handle to face object
        width_px,      // pixel_width
        height_px );   // pixel_height
    
    if(error){
        printf("FreeType set pixel size error\n");
        //return error;
    }

    /* Prepare to draw bitmaps (store texture on gpu&store ref on cpu) */
    unsigned int text_len = 0;
    while(text[text_len]) text_len++;

    Character* characters = (Character*) malloc(sizeof(Character)*text_len); //? -1

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); //remove 4 bit alignment

    for(unsigned int i = 0; i<text_len; i++){
        unsigned int glyph_index = FT_Get_Char_Index( face,  text[i]);

        error = FT_Load_Glyph(
            face,          // handle to face object
            glyph_index,   // glyph index
            FT_LOAD_DEFAULT );  // https://freetype.org/freetype2/docs/reference/ft2-base_interface.html#ft_load_xxx
            
        if(error){
            printf("FreeType load glyph error\n");
            //return error;
        }

        //We suppoose not FT_GLYPH_FORMAT_BITMAP for face−>glyph−>format
        error = FT_Render_Glyph( face->glyph,   // glyph slot
                            FT_RENDER_MODE_NORMAL ); // render mode https://freetype.org/freetype2/docs/reference/ft2-base_interface.html#ft_render_mode

        if(error){
            printf("FreeType render glyph error\n");
            //return error;
        }

        //printBitmap(face->glyph->bitmap);
        
        // gen texture
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        
        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // set texture data
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );

        // save caracter with texture
        characters[i].textureID = texture;
        Dim2D size = {.width=face->glyph->bitmap.width, .height=face->glyph->bitmap.rows};
        characters[i].size = size;
        Dim2D bearing = {.width=face->glyph->bitmap_left, .height=face->glyph->bitmap_top};
        characters[i].bearing = bearing;
        characters[i].advance = face->glyph->advance.x;
    }

    FT_Done_Face(face);
    FT_Done_FreeType(library);
    CharacterList cl = {characters, text_len};
    return cl;
}

void drawTexturesText(unsigned long* text, const char* font_path, int width_px, int height_px, int x, int y, ColorRGBAf color){
    CharacterList cl = genTextCharacters(text, font_path, width_px, height_px);
    unsigned int text_len = cl.count;
    Character* characters = cl.characters;

    // opengl draw

    //load one time the shader to avoid some crash by reading too much shader files,.. //todo improve
    if(!glyphShaderProgram){
        glyphShaderProgram = compileTextShader();
    }
    GLuint shaderProgram = glyphShaderProgram;


    // creating vao and vbo
    GLuint vao, vbo, ebo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glBindVertexArray(vao);

    GLuint elements[] = {
        0, 1, 2,
        2, 3, 0
    };
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW); //? sizeof(elements) or sizeof(GLuint) * 6
    
    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 4*sizeof(GLfloat), (GLvoid*)0);

    GLint texAttrib = glGetAttribLocation(shaderProgram, "texcoord");
    glEnableVertexAttribArray(texAttrib);
    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 4*sizeof(GLfloat), (GLvoid*)(2*sizeof(GLfloat))); 

    // Start rendering
    glUseProgram(shaderProgram);

    glUniform4f(glGetUniformLocation(shaderProgram, "textColor"), color.r, color.g, color.b, color.a);
    //glUniform4f(glGetUniformLocation(shaderProgram, "projection"), 230, 0, 384, 0);
    float s_w = 230;
    float s_h = 384;

    glActiveTexture(GL_TEXTURE0);
    for(int i = 0; i<text_len; i++){
        Character ch = characters[i];

        float xpos = x + ch.bearing.width;
        float ypos = y - (ch.size.height - ch.bearing.height);

        float w = ch.size.width;
        float h = ch.size.height;
        // update VBO for each character
        GLfloat vertices[] = {
            //position                     // tex coords
            xpos/s_w,     (ypos + h)/s_h,   0.0f, 0.0f,            
            xpos/s_w,     ypos/s_h,       0.0f, 1.0f,
            (xpos + w)/s_w, ypos/s_h,       1.0f, 1.0f,
            (xpos + w)/s_w, (ypos + h)/s_h,   1.0f, 0.0f        
        };

        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.textureID);
        glUniform1i(glGetUniformLocation(shaderProgram, "text"), 0); //0 or textureID or GL_TEXTURE0
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

        // render quad
        glDrawElements(GL_TRIANGLES, 2*3, GL_UNSIGNED_INT, 0);

        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.advance >> 6); //? bitshift by 6 to get value in pixels (2^6 = 64)
        printf("x : %i, xpos : %f, add : %i\n", x, xpos, ch.advance >> 6);
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

void createText(UsableShaderDataInput* shaderData){
    shaderData->shaderProgram = compileTextShader();

    GLfloat overlay_backgroundShapeVertices[] = {
        -1, -1,
        -1, 1,
        1, 1,
        1, -1
    };

    GLuint overlay_backgroundShapeElements[] = {
        0, 1, 2,
        0, 2, 3
    };

    //Load vertices
    //GLuint* vao = &(shaderData->vao);
    GLuint vbo, vao;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, sizeof(overlay_backgroundShapeVertices), overlay_backgroundShapeVertices, GL_STATIC_DRAW);

    //Load elements
    GLuint ebo;
    glGenBuffers(1, &ebo);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(overlay_backgroundShapeElements), overlay_backgroundShapeElements, GL_STATIC_DRAW);

    //Link vertex datas to the program
    GLint posAttrib = glGetAttribLocation(shaderData->shaderProgram, "position");
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat), (GLvoid*)0); //3rd arg : normalize input if not floating point depending on the format; 4th arg : stride; 5th arg : offset 
    glEnableVertexAttribArray(vao);
    
    glUseProgram(shaderData->shaderProgram);
    shaderData->vao = vao;
    shaderData->drawFunction=drawText;
}