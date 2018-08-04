//
// Created by yifei on 4/22/18.
//

#ifndef DVRK_OPENGLVIEWER_VTKRENDERPROCEDURE_H
#define DVRK_OPENGLVIEWER_VTKRENDERPROCEDURE_H


#include "vtkExternalOpenGLRenderWindowFixed.h"
#include "IRenderProcedure.h"
#include "IImageAligned.h"
#include "TextureRenderer.h"

class VTKRenderProcedure  : public IRenderProcedure, public IImageAligned{
protected:
    void resize_textures();
    vtkSmartPointer<vtkExternalOpenGLRenderWindowFixed> vtkWin;
    std::unique_ptr<TextureRenderer> texture_renderer;
    vtkSmartPointer<vtkRenderer> renderer;

public:
    VTKRenderProcedure();

    virtual void vtk_setup(StereoWindow *stereoWindow, GLFWwindow *context, bool is_left);

    void execute(StereoWindow *stereoWindow, GLFWwindow *context, bool is_left) override;
    void setup(StereoWindow *stereoWindow, GLFWwindow *context, bool is_left) override;
    void imgui_callback(StereoWindow *stereoWindow, GLFWwindow *context, bool is_left) override;
    void resize_callback(StereoWindow *stereoWindow, GLFWwindow *context, bool is_left) override;

    void image_resize_callback(int width, int height) override;

    GLuint FramebufferName = 0;
    GLuint colorTexture_L;
    GLuint colorTexture_R;
    GLuint depthTexture_L;
    GLuint depthTexture_R;

};


#endif //DVRK_OPENGLVIEWER_VTKRENDERPROCEDURE_H
