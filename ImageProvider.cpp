//
// Created by yifei on 4/16/18.
//

#include "ImageProvider.h"


ImageProvider::ImageProvider(uint32_t height, uint32_t width, std::string fallback_msg) {
    image = cv::Mat(height, width, CV_8UC3, {0,0,0});
    in_gpu = false;
    need_upload = true;
    this->height = height;
    this->width = width;
    cv::putText(image, fallback_msg, {10, height/2}, cv::FONT_HERSHEY_SIMPLEX, 2, {0,255,0}, 2, cv::LINE_AA);
}

void ImageProvider::generate_texture() {
    glGenTextures(1, &texture_id);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, // pyramid level
                 GL_RGB, width, height,
                 0,  // border
                 GL_RGB, GL_UNSIGNED_BYTE,
                 image.data);
    glBindTexture(GL_TEXTURE_2D, 0);
    in_gpu = true;
}

void ImageProvider::delete_texture(){
    glDeleteTextures(1, &texture_id);
    in_gpu = false;
}

void ImageProvider::upload() {
    image_lock.lock();
    if (need_upload) {
        if (this->width != image.cols || this->height != image.rows || !in_gpu) {
            if (in_gpu) delete_texture();
            this->height = (uint32_t) std::max(image.rows, 1);
            this->width = (uint32_t) std::max(image.cols, 1);
            generate_texture();
        } else {
            glBindTexture(GL_TEXTURE_2D, texture_id);
            glTexSubImage2D(GL_TEXTURE_2D, 0, // pyramid level
                            0, 0, width, height,
                            GL_RGB, GL_UNSIGNED_BYTE,
                            image.data);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
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

