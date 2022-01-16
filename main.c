#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include <overlay.h>

#define FPS 144

//force use dedicated graphic card //https://stackoverflow.com/questions/16823372/forcing-machine-to-use-dedicated-graphics-card
__declspec(dllexport) unsigned long NvOptimusEnablement = 1;
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;

float tr,tg,tb;
Dim2D displayDim;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    //if (key == GLFW_KEY_E && action == GLFW_PRESS)
        printf("key %i | scancode %i | action %i | mods %i\n", key, scancode, action, mods);
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    printf("xpos : %lf | ypos %lf\n", xpos, ypos);
}

void joystick_callback(int jid, int event)
{
    if (event == GLFW_CONNECTED)
    {
        // The joystick was connected
        printf("Joystick %i (%s) connected\n", jid, glfwGetGamepadName(jid));
    }
    else if (event == GLFW_DISCONNECTED)
    {
        // The joystick was disconnected
        printf("Joystick %i (%s) disconnected\n", jid, glfwGetGamepadName(jid));
    }
}

void error_callback(int error, const char* description){
    printf("Error %i : %s\n", error, description);
}

void window_size_callback(GLFWwindow* window, int width, int height)
{
    printf("Size changed %i %i\n", width, height);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    printf("Framebuffer size changed %i %i\n", width, height);
    glViewport(0, 0, width, height);
    displayDim.width = (float) width;
    displayDim.height = (float) height;
}


/*
float triangleVertices[] = {
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f
};*/

float joystickL[2];
float joystickR[2];
float triggerL, triggerR;
bool smallGap = true;
enum SelectPart {NONE = 0, BG, FG};
enum SelectPart colorSelected = NONE;

OverlaySettings overlay_defaultSettings = {
    .sizeFactor=20,
    .backgroundColor={.r=0.3, .g=0.6, .b=0.9},
    .foregroundColor={.r=0.45, .g=0.38, .b=0.4},
    .side=TOP
};

