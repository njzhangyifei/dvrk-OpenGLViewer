//
// Created by yifei on 4/22/18.
//

#include <vtkCamera.h>
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
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

        vtkWin = vtkSmartPointer<vtkExternalOpenGLRenderWindowFixed>::New();
        vtkWin->Start();
//        vtkWin->DoubleBufferOff();
//        vtkWin->SwapBuffersOff();
//        vtkWin->StereoCapableWindowOn();
//        vtkWin->SetStereoTypeToLeft();
//        vtkWin->StereoRenderOn();
        vtkWin->AlphaBitPlanesOn();


        // Create a sphere
        vtkSmartPointer<vtkSphereSource> sphereSource =
                vtkSmartPointer<vtkSphereSource>::New();
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

        // Combine the sphere and cube into an assembly
        vtkSmartPointer<vtkAssembly> assembly =
                vtkSmartPointer<vtkAssembly>::New();
        assembly->AddPart(sphereActor);
        assembly->AddPart(cubeActor);


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
            vtkActor::SafeDownCast(collection->GetNextProp())->GetProperty()->SetOpacity(0.5);
        }

        vtkSmartPointer<vtkVectorText> textSource =
                vtkSmartPointer<vtkVectorText>::New();
        textSource->SetText("Hello");
        textSource->Update();

//        // Create a mapper and actor
//        vtkSmartPointer<vtkPolyDataMapper> mapper =
//                vtkSmartPointer<vtkPolyDataMapper>::New();
//        mapper->SetInputConnection(textSource->GetOutputPort());
//
//        vtkSmartPointer<vtkActor> text_actor =
//                vtkSmartPointer<vtkActor>::New();
//        text_actor->SetMapper(mapper);
//        text_actor->GetProperty()->SetColor(1.0, 0.0, 0.0);


        //Create a renderer, render window, and interactor
        renderer = vtkSmartPointer<vtkRenderer>::New();
        renderer->UseDepthPeelingOff();
        renderer->EraseOn();
        renderer->SetBackgroundAlpha(0.0);
        renderer->SetBackground(0.275, 0.510, 0.706);
        renderer->AddActor(assembly);
//        renderer->ResetCamera();
        renderer->GetActiveCamera()->SetViewUp(0, -1, 0);
        renderer->GetActiveCamera()->UseOffAxisProjectionOn();

//        renderer->AddActor(text_actor);
        vtkWin->AddRenderer(renderer);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        texture_renderer = std::make_unique<TextureRenderer>(true);
    }
    texture_renderer->setup(stereoWindow, context, is_left);
    texture_renderer->texture_id_left = colorTexture_L;
    texture_renderer->texture_depth_id_left = depthTexture_L;
    texture_renderer->texture_id_right = colorTexture_R;
    texture_renderer->texture_depth_id_right = depthTexture_R;

}

void VTKRenderProcedure::execute(StereoWindow *stereoWindow, GLFWwindow *context, bool is_left) {
    if (is_left) {
        glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);

        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, colorTexture_L, 0);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture_L, 0);
//        vtkWin->SetStereoTypeToLeft();
        renderer->SetActiveCamera(VTKCameraManager::get_instance()->camera_left);
        vtkWin->Render();

        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, colorTexture_R, 0);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture_R, 0);
//        vtkWin->SetStereoTypeToRight();
        renderer->SetActiveCamera(VTKCameraManager::get_instance()->camera_right);
        vtkWin->Render();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);
    texture_renderer->execute(stereoWindow, context, is_left);
    glDisable(GL_BLEND);
}

void VTKRenderProcedure::resize_callback(StereoWindow *stereoWindow, GLFWwindow *context, bool is_left) {
    if (is_left) {
//        glGetIntegerv(GL_VIEWPORT, info);
//        int info[4];
//        vtkWin->SetPosition(info[0], info[1]);
//        vtkWin->SetSize(info[2], info[3]);
//        vtkWin->Modified();

        // 1920 1080
//        float ratio = info[2] / ((float)info[3]);
//        renderer->GetActiveCamera()->SetScreenBottomLeft (-0.5, -0.5 / ratio, -0.5);
//        renderer->GetActiveCamera()->SetScreenBottomRight( 0.5, -0.5 / ratio, -0.5);
//        renderer->GetActiveCamera()->SetScreenTopRight   ( 0.5,  0.5 / ratio, -0.5);
//        renderer->GetActiveCamera()->UseOffAxisProjectionOn();

        // Give an empty image to OpenGL ( the last "0" )
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, colorTexture_L);
        glTexImage2D(GL_TEXTURE_2D, 0,
                     GL_RGBA8, stereoWindow->width, stereoWindow->height, 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, 0);
        glBindTexture(GL_TEXTURE_2D, 0);

        glBindTexture(GL_TEXTURE_2D, colorTexture_R);
        glTexImage2D(GL_TEXTURE_2D, 0,
                     GL_RGBA8, stereoWindow->width, stereoWindow->height, 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, 0);
        glBindTexture(GL_TEXTURE_2D, 0);

        glBindTexture(GL_TEXTURE_2D, depthTexture_L);
        glTexImage2D(GL_TEXTURE_2D, 0,
                     GL_DEPTH_COMPONENT24, stereoWindow->width, stereoWindow->height, 0,
                     GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
        glBindTexture(GL_TEXTURE_2D, 0);

        glBindTexture(GL_TEXTURE_2D, depthTexture_R);
        glTexImage2D(GL_TEXTURE_2D, 0,
                     GL_DEPTH_COMPONENT24, stereoWindow->width, stereoWindow->height, 0,
                     GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

void VTKRenderProcedure::imgui_callback(StereoWindow * stereoWindow, GLFWwindow * context, bool is_left) {
#ifdef __WITH_IMGUI
    if (!is_left) return;
    {
        ImGui::Begin(typeid(this).name());
        transform->Identity();
        static float x, y, z;
        ImGui::SliderFloat("X displacement", &x, -1.0f, 1.0f);
        ImGui::SliderFloat("Y displacement", &y, -1.0f, 1.0f);
        ImGui::SliderFloat("Z displacement", &z, -10.0f, 0.0f);
        transform->Translate(x,y,z);
        double eye[3];
        renderer->GetActiveCamera()->GetPosition(eye);
        ImGui::Text("Eye Position: [%.2f   %.2f   %.2f]", eye[0], eye[1], eye[2]);
        ImGui::End();
    }
#endif
}

void VTKRenderProcedure::image_resize_callback(int width, int height, bool is_left) {

}



