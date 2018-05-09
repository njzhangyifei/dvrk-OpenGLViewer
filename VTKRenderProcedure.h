//
// Created by yifei on 4/22/18.
//

#ifndef DVRK_OPENGLVIEWER_VTKRENDERPROCEDURE_H
#define DVRK_OPENGLVIEWER_VTKRENDERPROCEDURE_H


#include "vtkExternalOpenGLRenderWindowFixed.h"
#include "IRenderProcedure.h"
#include "IImageAligned.h"

class VTKRenderProcedure  : public IRenderProcedure, public IImageAligned{
protected:
    void resize_textures();
    vtkSmartPointer<vtkExternalOpenGLRenderWindowFixed> vtkWin;
public:
    VTKRenderProcedure();

    void execute(StereoWindow *stereoWindow, GLFWwindow *context, bool is_left) override;
    void setup(StereoWindow *stereoWindow, GLFWwindow *context, bool is_left) override;
    void resize_callback(StereoWindow *stereoWindow, GLFWwindow *context, bool is_left) override;
    void image_resize_callback(int width, int height) override;

    GLuint FramebufferName = 0;
    vtkSmartPointer<vtkRenderer> renderer;
    GLuint colorTexture_L;
    GLuint colorTexture_R;
    GLuint depthTexture_L;
    GLuint depthTexture_R;

    void imgui_callback(StereoWindow *stereoWindow, GLFWwindow *context, bool is_left);
    vtkSmartPointer<vtkTransform> transform;
};


#endif //DVRK_OPENGLVIEWER_VTKRENDERPROCEDURE_H
