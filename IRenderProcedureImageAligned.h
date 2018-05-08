//
// Created by yifei on 5/6/18.
//

#ifndef DVRK_OPENGLVIEWER_IRENDERPROCEDURECAMERAALIGNED_H
#define DVRK_OPENGLVIEWER_IRENDERPROCEDURECAMERAALIGNED_H

#include "StereoWindow.h"
#include "IRenderProcedure.h"

class IRenderProcedureImageAligned : public IRenderProcedure {
public:
    virtual void image_resize_callback(int width, int height, bool is_left){};
};


#endif //DVRK_OPENGLVIEWER_IRENDERPROCEDURECAMERAALIGNED_H
