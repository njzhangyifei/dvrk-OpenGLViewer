//
// Created by arclab on 4/20/18.
//

#ifndef DVRK_OPENGLVIEWER_CAMERATEXTURERENDERER_H
#define DVRK_OPENGLVIEWER_CAMERATEXTURERENDERER_H

#include <boost/shared_ptr.hpp>
#include "dvrk_OpenGLViewer/TextureRenderer.h"
#include "dvrk_OpenGLViewer/ImageProvider.h"
#include "dvrk_OpenGLViewer/IImageAligned.h"


extern cv::Mat obj;

class CameraTextureRenderer : public TextureRenderer{
public:
    std::shared_ptr<ImageProvider> image_provider_left;
    std::shared_ptr<ImageProvider> image_provider_right;
    void execute(StereoWindow * stereoWindow, GLFWwindow *context, bool is_left) override;
    void resize_callback(StereoWindow *stereoWindow, GLFWwindow *context, bool is_left) override;
};


#endif //DVRK_OPENGLVIEWER_CAMERATEXTURERENDERER_H
