#pragma once

#include <GL/glew.h>
#include <string>

std::string readShaderSource(const char *filePath);
void printShaderLog(GLuint shader);
void printProgramLog(int prog);
bool checkOpenGLError();
