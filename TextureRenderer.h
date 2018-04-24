//
// Created by Yifei on 4/20/2018.
//

#ifndef OPENGLVIEWER_CAMERARENDERER_H
#define OPENGLVIEWER_CAMERARENDERER_H

#include "IRenderProcedure.h"

class StereoWindow;

class TextureRenderer : public IRenderProcedure {

protected:
    GLuint VBO;
    GLuint VAO_left;
    GLuint VAO_right;
    GLfloat * quadVertices;
    static GLuint shaderProgram;
    void load_data();

public:
    TextureRenderer();
    void execute(StereoWindow * stereoWindow, GLFWwindow * context, bool is_left) override;
    void setup(StereoWindow * stereoWindow, GLFWwindow * context, bool is_left) override;
    void teardown(StereoWindow * stereoWindow, GLFWwindow * context, bool is_left) override;

    GLuint texture_id_left;
    GLuint texture_id_right;
};


#endif //OPENGLVIEWER_CAMERARENDERER_H
