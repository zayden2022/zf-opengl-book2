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

GLuint createShaderProgram(const char *vertShaderPath, const char *fragShaderPath) {
    GLint vertCompiled;
    GLint fragCompiled;
    GLint linked;

    std::string vertShaderStr;
    std::string fragShaderStr;
    try {
        vertShaderStr = readShaderSource(vertShaderPath);
        fragShaderStr = readShaderSource(fragShaderPath);
    } catch (const std::exception &e) {
        std::cerr << "Error reading shader: " << e.what() << std::endl;
        return 0;
    }
    GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);

    const char *vertShaderSrc = vertShaderStr.c_str();
    const char *fragShaderSrc = fragShaderStr.c_str();

    glShaderSource(vShader, 1, &vertShaderSrc, NULL);
    glCompileShader(vShader);
    checkOpenGLError();
    glGetShaderiv(vShader, GL_COMPILE_STATUS, &vertCompiled);
    if (vertCompiled != GL_TRUE) {
        std::cout << "vertex compilation failed" << std::endl;
        printShaderLog(vShader);
    }

    glShaderSource(fShader, 1, &fragShaderSrc, NULL);
    glCompileShader(fShader);
    checkOpenGLError();
    glGetShaderiv(fShader, GL_COMPILE_STATUS, &fragCompiled);
    if (fragCompiled != GL_TRUE) {
        std::cout << "fragment compilation failed " << std::endl;
        printShaderLog(fShader);
    }

    if (vertCompiled != GL_TRUE || fragCompiled != GL_TRUE) {
        glDeleteShader(fShader);
        glDeleteShader(vShader);
        return 0;
    }

    GLuint vfprogram = glCreateProgram();
    glAttachShader(vfprogram, vShader);
    glAttachShader(vfprogram, fShader);
    glLinkProgram(vfprogram);
    checkOpenGLError();
    glGetProgramiv(vfprogram, GL_LINK_STATUS, &linked);
    if (linked != GL_TRUE) {
        std::cout << "linking failed" << std::endl;
        printProgramLog(vfprogram);
        glDeleteProgram(vfprogram);
        vfprogram = 0;
    }

    // detaching the shaders after linking is not advised (may cause problems on some
    // mobile platforms), but deleting is a good practice.

    glDeleteShader(vShader);
    glDeleteShader(fShader);

    return vfprogram;
}
