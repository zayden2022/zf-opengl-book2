#include "utils.h"

#include <fstream>
#include <iostream>

std::string readShaderSource(const char *filePath) {
    std::string content;
    std::ifstream fileStream(filePath, std::ios::in);
    if (!fileStream)
        throw std::runtime_error(std::string("Failed to open: ") + filePath);
    std::string line = "";
    while (!fileStream.eof()) {
        getline(fileStream, line);
        content.append(line + "\n");
    }
    fileStream.close();
    return content;
}

void printShaderLog(GLuint shader) {
    int len = 0;
    int chWrittn = 0;
    char *log;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
    // glGetShaderiv — Returns a parameter from a shader object
    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glGetShader.xhtml
    if (len > 0) {
        log = (char *)malloc(len);
        glGetShaderInfoLog(shader, len, &chWrittn, log);
        // glGetShaderInfoLog — Returns the information log for a shader object
        // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glGetShaderInfoLog.xhtml
        std::cout << "Shader Info Log: " << log << std::endl;
        free(log);
    }
}

void printProgramLog(int prog) {
    int len = 0;
    int chWrittn = 0;
    char *log;
    glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &len);
    if (len > 0) {
        log = (char *)malloc(len);
        glGetProgramInfoLog(prog, len, &chWrittn, log);
        std::cout << "Program Info Log: " << log << std::endl;
        free(log);
    }
}

bool checkOpenGLError() {
    bool foundError = false;
    int glErr = glGetError();
    // glGetError — return error information
    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glGetError.xhtml
    while (glErr != GL_NO_ERROR) {
        std::cout << "glError: " << glErr << std::endl;
        foundError = true;
        glErr = glGetError();
    }
    return foundError;
}

/**
 * Creates a shader program that positions vertices at the clip-space origin
 * and outputs opaque blue fragments. Requires a current OpenGL context and
 * initialized GLEW. Shader and program objects are left allocated.
 * @return The program handle.
 */
static GLuint createShaderProgram() {
    GLint vertCompiled;
    GLint fragCompiled;
    GLint linked;

    GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
    GLuint vfprogram = glCreateProgram();

    std::string vertShaderStr = readShaderSource("ch02/vertShader.glsl");
    std::string fragShaderStr = readShaderSource("ch02/fragShader.glsl");
    const char *vertShaderSrc = vertShaderStr.c_str();
    const char *fragShaderSrc = fragShaderStr.c_str();

    glShaderSource(vShader, 1, &vertShaderSrc, NULL);
    glShaderSource(fShader, 1, &fragShaderSrc, NULL);
    glCompileShader(vShader);
    checkOpenGLError();
    glGetShaderiv(vShader, GL_COMPILE_STATUS, &vertCompiled);
    if (vertCompiled != 1) {
        std::cout << "vertex compilation failed" << std::endl;
        printShaderLog(vShader);
    }

    glCompileShader(fShader);
    checkOpenGLError();
    glGetShaderiv(fShader, GL_COMPILE_STATUS, &fragCompiled);
    if (fragCompiled != 1) {
        std::cout << "fragment compilation failed " << std::endl;
        printShaderLog(fShader);
    }

    glAttachShader(vfprogram, vShader);
    glAttachShader(vfprogram, fShader);

    glLinkProgram(vfprogram);
    checkOpenGLError();
    glGetProgramiv(vfprogram, GL_LINK_STATUS, &linked);
    if (linked != 1) {
        std::cout << "linking failed" << std::endl;
        printProgramLog(vfprogram);
    }
    return vfprogram;
}
