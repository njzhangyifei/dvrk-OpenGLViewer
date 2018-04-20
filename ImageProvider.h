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
    void upload();
    // RGB RGB RGB RGB RGB, row by column
    cv::Mat image;

    bool in_gpu;
    GLuint texture_id;
    uint32_t height;
    uint32_t width;

    void delete_texture();
};


#endif //OPENGLVIEWER_IMAGEPROVIDER_H
