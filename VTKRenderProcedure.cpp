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
//#include <vtkExternalOpenGLRenderWindow.h>

#include "GLError.h"
#include "vtkExternalOpenGLRenderWindowFixed.h"
#include "TextureRenderer.h"

VTKRenderProcedure::VTKRenderProcedure(){
}


int test = 0;
std::unique_ptr<TextureRenderer> texture_renderer;
void VTKRenderProcedure::setup(StereoWindow *stereoWindow, GLFWwindow *context, bool is_left) {
    //Create a mapper and actor
    if (is_left) {
        glGenFramebuffers(1, &FramebufferName);
        glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
        glEnable(GL_DEPTH_TEST); // depth buffer fighting between the cone and the backround without this
        glDepthFunc(GL_LEQUAL);

        glGenTextures(1, &colorTexture);
        glBindTexture(GL_TEXTURE_2D, colorTexture);
        glTexImage2D(GL_TEXTURE_2D, 0,
                     GL_RGBA8, stereoWindow->width, stereoWindow->height, 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, colorTexture, 0);

        glGenTextures(1, &depthTexture);
        glBindTexture(GL_TEXTURE_2D, depthTexture);
        glTexImage2D(GL_TEXTURE_2D, 0,
                     GL_DEPTH_COMPONENT24, stereoWindow->width, stereoWindow->height, 0,
                     GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);

//        glGenRenderbuffers(1, &depthrenderbuffer);
//        glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
//        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, stereoWindow->width, stereoWindow->height);
//        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0);

        // Set the list of draw buffers.
        GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
        glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

        // Always check that our framebuffer is ok
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cerr << "ERROR configuring" << std::endl;

        glBindTexture(GL_TEXTURE_2D, 0);

        vtkWin = vtkSmartPointer<vtkExternalOpenGLRenderWindowFixed>::New();
        vtkWin->SwapBuffersOff();
        vtkWin->StereoCapableWindowOn();
        vtkWin->SetStereoTypeToLeft();
        vtkWin->StereoRenderOn();
        vtkWin->AlphaBitPlanesOn();
        vtkWin->InitializeFromCurrentContext();
        // Create a cone
        vtkSmartPointer<vtkConeSource> coneSource = vtkSmartPointer<vtkConeSource>::New();
        coneSource->Update();
        renderer = vtkSmartPointer<vtkRenderer>::New();
        mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        mapper->SetInputConnection(coneSource->GetOutputPort());
        actor = vtkSmartPointer<vtkActor>::New();
        actor->SetMapper(mapper);


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
        vtkSmartPointer<vtkTransform> transform =
                vtkSmartPointer<vtkTransform>::New();
        transform->PostMultiply(); //this is the key line
        transform->Translate(5.0, 0, 0);
        assembly->SetUserTransform(transform);


        // Extract each actor from the assembly and change its opacity
        vtkSmartPointer<vtkPropCollection> collection =
                vtkSmartPointer<vtkPropCollection>::New();

        assembly->GetActors(collection);
        collection->InitTraversal();
        for(vtkIdType i = 0; i < collection->GetNumberOfItems(); i++)
        {
            vtkActor::SafeDownCast(collection->GetNextProp())->GetProperty()->SetOpacity(0.1);
        }

        //Create a renderer, render window, and interactor
        vtkWin->AddRenderer(renderer);
        renderer->UseDepthPeelingOff();
        renderer->EraseOn();
        renderer->AddActor(assembly);
        renderer->AddActor(actor);
        renderer->ResetCamera();
        renderer->GetActiveCamera()->UseOffAxisProjectionOn();
//        GLint attachment_name;
//        glGetFramebufferAttachmentParameteriv(GL_DRAW_FRAMEBUFFER,
//                                              GL_DEPTH_ATTACHMENT,
////                                              GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE,
//                                              GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME,
//                                              &attachment_name);
//        std::cerr << "DEPTH ATTACHMENT: [" << attachment_name << "]" << std::endl;
//        std::cerr << "DEPTH TEX: [" << depthTexture << "]" << std::endl;
//        std::cerr << "DEPTH SIZE " << vtkWin->GetDepthBufferSize() << std::endl;
//        int rgba[4];
//        vtkWin->GetColorBufferSizes(rgba);
//        std::cerr << "COLOR SIZE "
//                << " R" << rgba[0]
//                << " G" << rgba[1]
//                << " B" << rgba[2]
//                << " A" << rgba[3]
//                << std::endl;
//        std::cerr << "ALPHA BitPlanes " << vtkWin->GetAlphaBitPlanes() << std::endl;


        glDisable(GL_DEPTH_TEST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        texture_renderer = std::make_unique<TextureRenderer>(true);
    } else {
        test -=20;
    }
    renderer->GetActiveCamera()->Azimuth(test);
    texture_renderer->setup(stereoWindow, context, is_left);
}

void VTKRenderProcedure::execute(StereoWindow *stereoWindow, GLFWwindow *context, bool is_left) {
    if (is_left) {
        glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
        vtkWin->SetStereoTypeToLeft();
        glViewport(0, 0, stereoWindow->width, stereoWindow->height);
//        std::cerr << vtkWin->GetDefaultFrameBufferId() << std:: endl;
        glEnable(GL_DEPTH_TEST); // depth buffer fighting between the cone and the backround without this
        glDepthFunc(GL_LEQUAL);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        vtkWin->Render();
        glDisable(GL_DEPTH_TEST); // depth buffer fighting between the cone and the backround without this
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        texture_renderer->texture_id_left = colorTexture;
        texture_renderer->texture_depth_id_left = depthTexture;

        glEnable(GL_DEPTH_TEST); // depth buffer fighting between the cone and the backround without this
        glDepthFunc(GL_ALWAYS);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBlendEquation(GL_FUNC_ADD);
        texture_renderer->execute(stereoWindow, context, is_left);
        glDisable(GL_BLEND);
        glDisable(GL_DEPTH_TEST); // depth buffer fighting between the cone and the backround without this

        glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
        vtkWin->SetStereoTypeToRight();
        glViewport(0, 0, stereoWindow->width, stereoWindow->height);
        glEnable(GL_DEPTH_TEST); // depth buffer fighting between the cone and the backround without this
        glDepthFunc(GL_LEQUAL);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        vtkWin->Render();
        glDisable(GL_DEPTH_TEST); // depth buffer fighting between the cone and the backround without this
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    } else {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        texture_renderer->texture_id_right = colorTexture;
        texture_renderer->texture_depth_id_right = depthTexture;
        glEnable(GL_DEPTH_TEST); // depth buffer fighting between the cone and the backround without this
        glDepthFunc(GL_ALWAYS);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBlendEquation(GL_FUNC_ADD);
        texture_renderer->execute(stereoWindow, context, is_left);
        glDisable(GL_BLEND);
        glDisable(GL_DEPTH_TEST); // depth buffer fighting between the cone and the backround without this
    }
}

void VTKRenderProcedure::resize_callback(StereoWindow *stereoWindow, GLFWwindow *context, bool is_left) {
    if (is_left) {
        int info[4];
        glGetIntegerv(GL_VIEWPORT, info);
        vtkWin->SetPosition(info[0], info[1]);
        vtkWin->SetSize(info[2], info[3]);
        vtkWin->Modified();
        // 1920 1080
        float ratio = info[2] / ((float)info[3]);
        renderer->GetActiveCamera()->SetScreenBottomLeft (-0.5, -0.5 / ratio, -0.5);
        renderer->GetActiveCamera()->SetScreenBottomRight( 0.5, -0.5 / ratio, -0.5);
        renderer->GetActiveCamera()->SetScreenTopRight   ( 0.5,  0.5 / ratio, -0.5);
        renderer->GetActiveCamera()->UseOffAxisProjectionOn();

        // Give an empty image to OpenGL ( the last "0" )
        glBindTexture(GL_TEXTURE_2D, colorTexture);
        glTexImage2D(GL_TEXTURE_2D, 0,
                     GL_RGBA8, stereoWindow->width, stereoWindow->height, 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, 0);
        glBindTexture(GL_TEXTURE_2D, 0);

        glBindTexture(GL_TEXTURE_2D, depthTexture);
        glTexImage2D(GL_TEXTURE_2D, 0,
                     GL_DEPTH_COMPONENT24, stereoWindow->width, stereoWindow->height, 0,
                     GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}




