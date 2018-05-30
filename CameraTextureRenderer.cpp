//
// Created by arclab on 4/20/18.
//

#include "CameraTextureRenderer.h"
#include "VTKCameraManager.h"


static void meshgrid(const cv::Mat &xgv, const cv::Mat &ygv,
                     cv::Mat1i &X, cv::Mat1i &Y)
{
    cv::repeat(xgv.reshape(1,1), ygv.total(), 1, X);
    cv::repeat(ygv.reshape(1,1).t(), 1, xgv.total(), Y);
}


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
    if (is_left) {
        static int i = 0;
        i ++;
        if (i >= 100) {
            cv::Mat gray;
            cv::cvtColor(image_provider_left->image, gray, cv::COLOR_RGB2GRAY);
            cv::threshold(gray, gray, 0, 255,  cv::THRESH_OTSU);
//            cv::imshow("test", gray);
            std::vector<cv::Point2f> corners; //this will be filled by the detected corners
            bool found = cv::findChessboardCorners(gray, cv::Size(9,7), corners, 0);
            std::vector<cv::Point3f> objp;
            for (int i = 0; i < 7; i ++) {
                for (int j = 0; j < 9; j ++) {
                    objp.push_back(cv::Point3f(j, i, 0));
                }
            }
            std::cerr << found << std::endl;
            if (found){
                auto criteria = cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 5, 0.001);
                cv::cornerSubPix(gray, corners, cv::Size(11,11), cv::Size(-1,-1), criteria );
                cv::Mat rvec, tvec;
                cv::solvePnP(objp, corners, VTKCameraManager::get()->K_left, VTKCameraManager::get()->dist_coeff_left, rvec, tvec);
                std::cerr << rvec << std::endl;
                std::cerr << tvec << std::endl;
            }
            i = 0;
        }
    }
    TextureRenderer::execute(stereoWindow, context, is_left);
}

void CameraTextureRenderer::resize_callback(StereoWindow *stereoWindow, GLFWwindow *context, bool is_left) {
    TextureRenderer::resize_callback(stereoWindow, context, is_left);
}
