//
// Created by arclab on 4/20/18.
//

#include "dvrk_OpenGLViewer/ROSStereoImageProvider.h"
#include "dvrk_OpenGLViewer/VTKCameraManager.h"
#include <cv_bridge/cv_bridge.h>
#include <vtkCamera.h>

ROSStereoImageProvider::ROSStereoImageProvider(ros::NodeHandlePtr nh_ptr) {
    this->nh_ptr = nh_ptr;
    image_transport = std::make_unique<image_transport::ImageTransport>(*this->nh_ptr);
    subscriber_left = std::make_unique<image_transport::CameraSubscriber>(
            image_transport->subscribeCamera("/stereo/master/left/image" , 1, &ROSStereoImageProvider::image_callback_left, this)
//        image_transport->subscribeCamera("/stereo/slave/left/image" , 1, &ROSStereoImageProvider::image_callback_left, this)
    );
    subscriber_right = std::make_unique<image_transport::CameraSubscriber>(
            image_transport->subscribeCamera("/stereo/master/right/image", 1, &ROSStereoImageProvider::image_callback_right, this)
//        image_transport->subscribeCamera("/stereo/slave/right/image", 1, &ROSStereoImageProvider::image_callback_right, this)
    );
}

void ROSStereoImageProvider::image_callback_left(const sensor_msgs::ImageConstPtr& msg, const sensor_msgs::CameraInfoConstPtr& ci) {
    if (image_provider_left) {
        cv_share_left = cv_bridge::toCvShare(msg, "rgb8");
        image_provider_left->set_image(cv_share_left->image);
    }
}

void ROSStereoImageProvider::image_callback_right(const sensor_msgs::ImageConstPtr& msg, const sensor_msgs::CameraInfoConstPtr& ci) {
    if (image_provider_right){
        cv_share_right = cv_bridge::toCvShare(msg, "rgb8");
        image_provider_right->set_image(cv_share_right->image);
    }
}
