//
// Created by Yifei on 5/25/2018.
//

#ifndef DVRK_OPENGLVIEWER_VIDEOCAPTUREIMAGESOURCE_H
#define DVRK_OPENGLVIEWER_VIDEOCAPTUREIMAGESOURCE_H

#include "ImageProvider.h"

class VideoCaptureImageProvider {
    std::shared_ptr<ImageProvider> image_provider_left;
    std::shared_ptr<ImageProvider> image_provider_right;
};


#endif //DVRK_OPENGLVIEWER_VIDEOCAPTUREIMAGESOURCE_H
