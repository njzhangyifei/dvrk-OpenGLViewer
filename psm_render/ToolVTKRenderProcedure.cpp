//
// Created by Yifei on 6/20/2018.
//
#include <vtkCamera.h>
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkWarpLens.h>
#include <vtkConeSource.h>
#include "dvrk_OpenGLViewer/VTKRenderProcedure.h"
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
#include "dvrk_OpenGLViewer/VTKCameraManager.h"

#include "ToolVTKRenderProcedure.h"

//can remove after removing hardcoded hand eye
#include <opencv2/core/eigen.hpp>

#ifdef __WITH_IMGUI
#include <imgui.h>
#endif

ToolVTKRenderProcedure::ToolVTKRenderProcedure(ros::NodeHandlePtr nh):m_nh(nh){}

void ToolVTKRenderProcedure::vtk_setup(StereoWindow *stereoWindow, GLFWwindow *context, bool is_left) {
    VTKRenderProcedure::vtk_setup(stereoWindow, context, is_left);
    if (is_left) {
        vtkWin = vtkSmartPointer<vtkExternalOpenGLRenderWindowFixed>::New();
        vtkWin->Start();

        renderer = vtkSmartPointer<vtkRenderer>::New();

        //Initialize the robot state
        robotState = new RobotState();

        //Get rosStateProvider running and linked with this robotState
        rosStateProvider = new ROSStateProvider(10, m_nh);
        rosStateProvider->robot_state = robotState;

        //Intialize psm rendering
        psmToolRenderer = new RendererPsmTool(renderer, "/home/arclab/catkin_ws/src/dvrk-OpenGLViewer/model/");

//      LND,
//		MCS,
//		ProGrasp,
//		CF,
//		MBF,
//		RTS,
        psmToolRenderer->load_psm_tools(PsmTool::ToolType::LND, PsmTool::ToolType::LND);

        renderer->UseDepthPeelingOff();
        renderer->EraseOn();
        renderer->SetBackgroundAlpha(0.0);
        renderer->SetBackground(0.275, 0.510, 0.706);

        vtkWin->AddRenderer(renderer);

        rosStateProvider->start();
    }
}

ToolVTKRenderProcedure::~ToolVTKRenderProcedure(){
    rosStateProvider->stop();
}

void ToolVTKRenderProcedure::execute(StereoWindow *stereoWindow, GLFWwindow *context, bool is_left) {


    auto start = std::chrono::system_clock::now();

    if(!is_left){
        RobotState t_robotState;

        //Update the transforms for PSM tool here
        if(!rosStateProvider->robot_state) return;    //If we have nothing here just exit

        try
        {
            std::lock_guard<std::mutex> lock(robotState->data_mutex);
            t_robotState = *robotState;

        } catch (std::exception& e) {
            std::cout << "Robot State is not ready yet." << std::endl;
            std::cerr << e.what() << std::endl;
            return;
        }

        psmToolRenderer->update_actors(&t_robotState);
    }

    // set transparency
    texture_renderer->use_transparency = true;
    texture_renderer->transparency = 0.4f;
    VTKRenderProcedure::execute(stereoWindow, context, is_left);

    auto end = std::chrono::system_clock::now();
    std::chrono::duration<float> elapsed_seconds = end-start;
    render_time = elapsed_seconds.count() * 1000;

}

void ToolVTKRenderProcedure::imgui_callback(StereoWindow * stereoWindow, GLFWwindow * context, bool is_left) {
#ifdef __WITH_IMGUI
    if (!is_left) return;
    {
        ImGui::Begin(typeid(this).name());

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
