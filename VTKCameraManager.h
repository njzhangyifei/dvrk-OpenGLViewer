//
// Created by yifei on 5/6/18.
//

#ifndef DVRK_OPENGLVIEWER_VTKCAMERAMANAGER_H
#define DVRK_OPENGLVIEWER_VTKCAMERAMANAGER_H

#include <vtkCamera.h>

class VTKCameraManager {

private:
    VTKCameraManager();
    ~VTKCameraManager();
    static VTKCameraManager * instance;

public:
    void finalize();
    static VTKCameraManager * get_instance();

    int image_height;
    int image_width;
    void resize(int width, int height);
    vtkSmartPointer<vtkCamera> camera_left;
    vtkSmartPointer<vtkCamera> camera_right;
};


#endif //DVRK_OPENGLVIEWER_VTKCAMERAMANAGER_H
