//
// Created by Yifei on 5/29/2018.
//

#include "dvrk_OpenGLViewer/StaticStereoImageSource.h"
#include <opencv2/opencv.hpp>

StaticStereoImageSource::StaticStereoImageSource(int millis, const char *file_left, const char *file_right)
        : SimpleTimer(millis) {
    left =  cv::imread(file_left);
    right = cv::imread(file_right);

    cv::cvtColor(left, left, cv::COLOR_RGB2BGR);
    cv::cvtColor(right, right, cv::COLOR_RGB2BGR);
    this->start();
}

void StaticStereoImageSource::callback() {
    if (image_provider_left){
        image_provider_left->set_image(left);
    }
    if (image_provider_right){
        image_provider_right->set_image(right);
    }
}

