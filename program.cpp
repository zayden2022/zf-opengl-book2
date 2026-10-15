#include "program.h"

#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp>         // glm::value_ptr
#include <string>

#include "utils.h"

#define numVAOs 1
#define numVBOs 2

static bool InstancingFlag = true;

float cameraX, cameraY, cameraZ;
float cubeLocX, cubeLocY, cubeLocZ;
GLuint renderingProgram;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];

// variable allocation for display
GLuint mvLoc, pLoc;
int width, height;
float aspect;
glm::mat4 pMat, vMat, mMat, mvMat;

static void setupVertices(void) {
    float vertexPositions[108] = {
        -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f,
        1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,
        1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  -1.0f,
        1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,
        -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f,
        -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,
        -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f,
        1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f};

    glGenVertexArrays(1, vao);
    glBindVertexArray(vao[0]);
    glGenBuffers(numVBOs, vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_STATIC_DRAW);
}

void init(GLFWwindow *window) {
    if (InstancingFlag) {
        renderingProgram =
            createShaderProgram("ch04/vertShader_instancing.glsl", "ch04/fragShader.glsl");
    } else {
        renderingProgram = createShaderProgram("ch04/vertShader.glsl", "ch04/fragShader.glsl");
    }
    cameraX = 0.0f;
    cameraY = 0.0f;
    cameraZ = 40.0f;
    cubeLocX = 0.0f;
    cubeLocY = -2.0f;
    cubeLocZ = 0.0f;
    setupVertices();
}

void display(GLFWwindow *window, double currentTime) {
    glClear(GL_DEPTH_BUFFER_BIT);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(renderingProgram);

    mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");
    pLoc = glGetUniformLocation(renderingProgram, "p_matrix");

    glfwGetFramebufferSize(window, &width, &height);
    aspect = (float)width / (float)height;
    pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);

    vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, -cameraY, -cameraZ));

    if (InstancingFlag) {
        glClear(GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(renderingProgram);

        GLuint vLoc = glGetUniformLocation(renderingProgram, "v_matrix");
        pLoc = glGetUniformLocation(renderingProgram, "p_matrix");

        vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, -cameraY, -cameraZ));

        glUniformMatrix4fv(vLoc, 1, GL_FALSE, glm::value_ptr(vMat));
        glUniformMatrix4fv(pLoc, 1, GL_FALSE, glm::value_ptr(pMat));

        float timeFactor = ((float)currentTime);
        GLuint tfLoc = glGetUniformLocation(renderingProgram, "tf");
        glUniform1f(tfLoc, (float)timeFactor);

        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
        glEnableVertexAttribArray(0);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        // glDrawArraysInstanced(GL_TRIANGLES, 0, 36, 24);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 36, 100000);
    } else {
        for (int i = 0; i < 48; i++) {
            float tf = currentTime + i; // time factor

            // mMat = glm::translate(glm::mat4(1.0f), glm::vec3(cubeLocX, cubeLocY, cubeLocZ));
            {
                // use current time to compute different translations in x, y, and z
                glm::mat4 tMat = glm::translate(glm::mat4(1.0f), glm::vec3(sin(0.35f * tf) * 8.0f,
                                                                           cos(0.52f * tf) * 8.0f,
                                                                           sin(0.7f * tf) * 8.0f));
                glm::mat4 rMat =
                    glm::rotate(glm::mat4(1.0f), 1.75f * (float)tf, glm::vec3(0.0f, 1.0f, 0.0f));
                rMat = glm::rotate(rMat, 1.75f * (float)tf, glm::vec3(1.0f, 0.0f, 0.0f));
                rMat = glm::rotate(rMat, 1.75f * (float)tf, glm::vec3(0.0f, 0.0f, 1.0f));
                // the 1.75 adjusts the rotation speed

                mMat = tMat * rMat;
            }

            mvMat = vMat * mMat;

            glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
            glUniformMatrix4fv(pLoc, 1, GL_FALSE, glm::value_ptr(pMat));

            glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
            glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
            glEnableVertexAttribArray(0);

            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LEQUAL);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }
}
