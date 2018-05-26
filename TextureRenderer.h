//
// Created by Yifei on 4/20/2018.
//

#ifndef OPENGLVIEWER_CAMERARENDERER_H
#define OPENGLVIEWER_CAMERARENDERER_H

#include "IRenderProcedure.h"
#include <glm/glm.hpp>

class StereoWindow;

class TextureRenderer : public IRenderProcedure {

protected:
    GLuint VBO;
    GLuint VAO_left;
    GLuint VAO_right;
    GLfloat * quadVertices;
    static GLuint shaderProgram;
    bool use_depth;
    bool use_distortion;

    std::string vertex_shader;
    std::string fragment_shader;

    void load_data();

public:
    TextureRenderer(bool depth, bool distortion);
    TextureRenderer();
    void execute(StereoWindow * stereoWindow, GLFWwindow * context, bool is_left) override;
    void setup(StereoWindow * stereoWindow, GLFWwindow * context, bool is_left) override;
    void teardown(StereoWindow * stereoWindow, GLFWwindow * context, bool is_left) override;

    GLuint texture_id_left;
    GLuint texture_id_right;

    GLuint texture_depth_id_left;
    GLuint texture_depth_id_right;

    glm::vec3 radial_distortion;
    glm::vec2 tangential_distortion;
    glm::vec4 camera_center_focus;
    glm::vec2 image_size;
    float texture_scale_height;
    float texture_scale_width;
};


#endif //OPENGLVIEWER_CAMERARENDERER_H
