//
// Created by yifei on 4/16/18.
//

#include "ImageProvider.h"
#include "VTKCameraManager.h"


ImageProvider::ImageProvider(uint32_t height, uint32_t width, std::string fallback_msg) {
    image = cv::Mat(height, width, CV_8UC3, {0,0,0});
    in_gpu = false;
    need_upload = true;
    this->height = height;
    this->width = width;
    cv::putText(image, fallback_msg, {10, static_cast<int>(height / 2.0)},
                cv::FONT_HERSHEY_SIMPLEX, 2, {0, 255, 0}, 2, cv::LINE_AA);
}

void ImageProvider::generate_texture() {
    glGenTextures(1, &texture_id);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    in_gpu = true;
}

void ImageProvider::upload_texture(){
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, // pyramid level
                 GL_RGB, width, height,
                 0,  // border
                 GL_RGB, GL_UNSIGNED_BYTE,
                 image.data);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void ImageProvider::delete_texture(){
    glDeleteTextures(1, &texture_id);
    in_gpu = false;
}

void ImageProvider::upload() {
    image_lock.lock();
    if (need_upload) {
        if (this->width != image.cols || this->height != image.rows || !in_gpu) {
            this->height = (uint32_t) std::max(image.rows, 1);
            this->width = (uint32_t) std::max(image.cols, 1);
            if (!in_gpu) {
                generate_texture();
            }
            // resize callback
            VTKCameraManager::get()->resize(width, height);
            for (int i = 0; i < image_aligned_subscribers.size(); i++) {
                image_aligned_subscribers[i]->image_resize_callback(width, height);
            }
        }
        upload_texture();
//#ifdef __WITH_ROS
//#ifdef __ARCLAB
//        need_upload = false;
//#endif
//#endif
        need_upload = false;
    }
    image_lock.unlock();
}

void ImageProvider::set_image(const cv::Mat &m) {
    image_lock.lock();
    image = m;
    need_upload = true;
    image_lock.unlock();
}

