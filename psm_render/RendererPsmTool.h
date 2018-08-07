//
// Created by Yifei on 1/24/2018.
//

#ifndef OPENGLRENDERER_RENDERERPSMTOOL_H
#define OPENGLRENDERER_RENDERERPSMTOOL_H


#include "PsmTool.h"
#include "RobotState.h"
#include <vtkRenderer.h>
#include <vtkLight.h>
#include <vtkSmartPointer.h>
#include <Eigen/src/Core/util/Constants.h>

class RendererPsmTool {
public:

    RendererPsmTool(vtkSmartPointer<vtkRenderer> r, std::string model_dir);

    void load_psm_tools(PsmTool::ToolType psm1_type, PsmTool::ToolType psm2_type);
    void update_robotState(RobotState * robot_state);
    void update_actors();

    std::unique_ptr<PsmTool> tools[2];

    vtkSmartPointer<vtkRenderer> renderer;

protected:
    vtkSmartPointer<vtkLight> light;
    std::string model_dir;
    std::string config_dir;

    RobotState * robotState;
};


#endif //OPENGLRENDERER_RENDERERPSMTOOL_H
