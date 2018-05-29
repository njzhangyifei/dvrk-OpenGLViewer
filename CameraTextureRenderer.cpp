//
// Created by arclab on 4/20/18.
//

#include "CameraTextureRenderer.h"

void CameraTextureRenderer::execute(StereoWindow *stereoWindow, GLFWwindow *context, bool is_left) {
    if (is_left){
        // upload textures;
        image_provider_left->upload();
        image_provider_right->upload();
        this->texture_id_left = image_provider_left->texture_id;
        this->texture_id_right = image_provider_right->texture_id;
    }
    if (is_left) {
        this->texture_scale_height = ((float)stereoWindow->height) / image_provider_left->height;
        this->texture_scale_width =  ((float)stereoWindow->width) / image_provider_left->width;
    } else {
        this->texture_scale_height = ((float)stereoWindow->height) / image_provider_right->height;
        this->texture_scale_width =  ((float)stereoWindow->width) / image_provider_right->width;
    }
    TextureRenderer::execute(stereoWindow, context, is_left);
    if (is_left) {
        cv::Mat gray;
        cv::cvtColor(image_provider_left->image, gray, cv::COLOR_RGB2GRAY);
        std::vector<cv::Point2f> corners; //this will be filled by the detected corners
        bool found = cv::findChessboardCorners(gray, cv::Size(7,9), corners);
        std::cerr << found << std::endl;
        auto criteria = cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 30, 0.001)
        cv::cornerSubPix(gray, corners, cv::Size(11,11), cv::Size(-1,-1), criteria );
        cv::solvePnP()

    }
}

void CameraTextureRenderer::resize_callback(StereoWindow *stereoWindow, GLFWwindow *context, bool is_left) {
    TextureRenderer::resize_callback(stereoWindow, context, is_left);
}
