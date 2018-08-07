//
// Created by yifei on 5/6/18.
//

#ifndef DVRK_OPENGLVIEWER_VTKCAMERAMANAGER_H
#define DVRK_OPENGLVIEWER_VTKCAMERAMANAGER_H


#include <vtk_glew.h>
#include <opencv2/opencv.hpp>
#include <vtkCamera.h>

class VTKCameraManager {

private:
    VTKCameraManager();
    ~VTKCameraManager();
    static VTKCameraManager * instance;
    void setup_camera_extrinsics(const vtkSmartPointer<vtkCamera> & cam, const cv::Mat &world_to_cam);
    void setup_camera_intrinsics(const vtkSmartPointer<vtkCamera> & cam, double fx, double fy, double px, double py);
    void setup_camera_intrinsics(const vtkSmartPointer<vtkCamera> &cam, const cv::Mat &K);
    bool create_distortion_lookup(GLuint * distortion, const cv::Mat & dist_coeff, const cv::Mat & intrinsics);

public:
    void finalize();
    static VTKCameraManager * get();

    int image_height;
    int image_width;
    void resize(int width, int height);

    cv::Mat world_to_cam_left;
    cv::Mat world_to_cam_right;
    cv::Mat cam_left_to_cam_right;

    cv::Mat K_left;
    cv::Mat K_right;

    cv::Mat dist_coeff_left;
    cv::Mat dist_coeff_right;

    GLuint distortion_texture_left;
    GLuint distortion_texture_right;

    vtkSmartPointer<vtkCamera> camera_left;
    vtkSmartPointer<vtkCamera> camera_right;

    bool load_camera_calibration(const char * yaml_file = nullptr);
    std::pair<double, double> get_camera_center(const cv::Mat & m);
    std::pair<double, double> get_camera_focus(const cv::Mat & m);
};


#endif //DVRK_OPENGLVIEWER_VTKCAMERAMANAGER_H
