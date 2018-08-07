//
// Created by Yifei on 1/24/2018.
//

#include "RendererPsmTool.h"
#include "RobotState.h"

#include <cmath>
#include <iostream>

RendererPsmTool::RendererPsmTool(vtkSmartPointer<vtkRenderer> ren, std::string model) :
    model_dir(model) {

    renderer = vtkSmartPointer<vtkRenderer>(ren);

    light = vtkSmartPointer<vtkLight>::New();
    light->SetLightTypeToCameraLight();

    light->SetConeAngle(10);
    light->SetIntensity(0.9);

    light->SetDiffuseColor(1,1,1);
    light->SetAmbientColor(1,1,1);

    renderer->AddLight(light);
    robotState = new RobotState();
}

void RendererPsmTool::load_psm_tools(PsmTool::ToolType psm1_type, PsmTool::ToolType psm2_type ) {
    if (psm1_type != PsmTool::ToolType::None) {
        tools[0] = std::unique_ptr<PsmTool>(new PsmTool(psm1_type));
        tools[0]->init(this->model_dir);

        renderer->AddActor(tools[0]->Shaft_Actor());
        renderer->AddActor(tools[0]->Logo_Actor());
        renderer->AddActor(tools[0]->JawL_Actor());
        renderer->AddActor(tools[0]->JawR_Actor());
    }
    if (psm2_type != PsmTool::ToolType::None) {
        tools[1] = std::unique_ptr<PsmTool>(new PsmTool(psm2_type));
        tools[1]->init(this->model_dir);

        renderer->AddActor(tools[1]->Shaft_Actor());
        renderer->AddActor(tools[1]->Logo_Actor());
        renderer->AddActor(tools[1]->JawL_Actor());
        renderer->AddActor(tools[1]->JawR_Actor());
    }
}

void RendererPsmTool::update_robotState(RobotState * robot_state){
    if (robot_state) {
        try{
            std::lock_guard<std::mutex> lock(robotState->data_mutex);
            *robotState = *robot_state;
        }
        catch (std::exception& e) {
            std::cout << "Robot State is not ready yet." << std::endl;
            std::cerr << e.what() << std::endl;
            return;
        }
    }
}


void RendererPsmTool::update_actors() {

    try{
        std::lock_guard<std::mutex> lock(robotState->data_mutex);

        if (tools[0]) {
            Eigen::Matrix<double, 4, 4, Eigen::RowMajor> cHb_1;
            cHb_1 = robotState->psm1.bHc.inverse();

            std::lock_guard<std::mutex> lock_tool(tools[0]->data_mutex);
            tools[0]->Update_Base_Transform(cHb_1.data());

            tools[0]->Update_Shaft_Transform(robotState->psm1.bHj4.data());
            tools[0]->Update_Logo_Transform(robotState->psm1.bHj5.data());
            tools[0]->Update_JawL_Transform(robotState->psm1.bHeL.data());
            tools[0]->Update_JawR_Transform(robotState->psm1.bHeR.data());

        }
        if (tools[1]) {
            Eigen::Matrix<double, 4, 4, Eigen::RowMajor> cHb_2;
            cHb_2 = robotState->psm2.bHc.inverse();

            std::lock_guard<std::mutex> lock_tool(tools[1]->data_mutex);
            tools[1]->Update_Base_Transform(cHb_2.data());

            tools[1]->Update_Shaft_Transform(robotState->psm2.bHj4.data());
            tools[1]->Update_Logo_Transform(robotState->psm2.bHj5.data());
            tools[1]->Update_JawL_Transform(robotState->psm2.bHeL.data());
            tools[1]->Update_JawR_Transform(robotState->psm2.bHeR.data());
        }
    }
    catch (std::exception& e) {
        std::cout << "Robot State is not ready yet." << std::endl;
        std::cerr << e.what() << std::endl;
        return;
    }

}
