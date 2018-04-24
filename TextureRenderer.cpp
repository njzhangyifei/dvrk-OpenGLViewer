//
// Created by Yifei on 4/20/2018.
//

#include <memory>
#include "TextureRenderer.h"
#include "shader.h"

#ifdef _WIN32
#include <windows.h>
#endif
#include <vtk_glew.h>
#include <glm/glm.hpp>
#include <iostream>

#define SHADOW_MAP_QUAD_V_SHADER_PATH "./debug_shadow_map.vert"
#define SHADOW_MAP_QUAD_F_SHADER_PATH "./debug_shadow_map.frag"
GLuint TextureRenderer::shaderProgram = 0;

TextureRenderer::TextureRenderer() {
    glm::vec2 top_left = glm::vec2({-1, 1});
    glm::vec2 bottom_right = glm::vec2({1, -1});
    float tl_x = top_left.x;
    float tl_y = top_left.y;
    float br_x = bottom_right.x;
    float br_y = bottom_right.y;
    quadVertices = new GLfloat[20]{
            // positions        // texture Coords
            tl_x, br_y, 0.0f, 0.0f, 1.0f,
            br_x, br_y, 0.0f, 1.0f, 1.0f,
            tl_x, tl_y, 0.0f, 0.0f, 0.0f,
            br_x, tl_y, 0.0f, 1.0f, 0.0f,
    };
    if (!shaderProgram)
        shaderProgram = LoadShaders(SHADOW_MAP_QUAD_V_SHADER_PATH, SHADOW_MAP_QUAD_F_SHADER_PATH);
}

void TextureRenderer::setup(StereoWindow * stereoWindow, GLFWwindow *context, bool is_left) {
    if (is_left) {
        glGenVertexArrays(1, &VAO_left);
        glBindVertexArray(VAO_left);
    } else {
        glGenVertexArrays(1, &VAO_right);
        glBindVertexArray(VAO_right);
    }

    if (is_left) {
        glGenBuffers(1, &VBO);
        load_data();
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void *) 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void *) (3 * sizeof(float)));
}

void TextureRenderer::load_data() {
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 20 * sizeof(GLfloat), quadVertices, GL_STATIC_DRAW);
}

void TextureRenderer::execute(StereoWindow * stereoWindow, GLFWwindow *context, bool is_left) {
    glUseProgram(shaderProgram);
    glActiveTexture(GL_TEXTURE0);
    if (is_left) {
        glBindTexture(GL_TEXTURE_2D, texture_id_left);
    } else {
        glBindTexture(GL_TEXTURE_2D, texture_id_right);
    }
    glUniform1i(glGetUniformLocation(shaderProgram, "texture_sampler"), 0);
    if (is_left) {
        glBindVertexArray(VAO_left);
    } else {
        glBindVertexArray(VAO_right);
    }
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
    glClear(GL_DEPTH_BUFFER_BIT);
}

void TextureRenderer::teardown(StereoWindow *stereoWindow, GLFWwindow *context, bool is_left) {
    if (is_left) {
        glDeleteProgram(shaderProgram);
        shaderProgram = 0;
        glDeleteBuffers(1, &VBO);
    }

    if (is_left) {
        glDeleteVertexArrays(1, &VAO_left);
    } else {
        glDeleteVertexArrays(1, &VAO_right);
    }
}

