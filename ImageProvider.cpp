//
// Created by yifei on 4/16/18.
//

#include "ImageProvider.h"


ImageProvider::ImageProvider(uint32_t height, uint32_t width, std::string fallback_msg) {
    image = cv::Mat(height, width, CV_8UC3, {0,0,0});
    this->height = height;
    this->width = width;
    cv::putText(image, fallback_msg, {10, height/2}, cv::FONT_HERSHEY_SIMPLEX, 2, {0,255,0}, 2, cv::LINE_AA);
    raw_image_data = image.data;
}

void ImageProvider::generate_texture() {
    glGenTextures(1, &texture_id);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, // pyramid level
                 GL_RGB, image.cols, image.rows,
                 0,  // border
                 GL_RGB, GL_UNSIGNED_BYTE,
                 image.ptr());
}

void ImageProvider::upload() {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, // pyramid level
                 GL_RGB, image.cols, image.rows,
                 0,  // border
                 GL_RGB, GL_UNSIGNED_BYTE,
                 image.ptr());
    glBindTexture(GL_TEXTURE_2D, 0);
}

