//
// Created by Yifei on 4/20/2018.
//

#include <memory>
#include "TextureRenderer.h"
#include "shader.h"
#include "get_cwd.h"
#include "VTKCameraManager.h"

#ifdef _WIN32
#include <windows.h>
#endif
#include <vtk_glew.h>
#include <glm/glm.hpp>
#include <iostream>

#define TEXTURE_MAP_DEPTH_V_SHADER_PATH "/texture_map_depth.vert"
#define TEXTURE_MAP_DEPTH_F_SHADER_PATH "/texture_map_depth.frag"

GLuint TextureRenderer::shaderProgram = 0;

TextureRenderer::TextureRenderer() : TextureRenderer(false, false){
}

TextureRenderer::TextureRenderer(bool depth, bool distortion) {
    glm::vec2 top_left = glm::vec2({-1, 1});
    glm::vec2 bottom_right = glm::vec2({1, -1});
    float tl_x = top_left.x;
    float tl_y = top_left.y;
    float br_x = bottom_right.x;
    float br_y = bottom_right.y;
    quadVertices = new GLfloat[20]{
            // positions        // texture Coords
            tl_x, br_y, 1.0f, 0.0f, 1.0f,
            br_x, br_y, 1.0f, 1.0f, 1.0f,
            tl_x, tl_y, 1.0f, 0.0f, 0.0f,
            br_x, tl_y, 1.0f, 1.0f, 0.0f,
    };
    this->texture_scale_height = 1.0f;
    this->texture_scale_width = 1.0f;
    this->use_depth      = depth;
    this->use_distortion = distortion;
    vertex_shader = get_cwd() + std::string(TEXTURE_MAP_DEPTH_V_SHADER_PATH);
    fragment_shader = get_cwd() + std::string(TEXTURE_MAP_DEPTH_F_SHADER_PATH);
    if (!shaderProgram) {
        shaderProgram = LoadShaders(vertex_shader.c_str(), fragment_shader.c_str());
    }
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
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

}

void TextureRenderer::load_data() {
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 20 * sizeof(GLfloat), quadVertices, GL_STATIC_DRAW);
}

void TextureRenderer::execute(StereoWindow * stereoWindow, GLFWwindow *context, bool is_left) {
    glEnable(GL_DEPTH_TEST); // depth buffer fighting between the cone and the backround without this
    if (use_depth) {
        glDepthFunc(GL_LEQUAL);
    } else {
        glDepthFunc(GL_ALWAYS);
    }
    glUseProgram(shaderProgram);
    glActiveTexture(GL_TEXTURE0);
    if (is_left) {
        glBindTexture(GL_TEXTURE_2D, texture_id_left);
    } else {
        glBindTexture(GL_TEXTURE_2D, texture_id_right);
    }
    if (use_depth) {
        glActiveTexture(GL_TEXTURE1);
        if (is_left) {
            glBindTexture(GL_TEXTURE_2D, texture_depth_id_left);
        } else {
            glBindTexture(GL_TEXTURE_2D, texture_depth_id_right);
        }
    }
    glUniform1i(glGetUniformLocation(shaderProgram, "use_depth"),             (this->use_depth) ? 1 : 0);
    glUniform1i(glGetUniformLocation(shaderProgram, "use_distortion"),        (this->use_distortion) ? 1 : 0);
    glUniform1i(glGetUniformLocation(shaderProgram, "use_transparency"),      (this->use_transparency) ? 1 : 0);
    glUniform1f(glGetUniformLocation(shaderProgram, "texture_scale_height"),  texture_scale_height);
    glUniform1f(glGetUniformLocation(shaderProgram, "texture_scale_width"),   texture_scale_width);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture_color_sampler"), 0);
    if (use_depth) {
        glUniform1i(glGetUniformLocation(shaderProgram, "texture_depth_sampler"), 1);
    }
    if (use_distortion){
        glUniform2fv(glGetUniformLocation(shaderProgram, "image_size"), 1, &image_size[0]);
        glActiveTexture(GL_TEXTURE2);
        if (is_left) {
            glBindTexture(GL_TEXTURE_2D, distortion_texture_left);
        } else {
            glBindTexture(GL_TEXTURE_2D, distortion_texture_right);
        }
        glUniform1i(glGetUniformLocation(shaderProgram,  "texture_distortion_sampler"), 2);
    }
    if (use_transparency){
        glUniform1f(glGetUniformLocation(shaderProgram, "transparency"), transparency);
    }
    if (is_left) {
        glBindVertexArray(VAO_left);
    } else {
        glBindVertexArray(VAO_right);
    }
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    if (use_depth) {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    if (use_distortion){
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    glActiveTexture(GL_TEXTURE0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glUseProgram(0);
    glDisable(GL_DEPTH_TEST); // depth buffer fighting between the cone and the backround without this
}

void TextureRenderer::teardown(StereoWindow *stereoWindow, GLFWwindow *context, bool is_left) {
    if (is_left) {
        if (shaderProgram){
            glDeleteProgram(shaderProgram);
            shaderProgram = 0;
        }
    }

    if (is_left) {
        glDeleteVertexArrays(1, &VAO_left);
    } else {
        glDeleteVertexArrays(1, &VAO_right);
    }
}