int main(void)
{
    /* Initialize the library */
    if (!glfwInit())
        return -1;

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* deskInfo = glfwGetVideoMode(monitor);
    
    OverlaySettings overlaySettings = overlay_defaultSettings;

    GLFWwindow* overlayWindow = createOverlayWindow(monitor, &overlaySettings);

    if (!overlayWindow)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(overlayWindow);

    /* INIT GLEW */
    glewExperimental = GL_TRUE; //? move right after glfw init ?
    glewInit();

    int width, height;
    glfwGetFramebufferSize(overlayWindow, &width, &height);
    displayDim.width = (float) width;
    displayDim.height = (float) height;

    //printf("GLFW Version : %s\n", glfwGetVersionString());

    //...glfwSetCursorPos(overlayWindow, 0, 0);

    /*UsableShaderData* overlayBackground = (UsableShaderData*) malloc(sizeof(UsableShaderData));
    createOverlayBackground(overlayBackground);
    GLint bgColorUniform = glGetUniformLocation(overlayBackground->shaderProgram, "backgroundColor");
    GLint fgColorUniform = glGetUniformLocation(overlayBackground->shaderProgram, "foregroundColor");
    GLint joystickLUniform = glGetUniformLocation(overlayBackground->shaderProgram, "joystickL");
    GLint joystickRUniform = glGetUniformLocation(overlayBackground->shaderProgram, "joystickR");*/

    /** Wheel **/
    int dispSize[2] = {(deskInfo->width*overlaySettings.sizeFactor)/100, (deskInfo->width*overlaySettings.sizeFactor)/100};//todo move otherwere to take into account resize and improve
    UsableShaderData* overlayWheel1 = (UsableShaderData*) malloc(sizeof(UsableShaderData));
    createOverlayWheel(overlayWheel1);
    GLint overlayWheel1_screenDimension = glGetUniformLocation(overlayWheel1->shaderProgram, "screenDimension");
    GLint overlayWheel1_relativeWheelSize = glGetUniformLocation(overlayWheel1->shaderProgram, "relativeWheelSize");
    GLint overlayWheel1_divCount = glGetUniformLocation(overlayWheel1->shaderProgram, "divCount");
    GLint overlayWheel1_center = glGetUniformLocation(overlayWheel1->shaderProgram, "center");
    GLint overlayWheel1_circleMinMax = glGetUniformLocation(overlayWheel1->shaderProgram, "circleMinMax");
    GLint overlayWheel1_partColor = glGetUniformLocation(overlayWheel1->shaderProgram, "partColor");
    GLint overlayWheel1_backgroundColor = glGetUniformLocation(overlayWheel1->shaderProgram, "backgroundColor");
    GLint overlayWheel1_part = glGetUniformLocation(overlayWheel1->shaderProgram, "part");
    GLint overlayWheel1_segmentEnabled = glGetUniformLocation(overlayWheel1->shaderProgram, "segmentEnabled");
    GLint overlayWheel1_segmentColor = glGetUniformLocation(overlayWheel1->shaderProgram, "segmentColor");

    glUniform2f(overlayWheel1_relativeWheelSize, 0.5, 0.5);
    glUniform4f(overlayWheel1_segmentColor, 0.0, 0.7, 0.9, 0.8);
    GLboolean segmentEnabled = false;

    /** Scroll **/
    UsableShaderData* overlayScroll1 = (UsableShaderData*) malloc(sizeof(UsableShaderData));
    createOverlayScroll(overlayScroll1);
    GLint overlayScroll1_scrollDimension = glGetUniformLocation(overlayScroll1->shaderProgram, "scrollDimension");
    GLint overlayScroll1_divCount = glGetUniformLocation(overlayScroll1->shaderProgram, "divCount");
    GLint overlayScroll1_bottomLeft = glGetUniformLocation(overlayScroll1->shaderProgram, "bottomLeft");
    GLint overlayScroll1_partColor = glGetUniformLocation(overlayScroll1->shaderProgram, "partColor");
    GLint overlayScroll1_backgroundColor = glGetUniformLocation(overlayScroll1->shaderProgram, "backgroundColor");
    GLint overlayScroll1_part = glGetUniformLocation(overlayScroll1->shaderProgram, "part");
    GLint overlayScroll1_segmentEnabled = glGetUniformLocation(overlayScroll1->shaderProgram, "segmentEnabled");
    GLint overlayScroll1_segmentColor = glGetUniformLocation(overlayScroll1->shaderProgram, "segmentColor");

    glUniform4f(overlayScroll1_segmentColor, 0.0, 0.7, 0.9, 0.8);
    glUniform4f(overlayScroll1_partColor, 0.87, 0.17, 0.85, 1);
    glUniform4f(overlayScroll1_backgroundColor, 0.1, 0.6, 0.8, 0.5);

    //glfwSetKeyCallback(window, key_callback);
    //glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetJoystickCallback(joystick_callback);
    glfwSetWindowSizeCallback(overlayWindow, window_size_callback);
    glfwSetFramebufferSizeCallback(overlayWindow, framebuffer_size_callback);
    //glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);
    
    
    GLFWgamepadstate lastState;
    /*glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);*/
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(overlayWindow))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);
        //drawView();
        
        //glClearColor(0.4f, 0.2f, 0.3f, 0.4f);
        

        //Draw background
        /*glUseProgram(overlayBackground->shaderProgram);
        glUniform2f(joystickLUniform, joystickL[0], -joystickL[1]);
        glUniform2f(joystickRUniform, joystickR[0], -joystickR[1]);
        glUniform4f(bgColorUniform, overlaySettings.backgroundColor.r, overlaySettings.backgroundColor.g, overlaySettings.backgroundColor.b, 0.8);
        glUniform4f(fgColorUniform, overlaySettings.foregroundColor.r, overlaySettings.foregroundColor.g, overlaySettings.foregroundColor.b, 1);
        glBindVertexArray(overlayBackground->vao);
        overlayBackground->drawFunction();*/
        
        if(fabsf(joystickL[0]) > 0.5 || fabsf(joystickL[1]) > 0.5 || fabsf(joystickR[0]) > 0.5 || fabsf(joystickR[1]) > 0.5){
            glUseProgram(overlayWheel1->shaderProgram);
            glBindVertexArray(overlayWheel1->vao);
            if(fabsf(joystickL[0]) > 0.5 || fabsf(joystickL[1]) > 0.5){
                GLuint wheelDivCount = 10;
                //float angle = atan2(v.y/v.x) + PI; // angle from the point [-1, 0] in reverse clock cycle
                float angle = atan2f(joystickL[1],joystickL[0]) + PI; //angle in inverse clock cycle from 0 to 2*PI
                GLuint wheelDiv = ((GLuint)(angle / (2*PI/wheelDivCount))) % wheelDivCount;
                printf("Part1 : %i\n", wheelDiv);
                glUniform1ui(overlayWheel1_divCount, wheelDivCount);
                glUniform2f(overlayWheel1_circleMinMax, 0.3, 0.6);
                glUniform1i(overlayWheel1_segmentEnabled, segmentEnabled);
                glUniform1ui(overlayWheel1_part, wheelDiv);
                glUniform2f(overlayWheel1_screenDimension, displayDim.width, displayDim.height); // todo improve
                glUniform2f(overlayWheel1_center, displayDim.width/2, displayDim.height/2); // todo improve
                glUniform4f(overlayWheel1_partColor, 0.87, 0.17, 0.85, 1);
                glUniform4f(overlayWheel1_backgroundColor, 0.3, 0.0, 0.0, 0.5);
                overlayWheel1->drawFunction();
            }

            if(fabsf(joystickR[0]) > 0.5 || fabsf(joystickR[1]) > 0.5){
                GLuint wheelDivCount = 20;
                float angle = atan2f(joystickR[1],joystickR[0]) + PI; //angle in inverse clock cycle from 0 to 2*PI
                GLuint wheelDiv = ((GLuint) (angle / (2*PI/wheelDivCount))) % wheelDivCount;
                printf("Part2 : %i\n", wheelDiv);
                glUniform1ui(overlayWheel1_divCount, wheelDivCount);
                glUniform2f(overlayWheel1_circleMinMax, 0.6, 1);
                glUniform1i(overlayWheel1_segmentEnabled, segmentEnabled);
                glUniform1ui(overlayWheel1_part, wheelDiv);
                glUniform2f(overlayWheel1_screenDimension, displayDim.width, displayDim.height); // todo improve
                glUniform2f(overlayWheel1_center, displayDim.width/2, displayDim.height/2); // todo improve
                glUniform4f(overlayWheel1_partColor, 0.87, 0.17, 0.85, 1);
                glUniform4f(overlayWheel1_backgroundColor, 0.1, 0.6, 0.8, 0.5);
                overlayWheel1->drawFunction();
            }
        }

        if(triggerR != -1){
            glUseProgram(overlayScroll1->shaderProgram);
            glBindVertexArray(overlayScroll1->vao);
            
            GLuint scrollDivCount = 10;
            GLuint scrollDiv = ((GLuint) ((triggerR+1)/2*(scrollDivCount-0.00001)));
            printf("%f Ltrig part : %i\n", triggerR, scrollDiv);
            
            glUniform2f(overlayScroll1_scrollDimension, displayDim.width/5, displayDim.height/3);
            glUniform2f(overlayScroll1_bottomLeft, displayDim.width-displayDim.width/5, (displayDim.height-displayDim.height/3)/2);
            glUniform1ui(overlayScroll1_divCount, scrollDiv);

            overlayScroll1->drawFunction();
        }


        /* Swap front and back buffers */
        glfwSwapBuffers(overlayWindow);

        /* Poll for and process events */
        //glfwPollEvents();

        glfwWaitEventsTimeout(1.0f/FPS);

        GLFWgamepadstate state;
        //https://www.glfw.org/docs/3.3/structGLFWgamepadstate.html
        //https://www.glfw.org/docs/3.3/group__gamepad__axes.html
        //https://www.glfw.org/docs/3.3/group__gamepad__buttons.html
        if (glfwGetGamepadState(GLFW_JOYSTICK_1, &state))
        {
            joystickL[0] = state.axes[GLFW_GAMEPAD_AXIS_LEFT_X];
            joystickL[1] = state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y];

            joystickR[0] = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X];
            joystickR[1] = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y];

            triggerL = state.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER];
            triggerR = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER];

            tr = (joystickL[0]+1)/2;
            tg = (joystickL[1]+1)/2;
            tb = (joystickR[0]+1)/2;
            if(colorSelected == BG){
                overlaySettings.backgroundColor.r = tr;
                overlaySettings.backgroundColor.g = tg;
                overlaySettings.backgroundColor.b = tb;
            }else if(colorSelected == FG){
                overlaySettings.foregroundColor.r = tr;
                overlaySettings.foregroundColor.g = tg;
                overlaySettings.foregroundColor.b = tb;
            }
            /*if(abs(joystickL[0]) > 0.05  || abs(joystickL[1]) > 0.05){
                printf("part : %i \n", (int) (((atan(joystickL[0]/-joystickL[1])+PI/2)/radians(45.0)))); //TODO
            }*/
            //printf("%f %f %f\n", tr, tv, tb);
            if (state.buttons[GLFW_GAMEPAD_BUTTON_A])
            {
                printf("A pressed L(%f,%f) R(%f,%f)\n", state.axes[GLFW_GAMEPAD_AXIS_LEFT_X], state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y], state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X], state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y]);
            }
            if(state.buttons[GLFW_GAMEPAD_BUTTON_B] && state.buttons[GLFW_GAMEPAD_BUTTON_B] != lastState.buttons[GLFW_GAMEPAD_BUTTON_B]){
                if(colorSelected == NONE){
                    colorSelected = BG;
                }else if(colorSelected == BG){
                    colorSelected = FG;
                }else{
                    colorSelected = NONE;
                }
            }
            if(state.buttons[GLFW_GAMEPAD_BUTTON_X] && state.buttons[GLFW_GAMEPAD_BUTTON_X] != lastState.buttons[GLFW_GAMEPAD_BUTTON_X]){
                segmentEnabled = !segmentEnabled;
            }
            if(state.buttons[GLFW_GAMEPAD_BUTTON_GUIDE] && state.buttons[GLFW_GAMEPAD_BUTTON_GUIDE] != lastState.buttons[GLFW_GAMEPAD_BUTTON_GUIDE]){
                //need timing
                smallGap = !smallGap;
                if(smallGap){
                    glfwSetWindowSize(overlayWindow, (deskInfo->width*overlaySettings.sizeFactor)/100, 3);
                }else{
                    glfwSetWindowSize(overlayWindow, (deskInfo->width*overlaySettings.sizeFactor)/100, (deskInfo->width*overlaySettings.sizeFactor)/100);
                }
            }
            lastState = state;
        }
    }

    glfwTerminate();
    return 0;
}