//
// Created by yifei on 4/22/18.
//

#include <vtkCamera.h>
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkWarpLens.h>
#include <vtkConeSource.h>
#include "VTKRenderProcedure.h"
#include <vtkOpenGLError.h>
#include <vtkRenderer.h>
#include <vtkAssembly.h>
#include <vtkCubeSource.h>
#include <vtkSphereSource.h>
#include <vtkTransform.h>
#include <vtkProperty.h>
#include <vtkProperty.h>
#include <vtkTextProperty.h>
#include <vtkTextActor.h>
#include <vtkVectorText.h>
#include "GLError.h"
#include "vtkExternalOpenGLRenderWindowFixed.h"
#include "TextureRenderer.h"
#include "VTKCameraManager.h"

#ifdef __WITH_IMGUI
#include <imgui.h>
#endif

VTKRenderProcedure::VTKRenderProcedure(){
}

std::unique_ptr<TextureRenderer> texture_renderer;
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

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        vtkWin = vtkSmartPointer<vtkExternalOpenGLRenderWindowFixed>::New();
        vtkWin->Start();
//        vtkWin->SwapBuffersOff();
//        vtkWin->StereoCapableWindowOn();
//        vtkWin->SetStereoTypeToLeft();
//        vtkWin->StereoRenderOn();
//        vtkWin->AlphaBitPlanesOn();


        // Create a sphere
        vtkSmartPointer<vtkSphereSource> sphereSource =
                vtkSmartPointer<vtkSphereSource>::New();
        sphereSource->SetRadius(1);
        sphereSource->Update();

        vtkSmartPointer<vtkPolyDataMapper> sphereMapper =
                vtkSmartPointer<vtkPolyDataMapper>::New();
        sphereMapper->SetInputConnection(sphereSource->GetOutputPort());
        vtkSmartPointer<vtkActor> sphereActor =
                vtkSmartPointer<vtkActor>::New();
        sphereActor->SetMapper(sphereMapper);
        // Create a cube
        vtkSmartPointer<vtkCubeSource> cubeSource =
                vtkSmartPointer<vtkCubeSource>::New();
        cubeSource->SetCenter(5.0, 0.0, 0.0);
        cubeSource->Update();

        vtkSmartPointer<vtkPolyDataMapper> cubeMapper =
                vtkSmartPointer<vtkPolyDataMapper>::New();
        cubeMapper->SetInputConnection(cubeSource->GetOutputPort());

        vtkSmartPointer<vtkActor> cubeActor =
                vtkSmartPointer<vtkActor>::New();
        cubeActor->SetMapper(cubeMapper);

        vtkSmartPointer<vtkVectorText> textSource =
                vtkSmartPointer<vtkVectorText>::New();
        textSource->SetText("Hello");
        textSource->Update();

        // Create a mapper and actor
        vtkSmartPointer<vtkPolyDataMapper> mapper =
                vtkSmartPointer<vtkPolyDataMapper>::New();
        mapper->SetInputConnection(textSource->GetOutputPort());

        vtkSmartPointer<vtkActor> text_actor =
                vtkSmartPointer<vtkActor>::New();
        text_actor->SetMapper(mapper);
        text_actor->GetProperty()->SetColor(1.0, 0.0, 0.0);

        // Combine the sphere and cube into an assembly
        vtkSmartPointer<vtkAssembly> assembly =
                vtkSmartPointer<vtkAssembly>::New();
        assembly->AddPart(sphereActor);
        assembly->AddPart(cubeActor);
        assembly->AddPart(text_actor);


        // Apply a transform to the whole assembly
        transform = vtkSmartPointer<vtkTransform>::New();
        transform->PostMultiply(); //this is the key line
        assembly->SetUserTransform(transform);


        // Extract each actor from the assembly and change its opacity
        vtkSmartPointer<vtkPropCollection> collection =
                vtkSmartPointer<vtkPropCollection>::New();

        assembly->GetActors(collection);
        collection->InitTraversal();
        for(vtkIdType i = 0; i < collection->GetNumberOfItems(); i++)
        {
//            vtkActor::SafeDownCast(collection->GetNextProp())->GetProperty()->SetOpacity(0.9);
        }



        //Create a renderer, render window, and interactor
        renderer = vtkSmartPointer<vtkRenderer>::New();
        renderer->UseDepthPeelingOff();
        renderer->EraseOn();
        renderer->SetBackgroundAlpha(0.0);
        renderer->SetBackground(0.275, 0.510, 0.706);
        renderer->AddActor(assembly);
