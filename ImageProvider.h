//
// Created by yifei on 4/16/18.
//

#ifndef OPENGLVIEWER_IMAGEPROVIDER_H
#define OPENGLVIEWER_IMAGEPROVIDER_H


#include <string>
#include <vtk_glew.h>
#include <opencv2/opencv.hpp>

class ImageProvider {
public:
    ImageProvider(uint32_t height, uint32_t width, std::string fallback_msg = std::string("No Signal"));
    void generate_texture();
    void upload(GLuint texture_id);
    // RGB RGB RGB RGB RGB, row by column
    cv::Mat image;
    uint8_t * raw_image_data;
    uint32_t height;
    uint32_t width;
};


#endif //OPENGLVIEWER_IMAGEPROVIDER_H
