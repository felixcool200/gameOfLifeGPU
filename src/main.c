#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <time.h> 

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define STRINGIZE(x) #x
#define EXPAND(x) STRINGIZE(x)


#define WIDTH 800
#define WIDTH_STR EXPAND(WIDTH)

#define HEIGHT 600
#define HEIGHT_STR EXPAND(HEIGHT)

#define NUM_X 4
#define NUM_X_STR EXPAND(NUM_X)

#define NUM_Y 4
#define NUM_Y_STR EXPAND(NUM_Y)

GLFWwindow* window;

GLuint computeProgram;
GLuint drawProgram;
GLuint vao;
GLuint tex[2];
GLuint ssbo;

const char* computeShaderSource =
    "#version 430\n"
    "layout(local_size_x = 10, local_size_y = 10) in;\n"
    "layout(std430, binding = 0) buffer InputBuffer {\n"
    "    int state[];\n"
    "};\n"
    "void main() {\n"
    "    uint x = gl_GlobalInvocationID.x;\n"
    "    uint y = gl_GlobalInvocationID.y;\n"
    "    int neighbors = 0;\n"
    "    for (int dx = -1; dx <= 1; dx++) {\n"
    "        for (int dy = -1; dy <= 1; dy++) {\n"
    "            if (dx != 0 || dy != 0) {\n"
    "                int nx = int(x) + dx;\n"
    "                int ny = int(y) + dy;\n"
    "                if (nx >= 0 && nx < " NUM_X_STR " && ny >= 0 && ny < " NUM_Y_STR ") {\n"
    "                    neighbors += state[ny *" NUM_X_STR " + nx];\n"
    "                }\n"
    "            }\n"
    "        }\n"
    "    }\n"
    "    if (neighbors == 3 || (state[y * " NUM_X_STR " + x] == 1 && neighbors == 2)) {\n"
    "        state[y * " NUM_X_STR " + x] = 1;\n"
    "    } else {\n"
    "        state[y * " NUM_X_STR " + x] = 0;\n"
    "    }\n"
    "}\n";

const char* drawShaderSource =
    "#version 330 core\n"
    "out vec4 fragColor;\n"
    "uniform sampler2D tex;\n"
    "void main() {\n"
    "    fragColor = texture(tex, gl_FragCoord.xy / vec2(" WIDTH_STR ", " HEIGHT_STR "));\n"
    "}\n";

void error_callback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void CompileStatus(GLuint shader)
{
    GLint status = GL_TRUE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        GLint logLen;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);
        char* log = (char*)malloc(logLen);
        if (log == NULL)
        {
            fprintf(stderr, "Failed to allocate memory for shader log\n");
            return;
        }
        GLsizei written;
        glGetShaderInfoLog(shader, logLen, &written, log);
        printf("Compile error:\n%s\n", log);
        free(log);
    }
}

void LinkStatus(GLuint program)
{
    GLint status = GL_TRUE;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
    {
        GLint logLen;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLen);
        char* log = (char*)malloc(logLen);
        if (log == NULL)
        {
            fprintf(stderr, "Failed to allocate memory for program log\n");
            return;
        }
        GLsizei written;
        glGetProgramInfoLog(program, logLen, &written, log);
        printf("Link error:\n%s\n", log);
        free(log);
    }
}

void init() {
    // Initialize GLFW
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        exit(EXIT_FAILURE);
    }

    glfwSetErrorCallback(error_callback);

    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(WIDTH, HEIGHT, "Conway's Game of Life", NULL, NULL);
    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        exit(EXIT_FAILURE);
    }

    // Set the required callback functions
    glfwSetKeyCallback(window, key_callback);

    // Create compute shader program
    GLuint computeShader = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(computeShader, 1, &computeShaderSource, NULL);
    glCompileShader(computeShader);
    CompileStatus(computeShader);

    computeProgram = glCreateProgram();
    glAttachShader(computeProgram, computeShader);
    glLinkProgram(computeProgram);
    LinkStatus(computeProgram);

    // Create draw shader program
    GLuint drawShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(drawShader, 1, &drawShaderSource, NULL);
    glCompileShader(drawShader);
    CompileStatus(drawShader);

    drawProgram = glCreateProgram();
    glAttachShader(drawProgram, drawShader);
    glLinkProgram(drawProgram);
    LinkStatus(drawProgram);

    // Create texture
    glGenTextures(2, tex);
    glBindTexture(GL_TEXTURE_2D, tex[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, NUM_X, NUM_Y, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
    glBindImageTexture(0, tex[0], 0, GL_FALSE, 0, GL_READ_WRITE, GL_R8);
    glBindTexture(GL_TEXTURE_2D, tex[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, NUM_X, NUM_Y, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);

    // Create SSBO
    glGenBuffers(1, &ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, NUM_X * NUM_Y * sizeof(int), NULL, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);

    // Initialize state buffer
    int* state = (int*)malloc(NUM_X * NUM_Y * sizeof(int));
    for (int i = 0; i < NUM_X * NUM_Y; ++i) {
        state[i] = rand() % 2;
        printf("%d ", state[i]);
    }
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, NUM_X * NUM_Y * sizeof(int), state);
    free(state);

    // Create vertex array object
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glViewport(0, 0, WIDTH, HEIGHT);
}

void update() {
    glUseProgram(computeProgram);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);
    glDispatchCompute(NUM_X / 10, NUM_Y / 10, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    // Swap textures
    GLuint tmp = tex[0];
    tex[0] = tex[1];
    tex[1] = tmp;
    glBindImageTexture(0, tex[0], 0, GL_FALSE, 0, GL_READ_WRITE, GL_R8);
}

void draw() {
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(drawProgram);
    glUniform1i(glGetUniformLocation(drawProgram, "tex"), 0);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glfwSwapBuffers(window);
    glfwPollEvents();
}

int main() {
    init();

    while (!glfwWindowShouldClose(window)) {
        update();
        draw();
        //usleep(10);
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}