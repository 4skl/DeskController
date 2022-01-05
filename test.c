#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdbool.h>

#define FPS 60

//force use dedicated graphic card //https://stackoverflow.com/questions/16823372/forcing-machine-to-use-dedicated-graphics-card
__declspec(dllexport) unsigned long NvOptimusEnablement = 1;
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;

float tr,tv,tb;

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
GLFWvidmode deskInfo;

struct WindowPercentage{
    float height;
    float width;
};
struct WindowPercentage windowPercentage = {0.1f, 0.3f};

int main(void)
{
    GLFWwindow* overlayWindow;

    /* Initialize the library */
    if (!glfwInit())
        return -1;
    
    
    /* get desktop informations */
    deskInfo = *glfwGetVideoMode(glfwGetPrimaryMonitor());
    printf("w : %i h : %i\n", deskInfo.width, deskInfo.height);

    /* Set options */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    glfwWindowHint(GLFW_DECORATED, GL_FALSE);
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GL_TRUE);
    glfwWindowHint(GLFW_FLOATING, GL_TRUE);

    /* Create a windowed mode window and its OpenGL context */
    overlayWindow = glfwCreateWindow((int)((float)deskInfo.width)*windowPercentage.width, 3, "DeskController", NULL, NULL); //glfwGetPrimaryMonitor() as 3rd argument for fullscreen

    if (!overlayWindow)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(overlayWindow);

    /* INIT GLEW */
    glewExperimental = GL_TRUE;
    glewInit();

    
    glfwSetWindowPos(overlayWindow, deskInfo.width/2 - ((int)((float)deskInfo.width)*windowPercentage.width)/2, 0);


    /* Create Shaders */

    //Vertex Shader
    const GLchar * vertexShaderCode = "#version 150 core\n" //? change version ?
        "in vec2 position;\n"
        "void main(){\n"
        "   gl_Position = vec4(position, 0.0, 1.0);\n"
        "}";
        
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderCode, NULL);
    glCompileShader(vertexShader);

    //* check compilation errors
    {
        GLint status;
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
        if(!status){
            char buffer[512];
            glGetShaderInfoLog(vertexShader, 512, NULL, buffer);
            fprintf(stderr, "Vertex Shader compilation error : %s\n", buffer);
            return -1;
        }
    }


    //Fragment Shader
    const GLchar * fragmentShaderCode = "#version 150 core\n"
        "out vec4 outColor;\n"
        //"uniform vec3 triangleColor;\n"
        "void main(){\n"
        "   outColor = vec4(0.9,0.5,0.6, 1.0);\n"
        "}";

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderCode, NULL);
    glCompileShader(fragmentShader);

    //* check compilation errors
    {
        GLint status;
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
        if(!status){
            char buffer[512];
            glGetShaderInfoLog(fragmentShader, 512, NULL, buffer);
            fprintf(stderr, "Fragment Shader compilation error : %s\n", buffer);
            return -1;
        }
    }

    //Create shader program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    glBindFragDataLocation(shaderProgram, 0, "outColor");

    glLinkProgram(shaderProgram);
    //not needed, but delete shaders as them have been linked into the program :
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader); 

    //* check compilation errors
    {
        GLint status;
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &status);
        if(!status){
            char buffer[512];
            glGetProgramInfoLog(shaderProgram, 512, NULL, buffer);
            fprintf(stderr, "Linking Shader program error : %s\n", buffer);
            return -1;
        }
    }


    GLfloat shapeVertices[] = {
        -1.0, 1.0,
        1.0, 1.0,
        -0.3, -1.0,
        0.3, -1.0,
        -0.3, 1.0,
        0.3, 1.0,
    };

    GLuint shapeElements[] = {
        0, 2, 4,
        2, 3, 4,
        3, 4, 5,
        1, 3, 5
    };
    
    /* Shader datas */
    /*GLfloat vertices[] = {
        0.0f, -0.5f, 1.0f, // Vertex 1 (X, Y, k)
        -0.5f, 0.5f, 0.5f, // Vertex 2 (X, Y, k)
        0.5f, 0.5f, 0.0f, // Vertex 3 (X, Y, k)
    };*/

    GLuint vbo, vao;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, sizeof(shapeVertices), shapeVertices, GL_STATIC_DRAW);

    //Load elements
    GLuint ebo;
    glGenBuffers(1, &ebo);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(shapeElements), shapeElements, GL_STATIC_DRAW);

    //Link vertex datas to the program
    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat), (GLvoid*)0); //3rd arg : normalize input if not floating point depending on the format; 4th arg : stride; 5th arg : offset 
    glEnableVertexAttribArray(posAttrib);

    glUseProgram(shaderProgram);
   
    //glfwSetKeyCallback(window, key_callback);
    //glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetJoystickCallback(joystick_callback);
    glfwSetWindowSizeCallback(overlayWindow, window_size_callback);
    //glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);
    
    /*unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);*/
    
    // useless cause already called glBindVertexArray(vao);

    //GLint uniColor = glGetUniformLocation(shaderProgram, "ptColor");

    
    GLFWgamepadstate lastState;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(overlayWindow))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);
        //drawView();
        
        glClearColor(0.1f, 0.2f, 0.3f, 0.4f);
        //glUniform3f(uniColor, tr, tv, tb);
        //glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);

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
            tv = (joystickL[1]+1)/2;
            tb = (joystickR[0]+1)/2;
            //printf("%f %f %f\n", tr, tv, tb);
            if (state.buttons[GLFW_GAMEPAD_BUTTON_A])
            {
                printf("A pressed L(%f,%f) R(%f,%f)\n", state.axes[GLFW_GAMEPAD_AXIS_LEFT_X], state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y], state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X], state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y]);
            }
            if(state.buttons[GLFW_GAMEPAD_BUTTON_X] && state.buttons[GLFW_GAMEPAD_BUTTON_X] != lastState.buttons[GLFW_GAMEPAD_BUTTON_X]){
                //need timing
                smallGap = !smallGap;
                printf("%i %i\n", (int)((float)deskInfo.width)*windowPercentage.width, (int)((float)deskInfo.height)*windowPercentage.height);
                if(smallGap){
                    glfwSetWindowSize(overlayWindow, (int)((float)deskInfo.width)*windowPercentage.width, 3);
                }else{
                    glfwSetWindowSize(overlayWindow, (int)((float)deskInfo.width)*windowPercentage.width, (int)((float)deskInfo.height)*windowPercentage.height);
                }
            }
            lastState = state;
        }
    }

    glfwTerminate();
    return 0;
}