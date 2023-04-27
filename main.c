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
float leftSensitivity = 5, rightSensitivity = 3;

GLuint wheelDiv;
GLuint wheelDivCount = 8;

GLuint wheel2Div;
GLuint wheel2DivCount = 16;

OverlaySettings overlay_defaultSettings = {
    .sizeFactor=10,
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
    
    glfwSetJoystickCallback(joystick_callback);
    glfwSetWindowSizeCallback(overlayWindow, window_size_callback);
    glfwSetFramebufferSizeCallback(overlayWindow, framebuffer_size_callback);
    
    
    GLFWgamepadstate lastState;
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    /** Wheel **/
    int dispSize[2] = {width, height};
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
    /*UsableShaderData* overlayScroll1 = (UsableShaderData*) malloc(sizeof(UsableShaderData));
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

    glUniform4f(overlayScroll1_segmentColor, 0.0, 1, 0.7, 0.8);*/

    /** Knob **/
    UsableShaderData* overlayKnob = (UsableShaderData*) malloc(sizeof(UsableShaderData));
    createOverlayKnob(overlayKnob);
    GLint overlayKnob_colorIn = glGetUniformLocation(overlayKnob->shaderProgram, "colorIn");
    GLint overlayKnob_colorOut = glGetUniformLocation(overlayKnob->shaderProgram, "colorOut");
    GLint overlayKnob_dimension = glGetUniformLocation(overlayKnob->shaderProgram, "dimension");

    glUniform4f(overlayKnob_colorIn, 0.1, 0.6, 0.8, 0.8);
    glUniform4f(overlayKnob_colorOut, 0, 0, 0, 0);
    glUniform2f(overlayKnob_dimension, displayDim.width, height/5);

    /** Text **/
    //todo find how to calculate x, y instead of setting arbitrary -0.15, 0.1
    Atlas atlas = createTextAtlas(32, 1<<8, "fonts/Roboto-Bold.ttf", 64, 0);
    wchar_t* text1 = L"ABCDEFGHIJKLM";
    DrawableText drawableText1 = createDrawableTextWheelUsingAtlas(text1, &atlas, -0.1, 0.1, 1.f/width, 1.f/height, 0.8, 0.8);
    wchar_t* text2 = L"NOPQRSTUVWXYZ";
    DrawableText drawableText2 = createDrawableTextWheelUsingAtlas(text2, &atlas, -0.1, 0.1, 1.f/width, 1.f/height, 0.8, 0.8);
    wchar_t* text3 = L"0123456789";
    DrawableText drawableText3 = createDrawableTextWheelUsingAtlas(text3, &atlas, -0.1, 0.1, 1.f/width, 1.f/height, 0.8, 0.8);
    wchar_t* text4 = L" ()[]{}_\"'#&$@ ";
    DrawableText drawableText4 = createDrawableTextWheelUsingAtlas(text4, &atlas, -0.1, 0.1, 1.f/width, 1.f/height, 0.8, 0.8);
    wchar_t* text5 =L",.!?:;^¨°`~";
    DrawableText drawableText5 = createDrawableTextWheelUsingAtlas(text5, &atlas, -0.1, 0.1, 1.f/width, 1.f/height, 0.8, 0.8);
    wchar_t* text6 = L"=-+*/%";
    DrawableText drawableText6 = createDrawableTextWheelUsingAtlas(text6, &atlas, -0.1, 0.1, 1.f/width, 1.f/height, 0.8, 0.8);
    

    wchar_t* textCenter = L"AQ0(;+";
    DrawableText drawableTextCenter = createDrawableTextWheelUsingAtlas(textCenter, &atlas, -0.05, 0.05, .6f/width, .6f/height, 0.35, 0.35);
    wheelDivCount = wcslen(textCenter);
    printf("drawableText done\n");

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(overlayWindow))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        if(mouseMode){
            // display only a knob, no text, color showing if scroll actived, selection,...
            glUseProgram(overlayKnob->shaderProgram);
            glBindVertexArray(overlayKnob->vao);
            overlayKnob->drawFunction();
        }else{
            
            /*
            if(triggerL != -1 || triggerR != -1){
                glUseProgram(overlayScroll1->shaderProgram);
                glBindVertexArray(overlayScroll1->vao);
                
                glUniform1i(overlayScroll1_segmentEnabled, segmentEnabled);
                if(triggerR != -1){
                    GLuint scrollDivCount = 4;
                    GLuint scrollDiv = scrollDivCount - ((GLuint) ceilf((triggerR+1)/2*scrollDivCount));
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
                    glUniform1i(overlayScroll1_partGradient, false);
                    glUniform1i(overlayScroll1_horizontal, false);
                    glUniform2f(overlayScroll1_scrollDimension, displayDim.width, displayDim.width);
                    glUniform2f(overlayScroll1_bottomLeft, 0, 0);
                    glUniform1ui(overlayScroll1_divCount, scrollDivCount);
                    glUniform1ui(overlayScroll1_part, scrollDiv);

                    overlayScroll1->drawFunction();
                }
            }
            */

            if(fabsf(joystickL[0]) > 0.5 || fabsf(joystickL[1]) > 0.5 || fabsf(joystickR[0]) > 0.5 || fabsf(joystickR[1]) > 0.5){
                glUseProgram(overlayWheel1->shaderProgram);
                glBindVertexArray(overlayWheel1->vao);

                glUniform2f(overlayWheel1_wheelDimension, displayDim.width, displayDim.width);
                glUniform2f(overlayWheel1_center, displayDim.width/2, displayDim.width/2);
                glUniform1i(overlayWheel1_segmentEnabled, segmentEnabled);
                if(fabsf(joystickR[0]) > 0.5 || fabsf(joystickR[1]) > 0.5){
                    float angle = fmodf(atan2f(joystickR[1],joystickR[0]) - PI/2 + PI + 2*PI, 2*PI); //angle in inverse clock cycle from 0 to 2*PI
                    wheel2Div = (GLuint)(angle / (2*PI/wheel2DivCount));
                    printf("div %i : %i ; %f\n", wheel2Div, wheel2DivCount, angle);
                    glUniform1ui(overlayWheel1_divCount, wheel2DivCount);
                    glUniform2f(overlayWheel1_circleMinMax, 0.5, 1);
                    glUniform1ui(overlayWheel1_part, wheel2Div);
                    glUniform4f(overlayWheel1_partColor, 0.87, 0.17, 0.85, 1);
                    glUniform4f(overlayWheel1_backgroundColor, 0.1, 0.6, 0.8, 1);
                    overlayWheel1->drawFunction();
                }

                if(fabsf(joystickL[0]) > 0.5 || fabsf(joystickL[1]) > 0.5){
                    //float angle = atan2(v.y/v.x) + PI; // angle from the point [-1, 0] in reverse clock cycle
                    float angle = fmodf(atan2f(joystickL[1],joystickL[0]) - PI/2 + PI + 2*PI, 2*PI); //angle in inverse clock cycle from 0 to 2*PI
                    wheelDiv = (GLuint)(angle / (2*PI/(wheelDivCount)));
                    glUniform1ui(overlayWheel1_divCount, wheelDivCount);
                    glUniform2f(overlayWheel1_circleMinMax, 0.2, 0.5);
                    glUniform1ui(overlayWheel1_part, wheelDiv);
                    glUniform4f(overlayWheel1_partColor, 0.87, 0.17, 0.85, 1);
                    glUniform4f(overlayWheel1_backgroundColor, 0.3, 0.0, 0.0, 1);
                    overlayWheel1->drawFunction();

                    ColorRGBAf textColor = {.r=1, .g=1, .b=1, .a=1.f};
                    drawText(&drawableTextCenter, textColor);
                    if(wheelDiv == 0){
                        wheel2DivCount = wcslen(text1);
                        drawText(&drawableText1, textColor);
                    }else if(wheelDiv == 1){
                        wheel2DivCount = wcslen(text2);
                        drawText(&drawableText2, textColor);
                    }else if(wheelDiv == 2){
                        wheel2DivCount = wcslen(text3);
                        drawText(&drawableText3, textColor);
                    }else if(wheelDiv == 3){
                        wheel2DivCount = wcslen(text4);
                        drawText(&drawableText4, textColor);
                    }else if(wheelDiv == 4){
                        wheel2DivCount = wcslen(text5);
                        drawText(&drawableText5, textColor);
                    }else if(wheelDiv == 5){
                        wheel2DivCount = wcslen(text6);
                        drawText(&drawableText6, textColor);
                    }
                }
            }
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
                if(triggerR>-1){
                    mouseScroll((int) -powf(2,5*(triggerR+1)/2));
                }

                if(triggerL>-1){
                    mouseScroll((int) powf(2,5*(triggerL+1)/2));
                }

                // Mouse movement on left joystick
                if(fabsf(joystickL[0]) > 0){
                    mouseMove(copysignf(powf(2,fabs(joystickL[0])*leftSensitivity)-1, joystickL[0]), copysignf(powf(2,fabs(joystickL[1])*leftSensitivity)-1, joystickL[1]));
                }

                if(fabsf(joystickR[1]) > 0){
                    mouseMove(copysignf(powf(2,fabs(joystickR[0])*rightSensitivity)-1, joystickR[0]), copysignf(powf(2,fabs(joystickR[1])*rightSensitivity)-1, joystickR[1]));
                }
                
            }else{

                if(fabsf(joystickL[0]) > 0.5 || fabsf(joystickL[1]) > 0.5 || fabsf(joystickR[0]) > 0.5 || fabsf(joystickR[1]) > 0.5){
                    if(lastState.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER] != bumperR && bumperR || lastState.buttons[GLFW_GAMEPAD_BUTTON_LEFT_BUMPER] != bumperL && bumperL){ // Both bumpers works, useful when using buttons (ctrl,..)
                        wchar_t key = 0;
                        if(wheelDiv == 0){
                            key = text1[wheel2Div];
                        }else if(wheelDiv == 1){
                            key = text2[wheel2Div];
                        }else if(wheelDiv == 2){
                            key = text3[wheel2Div];
                        }else if(wheelDiv == 3){
                            key = text4[wheel2Div];
                        }else if(wheelDiv == 4){
                            key = text5[wheel2Div];
                        }else if(wheelDiv == 5){
                            key = text6[wheel2Div];
                        }

                        printf("key %c : %x\n", key, key);
                        //for maj letters
                        if(key >= 'A' && key <= 'Z'){
                             sendKey(key);
                             sendKeyEnd(key);
                        }else if(key != 0){
                            sendChar(key);
                        }
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

            if(lastState.buttons[GLFW_GAMEPAD_BUTTON_B] != buttonB){
                if(buttonB){
                    sendKey(VK_MENU);
                }else{
                    sendKeyEnd(VK_MENU);
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

            if(state.buttons[GLFW_GAMEPAD_BUTTON_GUIDE] && state.buttons[GLFW_GAMEPAD_BUTTON_GUIDE] != lastState.buttons[GLFW_GAMEPAD_BUTTON_GUIDE]){
                
                mouseMode = !mouseMode;

                if(mouseMode){
                    glfwSetWindowSize(overlayWindow, width, height/5);
                }else{
                    glfwSetWindowSize(overlayWindow, width, height);
                }
            }
            lastState = state;
        }
    }

    glfwTerminate();
    return 0;
}