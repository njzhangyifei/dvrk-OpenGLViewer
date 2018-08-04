//
// Created by arclab on 4/20/18.
//


#include "dvrk_OpenGLViewer/CameraTextureRenderer.h"
#include "dvrk_OpenGLViewer/VTKCameraManager.h"


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

    TextureRenderer::execute(stereoWindow, context, is_left);
}

void CameraTextureRenderer::resize_callback(StereoWindow *stereoWindow, GLFWwindow *context, bool is_left) {
    TextureRenderer::resize_callback(stereoWindow, context, is_left);
}
