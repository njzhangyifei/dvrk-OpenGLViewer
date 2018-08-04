//
// Created by yifei on 4/22/18.
//

#include "dvrk_OpenGLViewer/GLError.h"
#include "dvrk_OpenGLViewer/vtkExternalOpenGLRenderWindowFixed.h"
#include "dvrk_OpenGLViewer/TextureRenderer.h"
#include "dvrk_OpenGLViewer/VTKRenderProcedure.h"
#include <vtkRenderer.h>
#include "dvrk_OpenGLViewer/VTKCameraManager.h"

#ifdef __WITH_IMGUI
#include <imgui.h>
#endif

VTKRenderProcedure::VTKRenderProcedure(){
}

void VTKRenderProcedure::vtk_setup(StereoWindow *stereoWindow, GLFWwindow *context, bool is_left) {
    // setup vtk scene in vtkWin and renderer
}

void VTKRenderProcedure::setup(StereoWindow *stereoWindow, GLFWwindow *context, bool is_left) {
    //Create a mapper and actor
    if (is_left) {
        glActiveTexture(GL_TEXTURE0);
        glGenTextures(1, &colorTexture_L);
        glBindTexture(GL_TEXTURE_2D, colorTexture_L);
        glTexImage2D(GL_TEXTURE_2D, 0,
                     GL_RGBA8, stereoWindow->width, stereoWindow->height, 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glBindTexture(GL_TEXTURE_2D, 0);

        glGenTextures(1, &depthTexture_L);
        glBindTexture(GL_TEXTURE_2D, depthTexture_L);
        glTexImage2D(GL_TEXTURE_2D, 0,
                     GL_DEPTH_COMPONENT24, stereoWindow->width, stereoWindow->height, 0,
                     GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
        glBindTexture(GL_TEXTURE_2D, 0);

        glGenTextures(1, &colorTexture_R);
        glBindTexture(GL_TEXTURE_2D, colorTexture_R);
        glTexImage2D(GL_TEXTURE_2D, 0,
                     GL_RGBA8, stereoWindow->width, stereoWindow->height, 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glBindTexture(GL_TEXTURE_2D, 0);

        glGenTextures(1, &depthTexture_R);
        glBindTexture(GL_TEXTURE_2D, depthTexture_R);
        glTexImage2D(GL_TEXTURE_2D, 0,
                     GL_DEPTH_COMPONENT24, stereoWindow->width, stereoWindow->height, 0,
                     GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
        glBindTexture(GL_TEXTURE_2D, 0);

        glGenFramebuffers(1, &FramebufferName);
        glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);

        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, colorTexture_L, 0);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture_L, 0);

        // Set the list of draw buffers.
        GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
        glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

        // Always check that our framebuffer is ok
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cerr << "ERROR configuring" << std::endl;

        vtk_setup(stereoWindow, context, is_left);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        texture_renderer = std::make_unique<TextureRenderer>(true, true);
    }
    texture_renderer->setup(stereoWindow, context, is_left);
    texture_renderer->texture_id_left = colorTexture_L;
    texture_renderer->texture_depth_id_left = depthTexture_L;
    texture_renderer->texture_id_right = colorTexture_R;
    texture_renderer->texture_depth_id_right = depthTexture_R;
}

void VTKRenderProcedure::execute(StereoWindow *stereoWindow, GLFWwindow *context, bool is_left) {

    // setup


    int * pos = vtkWin->GetPosition();
    int * size = vtkWin->GetSize();


    if (is_left) {

        glEnable(GL_DEPTH_TEST); // depth buffer fighting between the cone and the backround without this
        glDepthFunc(GL_LEQUAL);
        int original_viewport[4];
        glGetIntegerv(GL_VIEWPORT, original_viewport);
        resize_textures();
        glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);

        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, colorTexture_R, 0);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture_R, 0);
        glViewport(pos[0], pos[1], size[0], size[1]);

        renderer->SetActiveCamera(VTKCameraManager::get()->camera_right);
        renderer->Modified();

        vtkWin->Modified();
        vtkWin->Render();

        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, colorTexture_L, 0);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture_L, 0);
        glViewport(pos[0], pos[1], size[0], size[1]);
        renderer->SetActiveCamera(VTKCameraManager::get()->camera_left);
        renderer->Modified();
        vtkWin->Modified();
        vtkWin->Render();

        glFlush();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(original_viewport[0], original_viewport[1],
                   original_viewport[2], original_viewport[3]);
        glDisable(GL_DEPTH_TEST);
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);

    texture_renderer->texture_scale_width = ((float)stereoWindow->width) / size[0];
    texture_renderer->texture_scale_height = ((float)stereoWindow->height) / size[1];
    texture_renderer->image_size = {VTKCameraManager::get()->image_width, VTKCameraManager::get()->image_height};
    texture_renderer->distortion_texture_left = VTKCameraManager::get()->distortion_texture_left;
    texture_renderer->distortion_texture_right = VTKCameraManager::get()->distortion_texture_right;
    texture_renderer->execute(stereoWindow, context, is_left);
    glDisable(GL_BLEND);
}

void VTKRenderProcedure::resize_callback(StereoWindow *stereoWindow, GLFWwindow *context, bool is_left) {
    // do we need to handle this?
}


void VTKRenderProcedure::imgui_callback(StereoWindow * stereoWindow, GLFWwindow * context, bool is_left) {
    // on window immediate gui
}

void VTKRenderProcedure::image_resize_callback(int width, int height) {
    IImageAligned::image_resize_callback(width, height);
}

void VTKRenderProcedure::resize_textures(){
    {
        std::lock_guard<std::mutex> lk(image_resize_lock);
        if (image_resized) {
            vtkWin->SetPosition(0, 0);
            vtkWin->SetSize(image_width, image_height);
            vtkWin->Modified();

            // image is resized, redo textures
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, colorTexture_L);
            glTexImage2D(GL_TEXTURE_2D, 0,
                         GL_RGBA8, image_width, image_height, 0,
                         GL_RGBA, GL_UNSIGNED_BYTE, 0);
            glBindTexture(GL_TEXTURE_2D, 0);

            glBindTexture(GL_TEXTURE_2D, colorTexture_R);
            glTexImage2D(GL_TEXTURE_2D, 0,
                         GL_RGBA8, image_width, image_height, 0,
                         GL_RGBA, GL_UNSIGNED_BYTE, 0);
            glBindTexture(GL_TEXTURE_2D, 0);

            glBindTexture(GL_TEXTURE_2D, depthTexture_L);
            glTexImage2D(GL_TEXTURE_2D, 0,
                         GL_DEPTH_COMPONENT24, image_width, image_height, 0,
                         GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
            glBindTexture(GL_TEXTURE_2D, 0);

            glBindTexture(GL_TEXTURE_2D, depthTexture_R);
            glTexImage2D(GL_TEXTURE_2D, 0,
                         GL_DEPTH_COMPONENT24, image_width, image_height, 0,
                         GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
            glBindTexture(GL_TEXTURE_2D, 0);
            image_resized = false;
        }
    }
}


