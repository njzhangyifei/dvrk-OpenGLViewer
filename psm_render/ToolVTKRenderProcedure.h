//
// Created by Yifei on 6/20/2018.
//

#ifndef DVRK_OPENGLVIEWER_CUBEAXESVTKRENDERPROCEDURE_H
#define DVRK_OPENGLVIEWER_CUBEAXESVTKRENDERPROCEDURE_H


#include "dvrk_OpenGLViewer/VTKRenderProcedure.h"

#include "RobotState.h"
#include "RendererPsmTool.h"
#include "ROSStateProvider.h"

class ToolVTKRenderProcedure : public VTKRenderProcedure{
public:
    ToolVTKRenderProcedure(ros::NodeHandlePtr nh);
    ~ToolVTKRenderProcedure();

    void vtk_setup(StereoWindow *stereoWindow, GLFWwindow *context, bool is_left) override;
    void execute(StereoWindow *stereoWindow, GLFWwindow *context, bool is_left) override;
    void imgui_callback(StereoWindow * stereoWindow, GLFWwindow * context, bool is_left) override;

    float render_time;

private:
    ros::NodeHandlePtr m_nh;

    RendererPsmTool  *psmToolRenderer;
    RobotState       *robotState;

    ROSStateProvider *rosStateProvider;

    bool firstTime;

};


#endif //DVRK_OPENGLVIEWER_CUBEAXESVTKRENDERPROCEDURE_H
