//
// Created by yifei on 4/16/18.
//

#include "ImageProvider.h"

void ImageProvider::upload(GLuint texture_id) {

}

ImageProvider::ImageProvider(uint32_t height, uint32_t width, std::string fallback_msg) {
    image = cv::Mat(height, width, CV_8UC3, {0,0,0});
    this->height = height;
    this->width = width;
    cv::putText(image, fallback_msg, {10, height/2}, cv::FONT_HERSHEY_SIMPLEX, 2, {0,255,0}, 2, cv::LINE_AA);
    raw_image_data = image.data;
}
