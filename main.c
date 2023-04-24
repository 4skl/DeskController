#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <overlay.h>
#include <draw_text.h>
#include <send_inputs.h>

#define FPS 144

//force use dedicated graphic card //https://stackoverflow.com/questions/16823372/forcing-machine-to-use-dedicated-graphics-card
__declspec(dllexport) unsigned long NvOptimusEnablement = 1;
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;

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
bool bumperL, bumperR;
bool start, back, guide;
bool buttonA, buttonB, buttonX, buttonY;
bool buttonDpadUp, buttonDpadDown, buttonDpadLeft, buttonDpadRight;

bool mouseMode = false;
float leftSensitivity = 15, rightSensitivity = 3;

GLuint wheelDiv;
GLuint wheelDivCount = 8;

GLuint wheel2Div;
GLuint wheel2DivCount = 16;

OverlaySettings overlay_defaultSettings = {
    .sizeFactor=20,
    .backgroundColor={.r=0.3, .g=0.6, .b=0.9},
    .foregroundColor={.r=0.45, .g=0.38, .b=0.4},
    .side=TOP,
};

int main(int argc,char *argv[])
{
    /* Initialize the GLFW library */
    if (!glfwInit())
        return -1;

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();

    const GLFWvidmode* deskInfo = glfwGetVideoMode(monitor);

    int width_mm, height_mm;
    glfwGetMonitorPhysicalSize(monitor, &width_mm, &height_mm);
    int dpix, dpiy;
    dpix = ((float) deskInfo->width/width_mm)*25.4;
    dpiy = ((float) deskInfo->height/height_mm)*25.4;
    printf("DPI : w=%i, h=%i\n", dpix, dpiy);
    
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
    GLint overlayWheel1_wheelDimension = glGetUniformLocation(overlayWheel1->shaderProgram, "wheelDimension");
    GLint overlayWheel1_divCount = glGetUniformLocation(overlayWheel1->shaderProgram, "divCount");
    GLint overlayWheel1_center = glGetUniformLocation(overlayWheel1->shaderProgram, "center");
    GLint overlayWheel1_circleMinMax = glGetUniformLocation(overlayWheel1->shaderProgram, "circleMinMax");
    GLint overlayWheel1_partColor = glGetUniformLocation(overlayWheel1->shaderProgram, "partColor");
    GLint overlayWheel1_backgroundColor = glGetUniformLocation(overlayWheel1->shaderProgram, "backgroundColor");
    GLint overlayWheel1_part = glGetUniformLocation(overlayWheel1->shaderProgram, "part");
    GLint overlayWheel1_segmentEnabled = glGetUniformLocation(overlayWheel1->shaderProgram, "segmentEnabled");
    GLint overlayWheel1_segmentColor = glGetUniformLocation(overlayWheel1->shaderProgram, "segmentColor");
    GLint overlayWheel1_partGradient = glGetUniformLocation(overlayWheel1->shaderProgram, "partGradient");

    glUniform1i(overlayWheel1_partGradient, true);
    glUniform4f(overlayWheel1_segmentColor, 0.0, 1, 0.7, 0.8);
    GLboolean segmentEnabled = true;

    /** Scroll **/
    UsableShaderData* overlayScroll1 = (UsableShaderData*) malloc(sizeof(UsableShaderData));
    createOverlayScroll(overlayScroll1);
    GLint overlayScroll1_scrollDimension = glGetUniformLocation(overlayScroll1->shaderProgram, "scrollDimension");
    GLint overlayScroll1_horizontal = glGetUniformLocation(overlayScroll1->shaderProgram, "horizontal");
    GLint overlayScroll1_divCount = glGetUniformLocation(overlayScroll1->shaderProgram, "divCount");
    GLint overlayScroll1_bottomLeft = glGetUniformLocation(overlayScroll1->shaderProgram, "bottomLeft");
    GLint overlayScroll1_partColor = glGetUniformLocation(overlayScroll1->shaderProgram, "partColor");
    GLint overlayScroll1_backgroundColor = glGetUniformLocation(overlayScroll1->shaderProgram, "backgroundColor");
    GLint overlayScroll1_part = glGetUniformLocation(overlayScroll1->shaderProgram, "part");
    GLint overlayScroll1_segmentEnabled = glGetUniformLocation(overlayScroll1->shaderProgram, "segmentEnabled");
    GLint overlayScroll1_segmentColor = glGetUniformLocation(overlayScroll1->shaderProgram, "segmentColor");
    GLint overlayScroll1_partGradient = glGetUniformLocation(overlayScroll1->shaderProgram, "partGradient");


    glUniform4f(overlayScroll1_segmentColor, 0.0, 1, 0.7, 0.8);
    //glUniform4f(overlayScroll1_backgroundColor, 1, 0, 0, 1);

    /** Text **/
    Atlas atlas = createTextAtlas(32, 1<<8, "fonts/NotoSansJP-Bold.otf", 64, 0);
    unsigned long* text1 = wcharToULong(L"ABCDEFGHIJKLMNOP");
    DrawableText drawableText1 = createDrawableTextWheelUsingAtlas(text1, &atlas, -0.1, 0.45, 1/230.0, 1/384.0, 0.8, 0.8*230/384, true);
    unsigned long* text2 = wcharToULong(L"QRSTUVWXYZ,.!?:;");
    DrawableText drawableText2 = createDrawableTextWheelUsingAtlas(text2, &atlas, -0.1, 0.45, 1/230.0, 1/384.0, 0.8, 0.8*230/384, true);
    unsigned long* text3 = wcharToULong(L"0123456789=-+*/%");
    DrawableText drawableText3 = createDrawableTextWheelUsingAtlas(text3, &atlas, -0.1, 0.45, 1/230.0, 1/384.0, 0.8, 0.8*230/384, true);
    unsigned long* text4 = wcharToULong(L" ()[]{}_\"'#&$@ ");
    DrawableText drawableText4 = createDrawableTextWheelUsingAtlas(text4, &atlas, -0.1, 0.45, 1/230.0, 1/384.0, 0.8, 0.8*230/384, true);

    unsigned long* textCenter = wcharToULong(L"AQ0(    ");
    DrawableText drawableTextCenter = createDrawableTextWheelUsingAtlas(textCenter, &atlas, -0.1, 0.45, 0.6/230.0, 0.6/384.0, 0.4, 0.4*230/384, true);
    printf("drawableText done\n");
    /*
    for(unsigned int i = 0; i<drawableText.vertices_count;i++){
        printf("%f, ", drawableText.vertices[i]);
    }
    printf("\n");

    for(unsigned int i = 0; i<drawableText.elements_count;i++){
        printf("%i, ", drawableText.elements[i]);
    }
    printf("\n");
    */
    /*CharacterList cl_text1 = genTextCharacters(text1, "fonts/OpenSans-Bold.ttf", 0, 64);
    UsableShaderDataInput* overlayText1 = (UsableShaderDataInput*) malloc(sizeof(UsableShaderDataInput));
    createText(overlayText1);
    GLint overlayText1_textColor = glGetUniformLocation(overlayText1, "textColor");
    GLint overlayText1_projection = glGetUniformLocation(overlayText1, "projection");

    glUniform4f(overlayText1_textColor, 0, 1, 0, 1); //set text color to green
    */

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
    //glBlendFunc(GL_ONE, GL_ONE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    //Set window size big (mouseMode = false)
    /*if(smallGap){
        glfwSetWindowSize(overlayWindow, (deskInfo->width*overlaySettings.sizeFactor)/100, 3);
    }else{*/
    glfwSetWindowSize(overlayWindow, (deskInfo->width*overlaySettings.sizeFactor)/100*3/5, (deskInfo->width*overlaySettings.sizeFactor)/100);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(overlayWindow))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        /***/
        //drawView();
        
        //glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        

        //Draw background
        /*glUseProgram(overlayBackground->shaderProgram);
        glUniform2f(joystickLUniform, joystickL[0], -joystickL[1]);
        glUniform2f(joystickRUniform, joystickR[0], -joystickR[1]);
        glUniform4f(bgColorUniform, overlaySettings.backgroundColor.r, overlaySettings.backgroundColor.g, overlaySettings.backgroundColor.b, 0.8);
        glUniform4f(fgColorUniform, overlaySettings.foregroundColor.r, overlaySettings.foregroundColor.g, overlaySettings.foregroundColor.b, 1);
        glBindVertexArray(overlayBackground->vao);
        overlayBackground->drawFunction();*/

        if(mouseMode){
            // display only a knob, no text, color showing if scroll actived, selection,...
        }else{

            if(triggerL != -1 || triggerR != -1){
                glUseProgram(overlayScroll1->shaderProgram);
                glBindVertexArray(overlayScroll1->vao);
                
                glUniform1i(overlayScroll1_segmentEnabled, segmentEnabled);
                if(triggerR != -1){
                    GLuint scrollDivCount = 4;
                    GLuint scrollDiv = scrollDivCount - ((GLuint) ceilf((triggerR+1)/2*scrollDivCount));
                    printf("%f Rtrig part : %i\n", triggerR, scrollDiv);
                    glUniform1i(overlayScroll1_partGradient, true);
                    glUniform1i(overlayScroll1_horizontal, true);
                    glUniform2f(overlayScroll1_scrollDimension, displayDim.width, displayDim.height/5);
                    glUniform2f(overlayScroll1_bottomLeft, 0, displayDim.height-displayDim.height/5);
                    glUniform1ui(overlayScroll1_divCount, scrollDivCount);
                    glUniform1ui(overlayScroll1_part, scrollDiv);
                    glUniform4f(overlayScroll1_partColor, 0.87, 0.17, 0.85, 1);

                    overlayScroll1->drawFunction();
                }

                if(triggerL != -1){ //at 0 cause like that or physically broken ?
                    GLuint scrollDivCount = 2;
                    GLuint scrollDiv = scrollDivCount - ((GLuint) ceilf((triggerL+1)/2*scrollDivCount));
                    printf("%f Ltrig part : %i\n", triggerL, scrollDiv);
                    glUniform1i(overlayScroll1_partGradient, false);
                    glUniform1i(overlayScroll1_horizontal, false);
                    glUniform2f(overlayScroll1_scrollDimension, displayDim.width, displayDim.width);
                    glUniform2f(overlayScroll1_bottomLeft, 0, 0);
                    glUniform1ui(overlayScroll1_divCount, scrollDivCount);
                    glUniform1ui(overlayScroll1_part, scrollDiv);

                    overlayScroll1->drawFunction();
                }
            }

            if(fabsf(joystickL[0]) > 0.5 || fabsf(joystickL[1]) > 0.5 || fabsf(joystickR[0]) > 0.5 || fabsf(joystickR[1]) > 0.5){
                glUseProgram(overlayWheel1->shaderProgram);
                glBindVertexArray(overlayWheel1->vao);

                glUniform2f(overlayWheel1_wheelDimension, displayDim.width, displayDim.width);
                glUniform2f(overlayWheel1_center, displayDim.width/2, displayDim.width/2);
                glUniform1i(overlayWheel1_segmentEnabled, segmentEnabled);
                if(fabsf(joystickR[0]) > 0.5 || fabsf(joystickR[1]) > 0.5){
                    float angle2 = atan2f(joystickR[1],joystickR[0]) + PI; //angle in inverse clock cycle from 0 to 2*PI
                    wheel2Div = ((GLuint) (angle2 / (2*PI/wheel2DivCount))) % wheel2DivCount;
                    printf("Part2 : %i\n", wheel2Div);
                    glUniform1ui(overlayWheel1_divCount, wheel2DivCount);
                    glUniform2f(overlayWheel1_circleMinMax, 0.5, 1);
                    glUniform1ui(overlayWheel1_part, wheel2Div);
                    glUniform4f(overlayWheel1_partColor, 0.87, 0.17, 0.85, 1);
                    glUniform4f(overlayWheel1_backgroundColor, 0.1, 0.6, 0.8, 1);
                    overlayWheel1->drawFunction();
                }

                if(fabsf(joystickL[0]) > 0.5 || fabsf(joystickL[1]) > 0.5){
                    //float angle = atan2(v.y/v.x) + PI; // angle from the point [-1, 0] in reverse clock cycle
                    float angle = atan2f(joystickL[1],joystickL[0]) + PI; //angle in inverse clock cycle from 0 to 2*PI
                    wheelDiv = ((GLuint)(angle / (2*PI/wheelDivCount))) % wheelDivCount;
                    printf("Part1 : %i\n", wheelDiv);
                    glUniform1ui(overlayWheel1_divCount, wheelDivCount);
                    glUniform2f(overlayWheel1_circleMinMax, 0.2, 0.5);
                    glUniform1ui(overlayWheel1_part, wheelDiv);
                    glUniform4f(overlayWheel1_partColor, 0.87, 0.17, 0.85, 1);
                    glUniform4f(overlayWheel1_backgroundColor, 0.3, 0.0, 0.0, 1);
                    overlayWheel1->drawFunction();

                    ColorRGBAf textColor = {.r=1, .g=1, .b=1, .a=1.f};
                    drawText(&drawableTextCenter, textColor);
                    if(wheelDiv == 2){
                        drawText(&drawableText1, textColor);
                    }else if(wheelDiv == 3){
                        drawText(&drawableText2, textColor);
                    }else if(wheelDiv == 4){
                        drawText(&drawableText3, textColor);
                    }else if(wheelDiv == 5){
                        drawText(&drawableText4, textColor);
                    }
                }
            }
        }
        
        /* Draw Text */
        //unsigned long text[4] = {65UL, 66UL, 67UL, 0UL};
        /*glUseProgram(overlayText1->shaderProgram);
        glUniform4f(overlayText1_projection, 0, width, 0, height);
        overlayText1->drawFunction(&cl_text1);*/
        //drawTexturesText(text1, "fonts/OpenSans-Bold.ttf", 0, 50, 0, 0, textColor);
        //drawTexturesText(text1, "fonts/OpenSans-Bold.ttf", 0, 100, 50, 0, textColor);

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

            bumperL = state.buttons[GLFW_GAMEPAD_BUTTON_LEFT_BUMPER];
            bumperR = state.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER];

            start = state.buttons[GLFW_GAMEPAD_BUTTON_START];
            back = state.buttons[GLFW_GAMEPAD_BUTTON_BACK];
            guide = state.buttons[GLFW_GAMEPAD_BUTTON_GUIDE];

            buttonA = state.buttons[GLFW_GAMEPAD_BUTTON_A];
            buttonB = state.buttons[GLFW_GAMEPAD_BUTTON_B];
            buttonX = state.buttons[GLFW_GAMEPAD_BUTTON_X];
            buttonY = state.buttons[GLFW_GAMEPAD_BUTTON_Y];

            buttonDpadUp = state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP];
            buttonDpadDown = state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN];
            buttonDpadLeft = state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_LEFT];
            buttonDpadRight = state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT];

            if(mouseMode){

                if(lastState.buttons[GLFW_GAMEPAD_BUTTON_BACK] != back){
                    if(back){
                        sendKey(VK_ESCAPE);
                    }else{
                        sendKeyEnd(VK_ESCAPE);
                    }
                }
                
                // Left/Right click on bumper
                if(lastState.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER] != bumperR){
                    if(bumperR){
                        mouseDown(VK_RBUTTON);
                    }else{
                        mouseUp(VK_RBUTTON);
                    }
                }

                if(lastState.buttons[GLFW_GAMEPAD_BUTTON_LEFT_BUMPER] != bumperL){
                    if(bumperL){
                        mouseDown(VK_LBUTTON);
                    }else{
                        mouseUp(VK_LBUTTON);
                    }
                }

                // Mouse scroll on triggers
                if(lastState.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER] != triggerR){
                    mouseScroll((int) (-3*(triggerR+1)/2));
                }else if(triggerR == 1){ // trigger fully pressed
                    mouseScroll(-4);
                }

                if(lastState.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER] != triggerL){
                    mouseScroll((int) (3*(triggerL+1)/2));
                }else if(triggerL == 1){ // trigger fully pressed
                    mouseScroll(4);
                }

                // Mouse movement on left joystick
                if(fabsf(joystickL[0]) > 0){
                    mouseMove(joystickL[0]*leftSensitivity, joystickL[1]*leftSensitivity);
                }

                if(fabsf(joystickR[1]) > 0){
                    mouseMove(joystickR[0]*rightSensitivity, joystickR[1]*rightSensitivity);
                }
                
            }else{

                if(fabsf(joystickL[0]) > 0.5 || fabsf(joystickL[1]) > 0.5 || fabsf(joystickR[0]) > 0.5 || fabsf(joystickR[1]) > 0.5){
                    if(lastState.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER] != bumperR && bumperR){
                        wchar_t key = 0;
                        if(wheelDiv == 2){
                            key = text1[((wheel2Div-wheel2DivCount/4)%wheel2DivCount)];
                        }else if(wheelDiv == 3){
                            key = text2[((wheel2Div-wheel2DivCount/4)%wheel2DivCount)];
                        }else if(wheelDiv == 4){
                            key = text3[((wheel2Div-wheel2DivCount/4)%wheel2DivCount)];
                        }else if(wheelDiv == 5){
                            key = text4[((wheel2Div-wheel2DivCount/4)%wheel2DivCount)];
                        }
                        //for maj letters
                        if(key >= 'A' && key <= 'Z'){
                             sendKey(key);
                             sendKeyEnd(key);
                        }else if(key != 0){
                            sendChar(key);
                        }
                        /*
                        switch(key){
                            case '*':
                                key = VK_MULTIPLY;
                                break;
                            case '+':
                                key = VK_ADD;
                                break;
                            case '|': 
                                key = VK_SEPARATOR;
                                break;
                            case '-':
                                key = VK_SUBTRACT;
                                break;
                            case '/':
                                key = VK_DIVIDE;
                                break;
                            //VK_DECIMAL	0x6E	Decimal key
                        }
                    */
                    }
                }

                if(lastState.buttons[GLFW_GAMEPAD_BUTTON_BACK] != back){
                    if(back){
                        sendKey(VK_BACK);
                    }else{
                        sendKeyEnd(VK_BACK);
                    }
                }

                if(lastState.buttons[GLFW_GAMEPAD_BUTTON_A] != buttonA){
                    if(buttonA){
                        sendKey(VK_RETURN);
                    }else{
                        sendKeyEnd(VK_RETURN);
                    }
                }
            }

            // Universal actions (ignore mouse mode)

            if(lastState.buttons[GLFW_GAMEPAD_BUTTON_X] != buttonX){
                if(buttonX){
                    sendKey(VK_SHIFT);
                }else{
                    sendKeyEnd(VK_SHIFT);
                }
            }

            if(lastState.buttons[GLFW_GAMEPAD_BUTTON_Y] != buttonY){
                if(buttonY){
                    sendKey(VK_CONTROL);
                }else{
                    sendKeyEnd(VK_CONTROL);
                }
            }

            if(lastState.buttons[GLFW_GAMEPAD_BUTTON_DPAD_LEFT] != buttonDpadLeft){
                if(buttonDpadLeft){
                    sendKey(VK_LEFT);
                    sendKeyEnd(VK_LEFT);
                }
            }

            if(lastState.buttons[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT] != buttonDpadRight){
                if(buttonDpadRight){
                    sendKey(VK_RIGHT);
                }else{
                    sendKeyEnd(VK_RIGHT);
                }
            }

            if(lastState.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP] != buttonDpadUp){
                if(buttonDpadUp){
                    sendKey(VK_UP);
                }else{
                    sendKeyEnd(VK_UP);
                }
            }

            if(lastState.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN] != buttonDpadDown){
                if(buttonDpadDown){
                    sendKey(VK_DOWN);
                }else{
                    sendKeyEnd(VK_DOWN);
                }
            }


/* useless
            float tr,tg,tb;
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
            //if(abs(joystickL[0]) > 0.05  || abs(joystickL[1]) > 0.05){
            //    printf("part : %i \n", (int) (((atan(joystickL[0]/-joystickL[1])+PI/2)/radians(45.0)))); //TODO
            //}
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
            */
            if(state.buttons[GLFW_GAMEPAD_BUTTON_GUIDE] && state.buttons[GLFW_GAMEPAD_BUTTON_GUIDE] != lastState.buttons[GLFW_GAMEPAD_BUTTON_GUIDE]){
                //need timing
                mouseMode = !mouseMode;

                if(mouseMode){
                    glfwSetWindowSize(overlayWindow, (deskInfo->width*overlaySettings.sizeFactor)/100, 3);
                }else{
                    glfwSetWindowSize(overlayWindow, (deskInfo->width*overlaySettings.sizeFactor)/100*3/5, (deskInfo->width*overlaySettings.sizeFactor)/100);
                }
            }
            lastState = state;
        }
    }

    glfwTerminate();
    return 0;
}