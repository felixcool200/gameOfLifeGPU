#include <GL/glew.h>
#include <GLFW/glfw3.h>

#ifndef DEBUG_H
#define DEBUG_H
void checkGLError();
void CompileStatus(GLuint shader);
void LinkStatus(GLuint program);
#endif