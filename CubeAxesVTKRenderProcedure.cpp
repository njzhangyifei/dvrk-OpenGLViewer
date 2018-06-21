//
// Created by Yifei on 6/20/2018.
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
#include <vtkAxesActor.h>
#include "CubeAxesVTKRenderProcedure.h"
#include "VTKCameraManager.h"

#include "CameraTextureRenderer.h"

#ifdef __WITH_IMGUI
#include <imgui.h>
#endif

void CubeAxesVTKRenderProcedure::vtk_setup(StereoWindow *stereoWindow, GLFWwindow *context, bool is_left) {
    VTKRenderProcedure::vtk_setup(stereoWindow, context, is_left);
    if (is_left) {
        vtkWin = vtkSmartPointer<vtkExternalOpenGLRenderWindowFixed>::New();
        vtkWin->Start();

        // Create a sphere
        vtkSmartPointer<vtkSphereSource> sphereSource =
                vtkSmartPointer<vtkSphereSource>::New();
        sphereSource->SetRadius(0.5);
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
        cubeSource->SetXLength(4 * 2.45);
        cubeSource->SetYLength(4 * 2.45);
        cubeSource->SetZLength(4 * 2.45);
        cubeSource->SetCenter(3.0 * 2.45,
                              3.0 * 2.45,
                              2 * 2.45);
        cubeSource->Update();

        vtkSmartPointer<vtkPolyDataMapper> cubeMapper =
                vtkSmartPointer<vtkPolyDataMapper>::New();
        cubeMapper->SetInputConnection(cubeSource->GetOutputPort());

        vtkSmartPointer<vtkActor> cubeActor =
                vtkSmartPointer<vtkActor>::New();
        cubeActor->SetMapper(cubeMapper);
        vtkSmartPointer<vtkTransform> cubeTransform = vtkSmartPointer<vtkTransform>::New();
        cubeTransform->Identity();
        cubeTransform->RotateX(180);
        cubeTransform->RotateZ(-90);
        cubeActor->SetUserTransform(cubeTransform);

        vtkSmartPointer<vtkAxesActor> axes = vtkSmartPointer<vtkAxesActor>::New();
        axes->SetTotalLength(3,3,3);

//        vtkSmartPointer<vtkVectorText> textSource =
//                vtkSmartPointer<vtkVectorText>::New();
//        textSource->SetText("Hello");
//        textSource->Update();
//
//        // Create a mapper and actor
//        vtkSmartPointer<vtkPolyDataMapper> mapper =
//                vtkSmartPointer<vtkPolyDataMapper>::New();
//        mapper->SetInputConnection(textSource->GetOutputPort());
//
//        vtkSmartPointer<vtkActor> text_actor =
//                vtkSmartPointer<vtkActor>::New();
//        text_actor->SetMapper(mapper);
//        text_actor->GetProperty()->SetColor(1.0, 0.0, 0.0);

        // Combine the sphere and cube into an assembly
        vtkSmartPointer<vtkAssembly> assembly =
                vtkSmartPointer<vtkAssembly>::New();
        assembly->AddPart(sphereActor);
        assembly->AddPart(cubeActor);
        assembly->AddPart(axes);
//        assembly->AddPart(text_actor);

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
//            vtkActor::SafeDownCast(collection->GetNextProp())->GetProperty()-> do some thing;
        }

        renderer = vtkSmartPointer<vtkRenderer>::New();
        renderer->UseDepthPeelingOff();
        renderer->EraseOn();
        renderer->SetBackgroundAlpha(0.0);
        renderer->SetBackground(0.275, 0.510, 0.706);
        renderer->AddActor(assembly);
        vtkWin->AddRenderer(renderer);
    }
}

void CubeAxesVTKRenderProcedure::execute(StereoWindow *stereoWindow, GLFWwindow *context, bool is_left) {

    ////////////////////////////
    ////////////////////////////
    ////////////////////////////
    // DEMO CODE
    //
    // load matrix from a cv::Mat, which is a 4x4 homogeneous tf matrix
    //
    if (!obj.empty()){
        transform->SetMatrix((double *)obj.data);
    }
    ////////////////////////////
    ////////////////////////////

    // set transparency
    texture_renderer->use_transparency = true;
    texture_renderer->transparency = 0.8f;
    VTKRenderProcedure::execute(stereoWindow, context, is_left);
}

void CubeAxesVTKRenderProcedure::imgui_callback(StereoWindow * stereoWindow, GLFWwindow * context, bool is_left) {
#ifdef __WITH_IMGUI
    if (!is_left) return;
    {
        ImGui::Begin(typeid(this).name());

//        set transform of the assembly with a double matrix
//
//        double matrix[16] = {-0.08139533854063273, -0.9869691150707346, 0.1388047721094156, -29.53294164007212,
//        0.9739215054909224, -0.1083539240752634, -0.1993397308109627, -31.64446475217511,
//        0.2117821994553621, 0.1189596277481696, 0.9700499507550454, 152.8550029792795,
//        0, 0, 0, 1};
//        transform->SetMatrix(matrix);

//        static float x = -29.53294164007212;
//        static float y = -31.64446475217511;
//        static float z = 152.8550029792795;
//        ImGui::SliderFloat("X displacement", &x, -100.0f, 100.0f);
//        ImGui::SliderFloat("Y displacement", &y, -100.0f, 100.0f);
//        ImGui::SliderFloat("Z displacement", &z, -200.0f, 200.0f);
//        transform->Translate(x,y,z);

        double eye[3];
        transform->GetPosition(eye);
        ImGui::Text("Object Position: [%.2f   %.2f   %.2f]", eye[0], eye[1], eye[2]);
        VTKCameraManager::get()->camera_left->GetPosition(eye);
        ImGui::Text("L Eye Position: [%.2f   %.2f   %.2f]", eye[0], eye[1], eye[2]);
        VTKCameraManager::get()->camera_left->GetFocalPoint(eye);
        ImGui::Text("L Focal Point : [%.2f   %.2f   %.2f]", eye[0], eye[1], eye[2]);
        VTKCameraManager::get()->camera_right->GetPosition(eye);
        ImGui::Text("R Eye Position: [%.2f   %.2f   %.2f]", eye[0], eye[1], eye[2]);
        VTKCameraManager::get()->camera_right->GetFocalPoint(eye);
        ImGui::Text("R Focal Point : [%.2f   %.2f   %.2f]", eye[0], eye[1], eye[2]);
        ImGui::Checkbox("Distortion", &texture_renderer->use_distortion);
        ImGui::End();
    }
#endif
}
