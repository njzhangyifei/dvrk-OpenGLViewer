//
// Created by yifei on 5/6/18.
//

#ifndef DVRK_OPENGLVIEWER_IIMAGEALIGNED_H
#define DVRK_OPENGLVIEWER_IIMAGEALIGNED_H

#include <atomic>

class IImageAligned {
protected:
    std::mutex image_resize_lock;
    bool image_resized = false;
    int  image_width;
    int  image_height;

public:
    virtual void image_resize_callback(int width, int height){
        {
            std::lock_guard<std::mutex> lk(image_resize_lock);
            image_width = width;
            image_height = height;
            image_resized = true;
        }
    };
};


#endif //DVRK_OPENGLVIEWER_IIMAGEALIGNED_H
