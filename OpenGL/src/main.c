#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include "debug.h"
#include "shaders.h"

#define STRINGIZE(x) #x
#define STR(x) STRINGIZE(x)

//TODO:
// Fix to allow for larger grid sizes
// Add functionality to record the fps
// Add the ability to pause and resume the simulation
// Add the ability to load a custom initial state


#define WIDTH 800
#define HEIGHT 600
#define DIV_FACTOR 8

// Function prototypes
void initOpenGL();
GLuint createComputeShader();
GLuint createRenderShaders();
void createInputTexture(GLuint *inputTextureID, int width, int height, const float* data);
void createTexture(GLuint* textureID, int width, int height);
void render(GLuint textureID, GLuint renderProgram);

void error_callback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

int main() {
    // Initialize OpenGL context
    if (!glfwInit()) {
        printf("Failed to initialize GLFW\n");
        return -1;
    }

    glfwSetErrorCallback(error_callback);
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Compute Shader Game of Life", NULL, NULL);
    if (!window) {
        printf("Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    
    if (glewInit() != GLEW_OK) {
        printf("Failed to initialize GLEW\n");
        return -1;
    }

    glfwSetKeyCallback(window, key_callback);
    
    // Create compute shader and render shaders
    GLuint computeShaderProgram = createComputeShader();
    GLuint renderShaderProgram = createRenderShaders();
    srand(time(NULL));
    // Create initial state for Game of Life
    float initialStateData[WIDTH * HEIGHT * 4];
    for (int i = 0; i < WIDTH * HEIGHT * 4; i += 4) {
        // Initialize with some pattern or random state
        float state = (rand() % 2 == 0) ? 1.0f : 0.0f;
        initialStateData[i + 0] = state; // Alive or dead cell
        initialStateData[i + 1] = state; // Unused
        initialStateData[i + 2] = state; // Unused
        initialStateData[i + 3] = 1.0f; // Alpha channel
    }
    
    // Create input and output textures
    GLuint inputTextureID, outputTextureID;
    createInputTexture(&inputTextureID, WIDTH, HEIGHT, initialStateData);
    createTexture(&outputTextureID, WIDTH, HEIGHT);

    // Main render loop
    while (!glfwWindowShouldClose(window)) {
        // Use compute shader to calculate next state
        glUseProgram(computeShaderProgram);
        checkGLError();
        // Bind the input texture
        glActiveTexture(GL_TEXTURE1);
        checkGLError();
        glBindTexture(GL_TEXTURE_2D, inputTextureID);
        checkGLError();
        // Bind the output texture
        glBindImageTexture(0, outputTextureID, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
        checkGLError();

        // Dispatch compute shader
        glDispatchCompute((GLuint)WIDTH / DIV_FACTOR, (GLuint)HEIGHT / DIV_FACTOR, 1);
        checkGLError();
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
        checkGLError();
        
        // Render the current state to the screen
        render(outputTextureID, renderShaderProgram);
        
        // Swap textures for next iteration
        GLuint temp = inputTextureID;
        inputTextureID = outputTextureID;
        outputTextureID = temp;

        glfwSwapBuffers(window);
        glfwPollEvents();
        // Limit to 60 FPS
        //usleep(1000 * 1000 / 60);
    }
    
    // Cleanup
    glDeleteProgram(computeShaderProgram);
    checkGLError();
    glDeleteProgram(renderShaderProgram);
    checkGLError();
    glDeleteTextures(1, &inputTextureID);
    checkGLError();
    glDeleteTextures(1, &outputTextureID);
    checkGLError();
    
    glfwDestroyWindow(window);
    glfwTerminate();
    
    return 0;
}

GLuint createComputeShader() {
    GLuint computeShader = glCreateShader(GL_COMPUTE_SHADER);
    checkGLError();
    glShaderSource(computeShader, 1, &computeShaderSource, NULL);
    checkGLError();
    glCompileShader(computeShader);
    checkGLError();
    CompileStatus(computeShader);
    
    GLuint program = glCreateProgram();
    checkGLError();
    glAttachShader(program, computeShader);
    checkGLError();
    glLinkProgram(program);
    checkGLError();
    LinkStatus(program);
    
    glDeleteShader(computeShader);
    return program;
}

void createTexture(GLuint *textureID, int width, int height) {
    glGenTextures(1, textureID);
    checkGLError();
    glBindTexture(GL_TEXTURE_2D, *textureID);
    checkGLError();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    checkGLError();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    checkGLError();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    checkGLError();
    glBindTexture(GL_TEXTURE_2D, 0);
    checkGLError();
}

void createInputTexture(GLuint *inputTextureID, int width, int height, const float* data) {
    glGenTextures(1, inputTextureID);
    checkGLError();
    glBindTexture(GL_TEXTURE_2D, *inputTextureID);
    checkGLError();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, data);
    checkGLError();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    checkGLError();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    checkGLError();
    glBindTexture(GL_TEXTURE_2D, 0);
    checkGLError();
}

GLuint createRenderShaders() {    
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    checkGLError();
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    checkGLError();
    glCompileShader(vertexShader);
    checkGLError();
    CompileStatus(vertexShader);
    
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    checkGLError();
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    checkGLError();
    glCompileShader(fragmentShader);
    checkGLError();
    CompileStatus(fragmentShader);
    
    GLuint program = glCreateProgram();
    checkGLError();
    glAttachShader(program, vertexShader);
    checkGLError();
    glAttachShader(program, fragmentShader);
    checkGLError();
    glLinkProgram(program);
    checkGLError();
    LinkStatus(program);

    glDeleteShader(vertexShader);
    checkGLError();
    glDeleteShader(fragmentShader);
    checkGLError();
    return program;
}

void render(GLuint textureID, GLuint renderProgram) {
    static const float vertices[] = {
        // positions   // texcoords
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f,
        -1.0f,  1.0f,  0.0f, 1.0f
    };

    static const unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    static GLuint VAO = 0, VBO = 0, EBO = 0;
    if (VAO == 0) {
        glGenVertexArrays(1, &VAO);
        checkGLError();
        glGenBuffers(1, &VBO);
        checkGLError();
        glGenBuffers(1, &EBO);
        checkGLError();

        glBindVertexArray(VAO);
        checkGLError();

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        checkGLError();
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        checkGLError();

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        checkGLError();
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
        checkGLError();

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        checkGLError();
        glEnableVertexAttribArray(0);
        checkGLError();

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        checkGLError();
        glEnableVertexAttribArray(1);
        checkGLError();

        glBindVertexArray(0);
        checkGLError();
    }

    glClear(GL_COLOR_BUFFER_BIT);
    checkGLError();

    glUseProgram(renderProgram);
    checkGLError();
    glBindVertexArray(VAO);
    checkGLError();

    glActiveTexture(GL_TEXTURE0);
    checkGLError();
    glBindTexture(GL_TEXTURE_2D, textureID);
    checkGLError();

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    checkGLError();
}