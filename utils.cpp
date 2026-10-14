#include "utils.h"

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>

std::string readShaderSource(const char *filePath) {
    std::ifstream fileStream(filePath);
    if (!fileStream) {
        throw std::runtime_error(std::string("Failed to open: ") + filePath);
    }

    std::string content;
    std::string line;

    while (std::getline(fileStream, line)) {
        content += line;
        content += '\n';
    }

    return content;
}

void printShaderLog(GLuint shader) {
    GLint len = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);

    if (len <= 1) {
        return;
    }

    std::vector<char> log(static_cast<std::size_t>(len));
    GLsizei charsWritten = 0;
    glGetShaderInfoLog(shader, static_cast<GLsizei>(log.size()), &charsWritten, log.data());
    std::cerr << "Shader Info Log: " << log.data() << '\n';
}

void printProgramLog(GLuint program) {
    GLint len = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);

    if (len <= 1) {
        return;
    }

    std::vector<char> log(static_cast<std::size_t>(len));
    GLsizei charsWritten = 0;
    glGetProgramInfoLog(program, static_cast<GLsizei>(log.size()), &charsWritten, log.data());
    std::cerr << "Program Info Log: " << log.data() << '\n';
}

bool checkOpenGLError() {
    bool foundError = false;
    int glErr = glGetError();
    // glGetError — return error information
    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glGetError.xhtml
    while (glErr != GL_NO_ERROR) {
        std::cerr << "OpenGL error: 0x" << std::hex << glErr << std::dec << '\n';
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
        std::cerr << "vertex compilation failed" << std::endl;
        printShaderLog(vShader);
    }

    glShaderSource(fShader, 1, &fragShaderSrc, NULL);
    glCompileShader(fShader);
    checkOpenGLError();
    glGetShaderiv(fShader, GL_COMPILE_STATUS, &fragCompiled);
    if (fragCompiled != GL_TRUE) {
        std::cerr << "fragment compilation failed " << std::endl;
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
        std::cerr << "linking failed" << std::endl;
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
