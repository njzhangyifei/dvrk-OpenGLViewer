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
}

void CameraTextureRenderer::resize_callback(StereoWindow *stereoWindow, GLFWwindow *context, bool is_left) {
    TextureRenderer::resize_callback(stereoWindow, context, is_left);
}
