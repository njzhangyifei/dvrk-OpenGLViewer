//
// Created by Yifei on 4/20/2018.
//

#ifndef OPENGLVIEWER_IRENDERPROCEDURE_H
#define OPENGLVIEWER_IRENDERPROCEDURE_H

#include <vtk_glew.h>
#include <GLFW/glfw3.h>
#include "StereoWindow.h"

class IRenderProcedure {
public:
    virtual void execute(StereoWindow * stereoWindow, GLFWwindow * context, bool is_left){};
    virtual void setup(StereoWindow * stereoWindow, GLFWwindow * context, bool is_left){};
    virtual void teardown(StereoWindow * stereoWindow, GLFWwindow * context, bool is_left){};
    virtual void resize_callback(StereoWindow * stereoWindow, GLFWwindow * context, bool is_left){};
};

#endif //OPENGLVIEWER_IRENDERPROCEDURE_H
