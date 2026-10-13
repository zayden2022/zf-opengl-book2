#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <iostream>

#include "utils.h"

#define numVAOs 1

static GLuint renderingProgram;
static GLuint vao[numVAOs];

static void processInput_basic(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
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

/**
 * Initializes the global rendering program and creates and binds a vertex array
 * in the current OpenGL context. Requires initialized GLEW.
 * @param window Unused; initialization uses the current context.
 */
static void init(GLFWwindow *window) {
    renderingProgram = createShaderProgram();
    glGenVertexArrays(numVAOs, vao);
    glBindVertexArray(vao[0]);
}

static void display(GLFWwindow *window, double currentTime) {
    glClear(GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    static float x = 0.0f;
    static float inc = 0.01f;
    x += inc;
    if (x > 1.0f) {
        inc = -0.01f;
    }
    if (x < -1.0f) {
        inc = 0.01f;
    }

    glUseProgram(renderingProgram);
    // glPointSize(30.0f);
    // glDrawArrays(GL_POINTS, 0, 1);

    GLuint offsetLoc = glGetUniformLocation(renderingProgram, "offset");
    // glGetUniformLocation — Returns the location of a uniform variable
    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glGetUniformLocation.xhtml
    glProgramUniform1f(renderingProgram, offsetLoc, x);
    // glProgramUniform — Specify the value of a uniform variable for a specified program object
    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glProgramUniform.xhtml

    glDrawArrays(GL_TRIANGLES, 0, 3);
    // glDrawArrays — render primitives from array data
    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glDrawArrays.xhtml
}

static void error_callback(int error, const char *description) {
    std::cerr << "glfw error: " << description << std::endl;
}

int main(void) {
    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        exit(EXIT_FAILURE);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    GLFWwindow *window = glfwCreateWindow(600, 600, "Chapter 2 - program 1", NULL, NULL);
    glfwMakeContextCurrent(window);
    GLenum err = glewInit();
    if (err != GLEW_OK && err != GLEW_ERROR_NO_GLX_DISPLAY) {
        std::cerr << "Failed to initialize GLEW: " << glewGetErrorString(err) << std::endl;
        return -1;
    }
    glfwSwapInterval(1);

    init(window);

    while (!glfwWindowShouldClose(window)) {
        processInput_basic(window);
        display(window, glfwGetTime());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
