//
// Created by yifei on 5/6/18.
//

#ifndef DVRK_OPENGLVIEWER_VTKCAMERAMANAGER_H
#define DVRK_OPENGLVIEWER_VTKCAMERAMANAGER_H


#include <opencv2/opencv.hpp>
#include <vtkCamera.h>

class VTKCameraManager {

private:
    VTKCameraManager();
    ~VTKCameraManager();
    static VTKCameraManager * instance;
    void setup_camera_intrinsics(const vtkSmartPointer<vtkCamera> & cam, double fx, double fy, double px, double py);
    void setup_camera_extrinsics(const vtkSmartPointer<vtkCamera> & cam, const cv::Mat &world_to_cam);

public:
    void finalize();
    static VTKCameraManager * get();

    int image_height;
    int image_width;
    void resize(int width, int height);

    cv::Mat world_to_cam_left;
    cv::Mat world_to_cam_right;

    vtkSmartPointer<vtkCamera> camera_left;
    vtkSmartPointer<vtkCamera> camera_right;

    std::array<double, 9>  K_left;
    std::array<double, 9>  K_right;

    void update_camera_intrinsics_left(const std::array<double, 9> &K);

    void update_camera_intrinsics_right(const std::array<double, 9> &K);
};


#endif //DVRK_OPENGLVIEWER_VTKCAMERAMANAGER_H