//        renderer->ResetCamera();
//        renderer->GetActiveCamera()->SetViewUp(0, -1, 0);
//        renderer->GetActiveCamera()->UseOffAxisProjectionOn();

        vtkWin->AddRenderer(renderer);

        texture_renderer = std::make_unique<TextureRenderer>(true, true);
    }
    texture_renderer->setup(stereoWindow, context, is_left);
    texture_renderer->texture_id_left = colorTexture_L;
    texture_renderer->texture_depth_id_left = depthTexture_L;
    texture_renderer->texture_id_right = colorTexture_R;
    texture_renderer->texture_depth_id_right = depthTexture_R;

}

void VTKRenderProcedure::execute(StereoWindow *stereoWindow, GLFWwindow *context, bool is_left) {
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
        renderer->SetActiveCamera(VTKCameraManager::get()->camera_left);
        renderer->Modified();
        vtkWin->Modified();
        vtkWin->Render();

        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, colorTexture_L, 0);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture_L, 0);
        glViewport(pos[0], pos[1], size[0], size[1]);
        renderer->SetActiveCamera(VTKCameraManager::get()->camera_right);
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
    texture_renderer->texture_scale_width =  ((float)stereoWindow->width) / size[0];
    texture_renderer->texture_scale_height = ((float)stereoWindow->height) / size[1];
    auto camera_center = VTKCameraManager::get()->get_camera_center(is_left ? VTKCameraManager::get()->K_left : VTKCameraManager::get()->K_right);
    auto camera_focus = VTKCameraManager::get()->get_camera_focus(is_left ? VTKCameraManager::get()->K_left : VTKCameraManager::get()->K_right);
    texture_renderer->camera_center_focus = {camera_center.first, camera_center.second, camera_focus.first, camera_focus.second};
    texture_renderer->image_size = {VTKCameraManager::get()->image_width, VTKCameraManager::get()->image_height};
    cv::Mat dist_coeff = (is_left ? VTKCameraManager::get()->dist_coeff_left : VTKCameraManager::get()->dist_coeff_right);
    texture_renderer->distortion_tangential = {
            dist_coeff.at<double>(0,2),
            dist_coeff.at<double>(0,3)
    };
    texture_renderer->distortion_radial = {
            dist_coeff.at<double>(0,0),
            dist_coeff.at<double>(0,1),
            dist_coeff.at<double>(0,4)
    };
    texture_renderer->execute(stereoWindow, context, is_left);
    glDisable(GL_BLEND);
}

void VTKRenderProcedure::resize_callback(StereoWindow *stereoWindow, GLFWwindow *context, bool is_left) {
    // do we need to handle this?
}


void VTKRenderProcedure::imgui_callback(StereoWindow * stereoWindow, GLFWwindow * context, bool is_left) {
#ifdef __WITH_IMGUI
    if (!is_left) return;
    {
        ImGui::Begin(typeid(this).name());
        transform->Identity();
        static float x, y, z;
        ImGui::SliderFloat("X displacement", &x, -100.0f, 100.0f);
        ImGui::SliderFloat("Y displacement", &y, -100.0f, 100.0f);
        ImGui::SliderFloat("Z displacement", &z, -200.0f, 200.0f);
        transform->Translate(x,y,z);
        double eye[3];
        transform->GetPosition(eye);
        ImGui::Text("Object Position: [%.2f   %.2f   %.2f]", eye[0], eye[1], eye[2]);
        VTKCameraManager::get()->camera_left->GetPosition(eye);
        ImGui::Text("L Eye Position: [%.2f   %.2f   %.2f]", eye[0], eye[1], eye[2]);
        VTKCameraManager::get()->camera_right->GetPosition(eye);
        ImGui::Text("R Eye Position: [%.2f   %.2f   %.2f]", eye[0], eye[1], eye[2]);
        ImGui::End();
    }
#endif
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

