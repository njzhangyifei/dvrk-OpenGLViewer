//
// Created by yifei on 5/6/18.
//

#include "VTKCameraManager.h"

VTKCameraManager * VTKCameraManager::instance = nullptr;

VTKCameraManager * VTKCameraManager::get_instance() {
    if (!instance) {
        instance = new VTKCameraManager();
    }
    return instance;
}

void VTKCameraManager::finalize() {
    delete instance;
    instance = nullptr;
}

VTKCameraManager::~VTKCameraManager() {
    camera_left = nullptr;
    camera_right = nullptr;
}

VTKCameraManager::VTKCameraManager() {
    camera_left = vtkSmartPointer<vtkCamera>::New();
    camera_right = vtkSmartPointer<vtkCamera>::New();
}

void VTKCameraManager::resize(int width, int height) {
    image_width = width;
    image_height = height;
}

