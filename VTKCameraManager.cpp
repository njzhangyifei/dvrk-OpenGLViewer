//
// Created by yifei on 5/6/18.
//

#include "VTKCameraManager.h"
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
    camera_left = vtkSmartPointer<vtkCamera>::New();
    camera_right = vtkSmartPointer<vtkCamera>::New();
    K_left = std::array<double, 9>({1, 0, 0,
                                    0, 1, 0,
                                    0, 0, 1});
    K_right = K_left;
    setup_camera_intrinsics(camera_left,  K_left[0], K_left[4], K_left[2], K_left[5]);
    setup_camera_intrinsics(camera_right, K_right[0], K_right[4], K_right[2], K_right[5]);
    setup_camera_extrinsics(camera_left, world_to_cam_left);
    setup_camera_extrinsics(camera_right, world_to_cam_right);
}

void VTKCameraManager::setup_camera_intrinsics
        (const vtkSmartPointer<vtkCamera> & cam, double fx, double fy, double px, double py) {
    double viewAngle = 2.0 * atan((image_height / 2.0 ) / fy ) * 180.0 / vtkMath::Pi();
    cam->SetViewAngle(viewAngle);
    double cx = image_width - px;
    double cy = py;
    double win_center_x = cx / ( (image_width-1)/2.0) - 1 ;
    double win_center_y = cy / ( (image_height-1)/2.0) - 1;
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
}

void VTKCameraManager::resize(int width, int height) {
    image_width = width;
    image_height = height;
    setup_camera_intrinsics(camera_left,  K_left[0], K_left[4], K_left[2], K_left[5]);
    setup_camera_intrinsics(camera_right, K_right[0], K_right[4], K_right[2], K_right[5]);
}

void VTKCameraManager::update_camera_intrinsics_left (const std::array<double, 9> & K) {
    if (K != K_left){
        K_left = K;
        setup_camera_intrinsics(camera_left, K_left[0], K_left[4], K_left[2], K_left[5]);
    }
}

void VTKCameraManager::update_camera_intrinsics_right(const std::array<double, 9> & K) {
    if (K != K_right){
        K_right = K;
        setup_camera_intrinsics(camera_right, K_right[0], K_right[4], K_right[2], K_right[5]);
    }
}

