//
// Created by yifei on 4/22/18.
//

#ifndef DVRK_OPENGLVIEWER_VTKRENDERPROCEDURE_H
#define DVRK_OPENGLVIEWER_VTKRENDERPROCEDURE_H


#include "vtkExternalOpenGLRenderWindowFixed.h"
#include "IRenderProcedure.h"

class VTKRenderProcedure  : public IRenderProcedure{
protected:
    vtkSmartPointer<vtkExternalOpenGLRenderWindowFixed> vtkWin;
public:
    VTKRenderProcedure();

    void execute(StereoWindow *stereoWindow, GLFWwindow *context, bool is_left) override;
    void setup(StereoWindow *stereoWindow, GLFWwindow *context, bool is_left) override;
    void resize_callback(StereoWindow *stereoWindow, GLFWwindow *context, bool is_left) override;
};


#endif //DVRK_OPENGLVIEWER_VTKRENDERPROCEDURE_H
