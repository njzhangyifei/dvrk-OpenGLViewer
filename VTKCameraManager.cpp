//
// Created by yifei on 5/6/18.
//

#include "VTKCameraManager.h"
#include <opencv2/opencv.hpp>
#include <vtkMath.h>

inline bool isEqual(double x, double y)
{
    const double epsilon = 1e-5 /* some small number such as 1e-5 */;
    return std::abs(x - y) <= epsilon * std::abs(x);
    // see Knuth section 4.2.2 pages 217-218
}

VTKCameraManager * VTKCameraManager::instance = nullptr;

VTKCameraManager * VTKCameraManager::get() {
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
    world_to_cam_left  = cv::Mat::eye(4, 4,  CV_64FC1);
    world_to_cam_right = cv::Mat::eye(4, 4, CV_64FC1);
    cam_left_to_cam_right = cv::Mat::eye(4, 4, CV_64FC1);
    camera_left = vtkSmartPointer<vtkCamera>::New();
    camera_right = vtkSmartPointer<vtkCamera>::New();
    K_left = cv::Mat::eye(3, 3, CV_64FC1);
    K_right = K_left.clone();
    setup_camera_intrinsics(camera_left,  K_left);
    setup_camera_intrinsics(camera_right, K_right);
    setup_camera_extrinsics(camera_left, world_to_cam_left);
    setup_camera_extrinsics(camera_right, world_to_cam_right);
}

void VTKCameraManager::setup_camera_intrinsics(const vtkSmartPointer<vtkCamera> & cam, const cv::Mat & K) {
    // fx   0  cx
    //  0  fy  cy
    //  0   0   1
    setup_camera_intrinsics(cam,
                            K.at<double>(0,0), K.at<double>(1,1),
                            K.at<double>(0,2), K.at<double>(1,2));
}

void VTKCameraManager::setup_camera_intrinsics
        (const vtkSmartPointer<vtkCamera> & cam, double fx, double fy, double px, double py) {
    double viewAngle = 2.0 * atan((image_height / 2.0 ) / fy ) * 180.0 / vtkMath::Pi();
    cam->SetViewAngle(viewAngle);
    double cx = image_width - px;
    double cy = py;
    double win_center_x = cx / ( (image_width-1)/2) - 1 ;
    double win_center_y = cy / ( (image_height-1)/2) - 1;
    cam->SetWindowCenter(win_center_x, win_center_y);
}

void VTKCameraManager::setup_camera_extrinsics
        (const vtkSmartPointer<vtkCamera> & cam, const cv::Mat & world_to_cam) {
    cv::Mat m_scaled_mat = cv::Mat::eye(4, 4, CV_64FC1);
    m_scaled_mat.at<double>(1, 1) = -m_scaled_mat.at<double>(1, 1);
    m_scaled_mat.at<double>(2, 2) = -m_scaled_mat.at<double>(2, 2);
    cv::Mat m_scaled_transform = cv::Mat::eye(4, 4, CV_64FC1);
    m_scaled_transform = m_scaled_mat * world_to_cam.inv();
    cv::Mat m_rotation = m_scaled_transform(cv::Rect(0,0,3,3));
    // Normalise row of the rotation matrix
    for (unsigned int i = 0; i < 3; i++)    // for each row in the rotation
    {
        double norm_temp = 0.0;
        for (unsigned int j = 0; j < 3; j++)    // for each element in row
            norm_temp += m_rotation.at<double>(i, j) * m_rotation.at<double>(i, j);

        if (!isEqual(norm_temp, 0.0))
        {
            double scale = 1.0 / sqrt(norm_temp);
            for (unsigned int j = 0; j < 3; j++)    // for each element in row
                m_rotation.at<double>(i, j) = m_rotation.at<double>(i, j) * scale;
        }
    }
    cv::Mat m_rotation_inv = m_rotation.inv();
    cv::Mat m_translation = m_scaled_transform(cv::Rect(3, 0, 1, 3));
    // rotate translation vector by inverse rotation P = P'
    m_translation = m_rotation_inv * m_translation;
//    m_translation = m_translation;
    m_translation *= -1; // save -P'
    // from here proceed as normal
    // focalPoint = P-viewPlaneNormal, viewPlaneNormal is rotation[2]
    cv::Vec3d m_view_plane_normal;
    m_view_plane_normal[0] = m_rotation.at<double>(2, 0);
    m_view_plane_normal[1] = m_rotation.at<double>(2, 1);
    m_view_plane_normal[2] = m_rotation.at<double>(2, 2);

    cam->SetPosition(m_translation.at<double>(0), m_translation.at<double>(1), m_translation.at<double>(2));
    cam->SetFocalPoint(m_translation.at<double>(0) - m_view_plane_normal[0],
                       m_translation.at<double>(1) - m_view_plane_normal[1],
                       m_translation.at<double>(2) - m_view_plane_normal[2]);
    cam->SetViewUp(m_rotation.at<double>(1,0), m_rotation.at<double>(1,1), m_rotation.at<double>(1,2));
    cam->SetClippingRange(20,200);
}

void VTKCameraManager::resize(int width, int height) {
    image_width = width;
    image_height = height;
    setup_camera_intrinsics(camera_left,  K_left);
    setup_camera_intrinsics(camera_right, K_right);
}

bool VTKCameraManager::load_camera_calibration(const char *yaml_file) {
    cv::FileStorage fs;
    std::cerr << yaml_file << std::endl;
    fs.open(yaml_file, cv::FileStorage::READ);
    if (!fs.isOpened()) return false;
    fs["K1"] >> K_left;
    fs["K2"] >> K_right;
    fs["D1"] >> dist_coeff_left;
    fs["D2"] >> dist_coeff_right;
    K_left.at<double>(0,2)--;
    K_left.at<double>(1,2)--;
    K_right.at<double>(0,2)--;
    K_right.at<double>(1,2)--;
    // load extrinsics
    cv::Vec3d T;
    cv::Mat R;
    fs["T"] >> T;
    fs["R"] >> R;
    std::cerr << R << std::endl;
    R.copyTo(cam_left_to_cam_right(cv::Range(0,3),cv::Range(0,3)));
    R = R.t();
    for (int i = 0; i < 3; i++){
        cam_left_to_cam_right.at<double>(i,3) = -T(i);
    }
    world_to_cam_right = world_to_cam_left * cam_left_to_cam_right;
    setup_camera_intrinsics(camera_left, K_left);
    setup_camera_intrinsics(camera_right, K_right);
    setup_camera_extrinsics(camera_left, world_to_cam_left);
    setup_camera_extrinsics(camera_right, world_to_cam_right);
    fs.release();
    return true;
}

std::pair<double, double> VTKCameraManager::get_camera_focus(const cv::Mat & m) {
    return {m.at<double>(0,0), m.at<double>(1,1)};
}

std::pair<double, double> VTKCameraManager::get_camera_center(const cv::Mat & m) {
    return {m.at<double>(0,2), m.at<double>(1,2)};
}


