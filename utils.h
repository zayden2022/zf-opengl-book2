#pragma once

#include <GL/glew.h>
#include <string>

std::string readShaderSource(const char *filePath);
void printShaderLog(GLuint shader);
void printProgramLog(int prog);
bool checkOpenGLError();

/**
 * Creates a shader program from a vertex and fragment shaders.
 * @return The program handle or 0 in case of an error.
 */
GLuint createShaderProgram(const char *vertShaderPath, const char *fragShaderPath);
