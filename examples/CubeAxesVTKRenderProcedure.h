//
// Created by Yifei on 6/20/2018.
//

#ifndef DVRK_OPENGLVIEWER_CUBEAXESVTKRENDERPROCEDURE_H
#define DVRK_OPENGLVIEWER_CUBEAXESVTKRENDERPROCEDURE_H


#include "dvrk_OpenGLViewer/VTKRenderProcedure.h"

class CubeAxesVTKRenderProcedure : public VTKRenderProcedure{
public:
    vtkSmartPointer<vtkTransform> transform;
    void vtk_setup(StereoWindow *stereoWindow, GLFWwindow *context, bool is_left) override;
    void execute(StereoWindow *stereoWindow, GLFWwindow *context, bool is_left) override;
    void imgui_callback(StereoWindow * stereoWindow, GLFWwindow * context, bool is_left) override;
};


#endif //DVRK_OPENGLVIEWER_CUBEAXESVTKRENDERPROCEDURE_H
