#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include <overlay.h>

#define FPS 60

//force use dedicated graphic card //https://stackoverflow.com/questions/16823372/forcing-machine-to-use-dedicated-graphics-card
__declspec(dllexport) unsigned long NvOptimusEnablement = 1;
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;

float tr,tg,tb;

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
    glViewport(0, 0, width, height);
}
/*
float triangleVertices[] = {
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f
};*/

float joystickL[2];
float joystickR[2];
bool smallGap = true;
enum SelectPart {NONE = 0, BG, FG};
enum SelectPart colorSelected = NONE;

OverlaySettings overlay_defaultSettings = {
    .sizeFactor=10,
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

    //...glfwSetCursorPos(overlayWindow, 0, 0);

    UsableShaderData* overlayBackground = (UsableShaderData*) malloc(sizeof(UsableShaderData));
    createOverlayBackground(overlayBackground);
   
    //glfwSetKeyCallback(window, key_callback);
    //glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetJoystickCallback(joystick_callback);
    glfwSetWindowSizeCallback(overlayWindow, window_size_callback);
    //glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);

    GLint bgColorUniform = glGetUniformLocation(overlayBackground->shaderProgram, "backgroundColor");
    GLint fgColorUniform = glGetUniformLocation(overlayBackground->shaderProgram, "foregroundColor");
    GLint joystickLUniform = glGetUniformLocation(overlayBackground->shaderProgram, "joystickL");
    GLint joystickRUniform = glGetUniformLocation(overlayBackground->shaderProgram, "joystickR");

    
    GLFWgamepadstate lastState;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(overlayWindow))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);
        //drawView();
        
        //glClearColor(0.4f, 0.2f, 0.3f, 0.4f);
        
        //Draw background
        glUniform2f(joystickLUniform, joystickL[0], -joystickL[1]);
        glUniform2f(joystickRUniform, joystickR[0], -joystickR[1]);
        glUniform4f(bgColorUniform, overlaySettings.backgroundColor.r, overlaySettings.backgroundColor.g, overlaySettings.backgroundColor.b, 0.8);
        glUniform4f(fgColorUniform, overlaySettings.foregroundColor.r, overlaySettings.foregroundColor.g, overlaySettings.foregroundColor.b, 1);
        glBindVertexArray(overlayBackground->vao);
        glUseProgram(overlayBackground->shaderProgram);
        overlayBackground->drawFunction();
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
            if(abs(joystickL[0]) > 0.05  || abs(joystickL[1]) > 0.05){
                printf("part : %i \n", (int) (((atan(joystickL[0]/-joystickL[1])+PI/2)/radians(45.0)))); //TODO
            }
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