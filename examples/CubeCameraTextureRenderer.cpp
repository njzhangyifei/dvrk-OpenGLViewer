//
// Created by arclab on 4/20/18.
//


#include "CubeCameraTextureRenderer.h"
#include "dvrk_OpenGLViewer/VTKCameraManager.h"


/////////////////////////////////////
/////////////////////////////////////
// FOR DEMO
cv::Mat obj;
/////////////////////////////////////
/////////////////////////////////////


static void meshgrid(const cv::Mat &xgv, const cv::Mat &ygv,
                     cv::Mat1i &X, cv::Mat1i &Y)
{
    cv::repeat(xgv.reshape(1,1), ygv.total(), 1, X);
    cv::repeat(ygv.reshape(1,1).t(), 1, xgv.total(), Y);
}


void CubeCameraTextureRenderer::execute(StereoWindow *stereoWindow, GLFWwindow *context, bool is_left) {
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

    /////////////////////////////////////
    /////////////////////////////////////
    /////////////////////////////////////
    // Experimental code, FOR DEMO ONLY
    //
    //
    // EXTEREMELY SLOW PERFORMANCE IF ENABLED!!!
    // PLEASE DO IMAGE PROCESSING IN ANOTHER THREAD (that subscribes image provider)
    //
    //
    // 1. detect checkerboard (9x7) from image
    // 2. convert r_vec and t_vec into homogeneous tf matrix
    // 3. save matrix to GLOBAL var (extern-ed) 'obj'
    //
    if (is_left) {
        static int i = 100;
        i ++;
        if (i >= 30) {
            cv::Mat input = image_provider_left->image.clone();
            cv::Mat gray;
            cv::cvtColor(input, gray, cv::COLOR_RGB2GRAY);
//            cv::threshold(gray, gray, 0, 255,  cv::THRESH_OTSU);
            std::vector<cv::Point2f> corners; //this will be filled by the detected corners
            bool found = cv::findChessboardCorners(gray, cv::Size(9,7), corners, cv::CALIB_CB_ADAPTIVE_THRESH);
            std::vector<cv::Point3f> objp;
            for (int i = 0; i < 7; i ++) {
                for (int j = 0; j < 9; j ++) {
                    objp.push_back(cv::Point3f(2.45f * j, 2.45f * i, 0));
                }
            }
            std::cerr << found << std::endl;
            if (found){
                auto criteria = cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 5, 0.001);
                cv::cornerSubPix(gray, corners, cv::Size(11,11), cv::Size(-1,-1), criteria );
                cv::Mat rvec, tvec;
                cv::solvePnP(objp, corners, VTKCameraManager::get()->K_left, VTKCameraManager::get()->dist_coeff_left, rvec, tvec);
//                std::vector<cv::Point3f> targets;
//                std::vector<cv::Point2f> image_pts;
//                targets.push_back(cv::Point3f(0, 0, 0));
//                cv::projectPoints(targets,
//                                  rvec, tvec,
//                                  VTKCameraManager::get()->K_left,
//                                  VTKCameraManager::get()->dist_coeff_left,
//                                  image_pts);
//                cv::circle(input, image_pts[0], 5, cv::Scalar(0, 255, 0));
//                cv::imshow("test", input);
//                cv::waitKey(1);

                cv::Mat R;
                cv::Rodrigues(rvec, R); // R is 3x3
                cv::Mat T = cv::Mat::eye(4, 4, R.type()); // T is 4x4
                T( cv::Range(0,3), cv::Range(0,3) ) = R * 1; // copies R into T
                T( cv::Range(0,3), cv::Range(3,4) ) = tvec * 1; // copies tvec into T

                obj = T;
            }
            i = 0;
        }
    }
    /////////////////////////////////////
    /////////////////////////////////////
    /////////////////////////////////////


    TextureRenderer::execute(stereoWindow, context, is_left);
}

void CubeCameraTextureRenderer::resize_callback(StereoWindow *stereoWindow, GLFWwindow *context, bool is_left) {
    TextureRenderer::resize_callback(stereoWindow, context, is_left);
}
