#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

#define numVAOs 1

GLuint renderingProgram;
GLuint vao[numVAOs];

/**
 * Creates a shader program that positions vertices at the clip-space origin
 * and outputs opaque blue fragments. Requires a current OpenGL context and
 * initialized GLEW. Shader and program objects are left allocated.
 * @return The program handle; compilation and linking failures are not checked,
 * so the handle does not guarantee a usable program.
 */
GLuint createShaderProgram() {
    const char *vshaderSource = "#version 430    \n"
                                "void main(void) \n"
                                "{ gl_Position = vec4(0.0, 0.0, 0.0, 1.0); }";

    const char *fshaderSource = "#version 430    \n"
                                "out vec4 color; \n"
                                "void main(void) \n"
                                "{ color = vec4(0.0, 0.0, 1.0, 1.0); }";

    GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
    GLuint vfprogram = glCreateProgram();

    glShaderSource(vShader, 1, &vshaderSource, NULL);
    glShaderSource(fShader, 1, &fshaderSource, NULL);
    glCompileShader(vShader);
    glCompileShader(fShader);

    glAttachShader(vfprogram, vShader);
    glAttachShader(vfprogram, fShader);
    glLinkProgram(vfprogram);

    return vfprogram;
}

/**
 * Initializes the global rendering program and creates and binds a vertex array
 * in the current OpenGL context. Requires initialized GLEW.
 * @param window Unused; initialization uses the current context.
 */
void init(GLFWwindow *window) {
    renderingProgram = createShaderProgram();
    glGenVertexArrays(numVAOs, vao);
    glBindVertexArray(vao[0]);
}

/**
 * Clears the current color buffer to opaque black and draws a blue point at the
 * center, requesting a size of 30 pixels. Requires init() in the current context;
 * buffer swapping is left to the caller.
 * @param window Unused; rendering uses the current context.
 * @param currentTime Unused; rendering does not vary with time.
 */
void display(GLFWwindow *window, double currentTime) {
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(renderingProgram);
    glPointSize(30.0f);
    glDrawArrays(GL_POINTS, 0, 1);
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
        display(window, glfwGetTime());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
