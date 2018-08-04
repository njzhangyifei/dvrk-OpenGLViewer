#include "ROSStateProvider.h"

#include <tf_conversions/tf_eigen.h>
#include <dvrk_components_ros/robotlink.h>


ROSStateProvider::ROSStateProvider(int millis, ros::NodeHandlePtr nh):
    SimpleTimer(millis), robot_state(nullptr){
    psm1 = new PSM(*nh, 1);
    psm2 = new PSM(*nh, 2);
}

void ROSStateProvider::read_state(PSM *psm, PsmState& output_psm) {

    geometry_msgs::PoseStamped p = psm->get_target_master_cart_pos();

    if(p.pose.position.x != 0 && p.pose.position.y != 0 && p.pose.position.z != 0){
        robotLink link = psm->inverseKin_robotLink(p.pose);

        output_psm.bHj4 = link.rosPose2EigenT(link.getPose(4)).matrix().cast<double>();
        output_psm.bHj5 = link.rosPose2EigenT(link.getPose(5)).matrix().cast<double>();
        output_psm.bHe  = link.rosPose2EigenT(link.getPose(6)).matrix().cast<double>();
        output_psm.jaw  = (float) psm->get_target_master_jaw().data;
    }
    else{
        output_psm.bHj4 = psm->link.rosPose2EigenT(psm->get_master_pose_at_joint(4)).matrix().cast<double>();
        output_psm.bHj5 = psm->link.rosPose2EigenT(psm->get_master_pose_at_joint(5)).matrix().cast<double>();
        output_psm.bHe  = psm->link.rosPose2EigenT(psm->get_master_pose_at_joint(6)).matrix().cast<double>();
        output_psm.jaw  = (float) psm->get_master_jaw().data;
    }


    //Multiplying translation elements to convert from meters to mm
    output_psm.bHj4.block(0, 3, 3, 1) *= 1000.0;
    output_psm.bHj5.block(0, 3, 3, 1) *= 1000.0;
    output_psm.bHe.block (0, 3, 3, 1) *= 1000.0;

    //For some reason we need to use ISI standard here?
    Eigen::Matrix<double, 4, 4, Eigen::RowMajor> tooltip_offset;

    tooltip_offset <<    0.0, -1.0,  0.0,  0.0,
                         0.0,  0.0,  1.0,  0.0,
                        -1.0,  0.0,  0.0,  0.0,
                         0.0,  0.0,  0.0,  1.0;

    output_psm.bHj4 = output_psm.bHj4 * tooltip_offset;
    output_psm.bHj5 = output_psm.bHj5 * tooltip_offset;
    output_psm.bHe  = output_psm.bHe  * tooltip_offset;

}

void ROSStateProvider::update_handeye(){
    tf::StampedTransform transform;
    Eigen::Affine3d affine_bHc;

    tf_listener.lookupTransform("/PSM1_base", "/cam0", ros::Time(0),transform);
    tf::transformTFToEigen(transform, affine_bHc);
    robot_state->psm1.bHc = affine_bHc.matrix();
    robot_state->psm1.bHc.block(0, 3, 3, 1) *= 1000.0;
    robot_state->psm1.bHc_corr = robot_state->psm1.bHc;

    tf_listener.lookupTransform("/PSM2_base", "/cam0", ros::Time(0),transform);
    tf::transformTFToEigen(transform, affine_bHc);
    robot_state->psm2.bHc = affine_bHc.matrix();
    robot_state->psm2.bHc.block(0, 3, 3, 1) *= 1000.0;
    robot_state->psm2.bHc_corr = robot_state->psm2.bHc;

}

void ROSStateProvider::callback() {
    if (robot_state && psm1 && psm2) {
        std::lock_guard<std::mutex> lock(robot_state->data_mutex);

        update_handeye();

        read_state(psm1, robot_state->psm1);
        robot_state->psm1.update_jaw();
        read_state(psm2, robot_state->psm2);
        robot_state->psm2.update_jaw();
    }
}
