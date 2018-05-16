//
// Created by arclab on 4/20/18.
//

#ifndef DVRK_OPENGLVIEWER_ROSSTEREOIMAGEPROVIDER_H
#define DVRK_OPENGLVIEWER_ROSSTEREOIMAGEPROVIDER_H

#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <memory>
#include <cv_bridge/cv_bridge.h>
#include "ImageProvider.h"

class ROSStereoImageProvider {
public:
    ros::NodeHandlePtr nh_ptr;
    ROSStereoImageProvider(ros::NodeHandlePtr nh_ptr);
    std::unique_ptr<image_transport::ImageTransport> image_transport;
    std::unique_ptr<image_transport::CameraSubscriber> subscriber_left;
    std::unique_ptr<image_transport::CameraSubscriber> subscriber_right;

    std::shared_ptr<ImageProvider> image_provider_left;
    std::shared_ptr<ImageProvider> image_provider_right;

protected:
    cv_bridge::CvImageConstPtr cv_share_left;
    cv_bridge::CvImageConstPtr cv_share_right;
    sensor_msgs::CameraInfoConstPtr camera_info_left;
    sensor_msgs::CameraInfoConstPtr camera_info_right;

    void image_callback_left(const sensor_msgs::ImageConstPtr &msg, const sensor_msgs::CameraInfoConstPtr &ci);
    void image_callback_right(const sensor_msgs::ImageConstPtr &msg, const sensor_msgs::CameraInfoConstPtr &ci);
};


#endif //DVRK_OPENGLVIEWER_ROSSTEREOIMAGEPROVIDER_H
