
#ifndef DVRK_OPENGLVIEWER_ROSSTATEPROVIDER_H
#define DVRK_OPENGLVIEWER_ROSSTATEPROVIDER_H

#include <ros/ros.h>
#include <dvrk_components_ros/psm.h>
#include <tf/transform_listener.h>

#include "dvrk_OpenGLViewer/SimpleTimer.h"

#include "RobotState.h"

#include <memory>
#include <mutex>


//This is run on a seperate thread and updates robotState
//Used to supply transforms for rendering

class ROSStateProvider : public SimpleTimer {
public:
    ROSStateProvider(int millis, ros::NodeHandlePtr nh);

    void callback();
    void update_handeye();
    void read_state(PSM *psm, PsmState& output_psm);

    RobotState      *robot_state;

    //This is used to get real robot information
    PSM             *psm1;
    PSM             *psm2;


    tf::TransformListener tf_listener;

};


#endif //DVRK_OPENGLVIEWER_ROSSTATEPROVIDER_H
