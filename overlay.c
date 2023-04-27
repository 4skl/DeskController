#include <overlay.h>

GLFWwindow* createOverlayWindow(GLFWmonitor* monitor, OverlaySettings* settings){
    //if(settings == NULL) settings = &defaultSettings;

    GLFWwindow* overlayWindow;
    
    /* get desktop informations */
    const GLFWvidmode* deskInfo = glfwGetVideoMode(monitor);

    /* Set options */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    glfwWindowHint(GLFW_DECORATED, GL_FALSE);
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GL_TRUE);
    glfwWindowHint(GLFW_FLOATING, GL_TRUE);
    // todo add when GLFW 3.4 release glfwWindowHint(GLFW_MOUSE_PASSTHROUGH, GLFW_TRUE);

    /* Create a windowed mode window and its OpenGL context */
    overlayWindow = glfwCreateWindow((deskInfo->width*settings->sizeFactor)/100, (deskInfo->width*settings->sizeFactor)/100, "DeskController", NULL, NULL); //glfwGetPrimaryMonitor() as 3rd argument for fullscreen

    glfwSetWindowPos(overlayWindow, deskInfo->width/2 - (deskInfo->width*settings->sizeFactor)/100/2, 0);

    return overlayWindow;
}





/** Background **/
void drawOverlayBackground(){
    glDrawElements(GL_TRIANGLES, 2*3, GL_UNSIGNED_INT, 0);
}

GLuint compileOverlayBackground(){
    //Load shaders
    GLchar * vertexShaderSource = readShaderFile("shaders/overlay_background.vs");
    if(vertexShaderSource == NULL) fprintf(stderr, "Can't read shaders/overlay_background.vs\n");
    GLuint vertexShader = createShader(vertexShaderSource, GL_VERTEX_SHADER);
    if(vertexShader == 0) fprintf(stderr, "Can't create shaders/overlay_background.vs\n");    

    GLchar * fragmentShaderSource = readShaderFile("shaders/overlay_background.fs");
    if(fragmentShaderSource == NULL) fprintf(stderr, "Can't read shaders/overlay_background.fs\n");
    GLuint fragmentShader = createShader(fragmentShaderSource, GL_FRAGMENT_SHADER);
    if(fragmentShader == 0) fprintf(stderr, "Can't create shaders/overlay_background.fs\n");


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

    //not needed, but delete shaders as them have been linked into the program :
    glDeleteShader(vertexShader);
    free(vertexShaderSource);

    glDeleteShader(fragmentShader);
    free(fragmentShaderSource);

    return shaderProgram;
}

void createOverlayBackground(UsableShaderData* shaderData){
    shaderData->shaderProgram = compileOverlayBackground();


    GLfloat a = 2/(SQRT_2+1);
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
    glEnableVertexAttribArray(posAttrib);
    
    glUseProgram(shaderData->shaderProgram);
    shaderData->vao = vao;
    shaderData->drawFunction=drawOverlayBackground;
}

/** Wheels **/
void drawOverlayWheel(){
    glDrawElements(GL_TRIANGLES, 2*3, GL_UNSIGNED_INT, 0);
}

GLuint compileOverlayWheel(){
    //Load shaders
    GLchar * vertexShaderSource = readShaderFile("shaders/overlay_wheel.vs");
    if(vertexShaderSource == NULL) fprintf(stderr, "Can't read shaders/overlay_wheel.vs\n");
    GLuint vertexShader = createShader(vertexShaderSource, GL_VERTEX_SHADER);
    if(vertexShader == 0) fprintf(stderr, "Can't create shaders/overlay_wheel.vs\n");
    

    GLchar * fragmentShaderSource = readShaderFile("shaders/overlay_wheel.fs");
    if(fragmentShaderSource == NULL) fprintf(stderr, "Can't read shaders/overlay_wheel.fs\n");
    GLuint fragmentShader = createShader(fragmentShaderSource, GL_FRAGMENT_SHADER);
    if(fragmentShader == 0) fprintf(stderr, "Can't create shaders/overlay_wheel.fs\n");


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

    //not needed, but delete shaders as them have been linked into the program :
    glDeleteShader(vertexShader);
    free(vertexShaderSource);

    glDeleteShader(fragmentShader);
    free(fragmentShaderSource);

    return shaderProgram;
}

void createOverlayWheel(UsableShaderData* shaderData){
    shaderData->shaderProgram = compileOverlayWheel();
    
    GLfloat overlay_wheelShapeVertices[] = {
        -1, -1,
        -1, 1,
        1, 1,
        1, -1
    };

    GLuint overlay_wheelShapeElements[] = {
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

    glBufferData(GL_ARRAY_BUFFER, sizeof(overlay_wheelShapeVertices), overlay_wheelShapeVertices, GL_STATIC_DRAW);

    //Load elements
    GLuint ebo;
    glGenBuffers(1, &ebo);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(overlay_wheelShapeElements), overlay_wheelShapeElements, GL_STATIC_DRAW);

    //Link vertex datas to the program
    GLint posAttrib = glGetAttribLocation(shaderData->shaderProgram, "position");
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat), (GLvoid*)0); //3rd arg : normalize input if not floating point depending on the format; 4th arg : stride; 5th arg : offset 
    glEnableVertexAttribArray(posAttrib);
    
    glUseProgram(shaderData->shaderProgram);
    shaderData->vao = vao;
    shaderData->drawFunction=drawOverlayWheel;
}


