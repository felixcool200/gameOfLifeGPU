#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdlib.h>

#if DEVELOPMENT == false
void checkGLError() {}
void printShaderSource(GLuint shader) {}
void CompileStatus(GLuint shader){}
void LinkStatus(GLuint program){}

#else

void checkGLError() {
    GLenum err;
    while((err = glGetError()) != GL_NO_ERROR) {
        printf("OpenGL error: %d\n", err);
    }
}

void printShaderSource(GLuint shader) {
    GLint length;
    GLchar *source;

    // Get the length of the shader source
    glGetShaderiv(shader, GL_SHADER_SOURCE_LENGTH, &length);

    if (length > 0) {
        // Allocate memory to store the shader source
        source = (GLchar *)malloc(length * sizeof(GLchar));
        
        if (source == NULL) {
            fprintf(stderr, "Unable to allocate memory for shader source\n");
            return;
        }

        // Get the shader source
        glGetShaderSource(shader, length, NULL, source);

        // Print the shader source
        printf("Shader Source:\n%s\n", source);

        // Free the allocated memory
        free(source);
    } else {
        printf("Shader source is empty or shader does not exist.\n");
    }
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
        printShaderSource(shader);
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
        printShaderSource(shader);
    }
}
#endif