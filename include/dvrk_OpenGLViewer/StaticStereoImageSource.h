//
// Created by Yifei on 5/29/2018.
//

#ifndef DVRK_OPENGLVIEWER_STATICSTEREOIMAGESOURCE_H
#define DVRK_OPENGLVIEWER_STATICSTEREOIMAGESOURCE_H


#include "ImageProvider.h"
#include "SimpleTimer.h"

class StaticStereoImageSource : protected SimpleTimer {
public:
    std::shared_ptr<ImageProvider> image_provider_left;
    std::shared_ptr<ImageProvider> image_provider_right;
    StaticStereoImageSource(int millis, const char *file_left, const char *file_right);

protected:
    void callback();
    cv::Mat left;
    cv::Mat right;
};


#endif //DVRK_OPENGLVIEWER_STATICSTEREOIMAGESOURCE_H