/** Scrolls **/
void drawOverlayScroll(){
    glDrawElements(GL_TRIANGLES, 2*3, GL_UNSIGNED_INT, 0);
}

GLuint compileOverlayScroll(){
    //Load shaders
    GLchar * vertexShaderSource = readShaderFile("shaders/overlay_scroll.vs");
    if(vertexShaderSource == NULL) fprintf(stderr, "Can't read shaders/overlay_scroll.vs\n");
    GLuint vertexShader = createShader(vertexShaderSource, GL_VERTEX_SHADER);
    if(vertexShader == 0) fprintf(stderr, "Can't create shaders/overlay_scroll.vs\n");
    

    GLchar * fragmentShaderSource = readShaderFile("shaders/overlay_scroll.fs");
    if(fragmentShaderSource == NULL) fprintf(stderr, "Can't read shaders/overlay_scroll.fs\n");
    GLuint fragmentShader = createShader(fragmentShaderSource, GL_FRAGMENT_SHADER);
    if(fragmentShader == 0) fprintf(stderr, "Can't create shaders/overlay_scroll.fs\n");


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

    //not needed, but delete shaders as them have been linked into the program :
    glDeleteShader(vertexShader);
    free(vertexShaderSource);

    glDeleteShader(fragmentShader);
    free(fragmentShaderSource);

    return shaderProgram;
}

void createOverlayScroll(UsableShaderData* shaderData){
    shaderData->shaderProgram = compileOverlayScroll();
    
    GLfloat overlay_scrollShapeVertices[] = {
        -1, -1,
        -1, 1,
        1, 1,
        1, -1
    };

    GLuint overlay_scrollShapeElements[] = {
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

    glBufferData(GL_ARRAY_BUFFER, sizeof(overlay_scrollShapeVertices), overlay_scrollShapeVertices, GL_STATIC_DRAW);

    //Load elements
    GLuint ebo;
    glGenBuffers(1, &ebo);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(overlay_scrollShapeElements), overlay_scrollShapeElements, GL_STATIC_DRAW);

    //Link vertex datas to the program
    GLint posAttrib = glGetAttribLocation(shaderData->shaderProgram, "position");
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat), (GLvoid*)0); //3rd arg : normalize input if not floating point depending on the format; 4th arg : stride; 5th arg : offset 
    glEnableVertexAttribArray(posAttrib);
    
    glUseProgram(shaderData->shaderProgram);
    shaderData->vao = vao;
    shaderData->drawFunction=drawOverlayScroll;
}


/** Knob **/
void drawOverlayKnob(){
    glDrawElements(GL_TRIANGLES, 2*3, GL_UNSIGNED_INT, 0);
}

GLuint compileOverlayKnob(){
    //Load shaders
    GLchar * vertexShaderSource = readShaderFile("shaders/knob.vs");
    if(vertexShaderSource == NULL) fprintf(stderr, "Can't read shaders/knob.vs\n");
    GLuint vertexShader = createShader(vertexShaderSource, GL_VERTEX_SHADER);
    if(vertexShader == 0) fprintf(stderr, "Can't create shaders/knob.vs\n");
    

    GLchar * fragmentShaderSource = readShaderFile("shaders/knob.fs");
    if(fragmentShaderSource == NULL) fprintf(stderr, "Can't read shaders/knob.fs\n");
    GLuint fragmentShader = createShader(fragmentShaderSource, GL_FRAGMENT_SHADER);
    if(fragmentShader == 0) fprintf(stderr, "Can't create shaders/knob.fs\n");


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

    //not needed, but delete shaders as them have been linked into the program :
    glDeleteShader(vertexShader);
    free(vertexShaderSource);

    glDeleteShader(fragmentShader);
    free(fragmentShaderSource);

    return shaderProgram;
}

void createOverlayKnob(UsableShaderData* shaderData){
    shaderData->shaderProgram = compileOverlayKnob();
    
    GLfloat overlay_knobShapeVertices[] = {
        -1, -1,
        -1, 1,
        1, 1,
        1, -1
    };

    GLuint overlay_knobShapeElements[] = {
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

    glBufferData(GL_ARRAY_BUFFER, sizeof(overlay_knobShapeVertices), overlay_knobShapeVertices, GL_STATIC_DRAW);

    //Load elements
    GLuint ebo;
    glGenBuffers(1, &ebo);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(overlay_knobShapeElements), overlay_knobShapeElements, GL_STATIC_DRAW);

    //Link vertex datas to the program
    GLint posAttrib = glGetAttribLocation(shaderData->shaderProgram, "position");
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat), (GLvoid*)0); //3rd arg : normalize input if not floating point depending on the format; 4th arg : stride; 5th arg : offset 
    glEnableVertexAttribArray(posAttrib);
    
    glUseProgram(shaderData->shaderProgram);
    shaderData->vao = vao;
    shaderData->drawFunction=drawOverlayKnob;
}


/** Round Text **/

DrawableText createDrawableTextWheelUsingAtlas(wchar_t * text, Atlas* atlas, float x, float y, float sx, float sy, float sizew, float sizeh){

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

        // characters in circle around x and y, away by size
        float xpos = x + ac.bl * sx;
        float ypos = -y - (ac.bh - ac.bt) * sy;
        float rv = -((float) i)/text_len*2*PI+PI/2;
        xpos += cos(rv)*sizew;
        ypos += sin(rv)*sizeh;

        float w = ac.bw * sx;
        float h = ac.bh * sy;

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