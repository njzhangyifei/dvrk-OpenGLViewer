//
// Created by yifei on 3/18/18.
//

#include <glm/glm.hpp>
#include "ShadowMapQuad.h"

GLuint ShadowMapQuad::shaderProgram = 0;

ShadowMapQuad::ShadowMapQuad(
        glm::vec2 top_left,
        glm::vec2 bottom_right
) {
    float tl_x = top_left.x;
    float tl_y = top_left.y;
    float br_x = bottom_right.x;
    float br_y = bottom_right.y;
    quadVertices = new GLfloat[20]{
            // positions        // texture Coords
            tl_x, br_y, 0.0f, 0.0f, 0.0f,
            tl_x, tl_y, 0.0f, 0.0f, 1.0f,
            br_x, br_y, 0.0f, 1.0f, 0.0f,
            br_x, tl_y, 0.0f, 1.0f, 1.0f,
    };
    load_data();
}

void ShadowMapQuad::load_data(){
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 20 * sizeof(GLfloat), quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void *) 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void *) (3 * sizeof(float)));
}

void ShadowMapQuad::draw(GLuint texture_id) {
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}