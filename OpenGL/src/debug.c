#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdlib.h>

void checkGLError() {
    GLenum err;
    while((err = glGetError()) != GL_NO_ERROR) {
        printf("OpenGL error: %d\n", err);
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