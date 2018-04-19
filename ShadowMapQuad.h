//
// Created by yifei on 3/18/18.
//

#ifndef PA4_SHADOWMAPQUAD_H
#define PA4_SHADOWMAPQUAD_H

#ifdef _WIN32
#include <windows.h>
#endif
#include <vtk_glew.h>
#include <glm/glm.hpp>

class ShadowMapQuad {
public:
//    ShadowMapQuad();

    static GLuint shaderProgram;
    ShadowMapQuad(glm::vec2 top_left, glm::vec2 bottom_right);

    void draw(GLuint);

    void load_data();

protected:
    GLuint VBO, VAO;
    GLfloat * quadVertices;

};


#endif //PA4_SHADOWMAPQUAD_H
