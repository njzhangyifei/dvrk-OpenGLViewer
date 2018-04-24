//
// Created by yifei on 4/16/18.
//

#ifndef OPENGLVIEWER_IMAGEPROVIDER_H
#define OPENGLVIEWER_IMAGEPROVIDER_H


#include <string>
#include <vtk_glew.h>
#include <opencv2/opencv.hpp>
#include <mutex>

class ImageProvider {
protected:
    std::mutex image_lock;
    bool need_upload;
    void generate_texture();
public:
    ImageProvider(uint32_t height, uint32_t width, std::string fallback_msg = std::string("No Signal"));
    void upload();
    void set_image(const cv::Mat & m);
    // RGB RGB RGB RGB RGB, row by column
    cv::Mat image;

    bool in_gpu;
    GLuint texture_id;
    uint32_t height;
    uint32_t width;

    void delete_texture();
};


#endif //OPENGLVIEWER_IMAGEPROVIDER_H
