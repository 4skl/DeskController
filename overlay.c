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
    //glfwWindowHint(GLFW_DECORATED, GL_FALSE);
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GL_TRUE);
    glfwWindowHint(GLFW_FLOATING, GL_TRUE);
    //TODO uncomment when glfw 3.4 released glfwWindowHint(GLFW_MOUSE_PASSTHROUGH, GLFW_TRUE);

    /* Create a windowed mode window and its OpenGL context */
    overlayWindow = glfwCreateWindow((deskInfo->width*settings->sizeFactor)/100, 3, "DeskController", NULL, NULL); //glfwGetPrimaryMonitor() as 3rd argument for fullscreen

    glfwSetWindowPos(overlayWindow, deskInfo->width/2 - (deskInfo->width*settings->sizeFactor)/100/2, 0);

    return overlayWindow;
}





/** Background **/
void drawOverlayBackground(){
    glDrawElements(GL_TRIANGLES, 4*3, GL_UNSIGNED_INT, 0);
}

GLuint compileOverlayBackground(){
    //Load shaders
    GLchar * bgVertexShaderSource = readShaderFile("shaders/overlay_background.vs");
    if(bgVertexShaderSource == NULL) fprintf(stderr, "Can't read shaders/overlay_background.vs\n");
    GLuint bgVertexShader = createShader(bgVertexShaderSource, GL_VERTEX_SHADER);
    

    GLchar * bgFragmentShaderSource = readShaderFile("shaders/overlay_background.fs");
    if(bgFragmentShaderSource == NULL) fprintf(stderr, "Can't read shaders/overlay_background.fs\n");
    GLuint bgFragmentShader = createShader(bgFragmentShaderSource, GL_FRAGMENT_SHADER);


    //Create shader program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, bgVertexShader);
    glAttachShader(shaderProgram, bgFragmentShader);

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
    glDeleteShader(bgVertexShader);
    free(bgVertexShaderSource);

    glDeleteShader(bgFragmentShader);
    free(bgFragmentShaderSource);

    return shaderProgram;
}

void createOverlayBackground(UsableShaderData* shaderData){
    shaderData->shaderProgram = compileOverlayBackground();


    GLfloat a = 2/(SQRT_2+1);
    GLfloat overlay_backgroundShapeVertices[] = {
        0, 0,
        1, -1,
        1, 1,
        -1, 1,
        -1, -1
    };

    GLuint overlay_backgroundShapeElements[] = {
        0, 1, 2,
        0, 2, 3,
        0, 3, 4,
        0, 4, 1,
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
    glDrawElements(GL_TRIANGLES, 4*3, GL_UNSIGNED_INT, 0);
}

GLuint compileOverlayWheel(){
    //Load shaders
    GLchar * bgVertexShaderSource = readShaderFile("shaders/overlay_wheel.vs");
    if(bgVertexShaderSource == NULL) fprintf(stderr, "Can't read shaders/overlay_wheel.vs\n");
    GLuint bgVertexShader = createShader(bgVertexShaderSource, GL_VERTEX_SHADER);
    

    GLchar * bgFragmentShaderSource = readShaderFile("shaders/overlay_wheel.fs");
    if(bgFragmentShaderSource == NULL) fprintf(stderr, "Can't read shaders/overlay_wheel.fs\n");
    GLuint bgFragmentShader = createShader(bgFragmentShaderSource, GL_FRAGMENT_SHADER);


    //Create shader program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, bgVertexShader);
    glAttachShader(shaderProgram, bgFragmentShader);

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
    glDeleteShader(bgVertexShader);
    free(bgVertexShaderSource);

    glDeleteShader(bgFragmentShader);
    free(bgFragmentShaderSource);

    return shaderProgram;
}

void createOverlayWheel(UsableShaderData* shaderData){
    shaderData->shaderProgram = compileOverlayWheel();
    
    GLfloat overlay_wheelShapeVertices[] = {
        0, 0,
        1, -1,
        1, 1,
        -1, 1,
        -1, -1
    };

    GLuint overlay_wheelShapeElements[] = {
        0, 1, 2,
        0, 2, 3,
        0, 3, 4,
        0, 4, 1,
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



//todo
void drawOverlay(GLFWwindow* overlayWindow, OverlaySettings* settings, UsableShaderData shaders[], GLuint shadersCount){
    /* Make the window's context current */
    //glfwMakeContextCurrent(overlayWindow);
    
    /* draw shaders views */
    for(GLuint i = 0; i<shadersCount; i++){
        glUseProgram(shaders[i].shaderProgram);
        glBindVertexArray(shaders[i].vao);
        shaders[i].drawFunction();
    }

    //...
}
