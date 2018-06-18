//
// Created by yifei on 5/6/18.
//

#include "VTKCameraManager.h"
#include <opencv2/opencv.hpp>
#include <vtkMath.h>
#include <vtkMatrix4x4.h>
#include <vtkTransform.h>
#include <glm/glm.hpp>
#include <cmath>

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
    image_width = -1;
    image_height = -1;
    distortion_texture_left = 0;
    distortion_texture_right = 0;
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
//    double viewAngle = 2.0 * atan((image_height / 2.0 ) / fy ) * 180.0 / vtkMath::Pi();
//    double cx = image_width - px;
//    double cy = py;
//    double win_center_x = cx / ( (image_width-1)/2) - 1 ;
//    double win_center_y = cy / ( (image_height-1)/2) - 1;

    double win_center_x =  -2.0 * (px - image_width / 2.0) / image_width;
    double win_center_y =  2.0 * (py - image_height / 2.0) / image_height;

    double viewAngle = 180.0 / vtkMath::Pi() * 2.0 * atan2(image_height/ 2.0, fy);
    cam->SetViewAngle(viewAngle);
    cam->SetWindowCenter(win_center_x, win_center_y);
    vtkTransform * m = vtkTransform::New();
    double aspect = fy/fx;
    m->Identity();
    m->Scale(1.0/aspect, 1.0, 1.0);
    cam->SetUserTransform(m);
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
//    cam->SetClippingRange(20,200);
}

void VTKCameraManager::resize(int width, int height) {
    image_width = width;
    image_height = height;
    setup_camera_intrinsics(camera_left,  K_left);
    setup_camera_intrinsics(camera_right, K_right);
    create_distortion_lookup(&distortion_texture_left, dist_coeff_left, K_left);
    create_distortion_lookup(&distortion_texture_right, dist_coeff_right, K_right);
}

bool VTKCameraManager::load_camera_calibration(const char *yaml_file) {
    cv::FileStorage fs;
    std::cerr << yaml_file << std::endl;
    fs.open(yaml_file, cv::FileStorage::READ);
    if (!fs.isOpened()) {
        std::cerr << "Error opening camera calibration yaml: " << "[" << yaml_file << "]" << std::endl;
        return false;
    }

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

glm::vec2 distort(const glm::vec2 & in,
                  const glm::vec2 & image_size,
                  const cv::Mat & dist_coeff,
                  const cv::Mat & intrinsics)
{
    glm::vec2 camera_focus  = {intrinsics.at<double>(0,0), intrinsics.at<double>(1,1)};
    glm::vec2 camera_center = {intrinsics.at<double>(0,2), intrinsics.at<double>(1,2)};
    glm::vec2 distortion_tangential = {
            dist_coeff.at<double>(0,2),
            dist_coeff.at<double>(0,3)
    };
    glm::vec3 distortion_radial = {
            dist_coeff.at<double>(0,0),
            dist_coeff.at<double>(0,1),
            dist_coeff.at<double>(0,4)
    };

    glm::vec2 image_coord = in * image_size;
    glm::vec2 normalized_center = camera_center / camera_focus;
    glm::vec2 normalized_coord = ((image_coord / camera_focus) - (normalized_center) );
    double r_2 = glm::dot(normalized_coord, normalized_coord);

    double radial_distort = (
        1.0
        + distortion_radial.x * r_2
        + distortion_radial.y * r_2 * r_2
        + distortion_radial.z * r_2 * r_2 * r_2
    );

    glm::vec2 tangential_distort = glm::vec2(
        2.0 * distortion_tangential.x * normalized_coord.x * normalized_coord.y +
            distortion_tangential.y * ((r_2) + 2 * (normalized_coord.x * normalized_coord.x)),
        2.0 * distortion_tangential.y * normalized_coord.x * normalized_coord.y +
            distortion_tangential.x * ((r_2) + 2 * (normalized_coord.y * normalized_coord.y))
    );
    glm::vec2 distorted_normalized_coord = normalized_coord * glm::vec2(radial_distort) + tangential_distort;
    glm::vec2 distorted_coord = distorted_normalized_coord * camera_focus + camera_center;
    glm::vec2 coord = distorted_coord / image_size;
    return coord;
}

glm::vec2 distortion_map_clamped(const std::vector<glm::vec2> & distortion_map,
                                 int width, int height,
                                 int i, int j){
    if (i < 0 || i >= width || j < 0 || j >= height)
    {
        return glm::vec2(-1.0,-1.0);
    }
    glm::vec2 res = distortion_map[j*width+i];
    return res;
}

bool VTKCameraManager::create_distortion_lookup(GLuint * texture_id, const cv::Mat & dist_coeff, const cv::Mat & intrinsics) {
    int pixel_count = image_width * image_height;
    if (pixel_count <= 0) return false;
    std::vector<glm::vec2> distortionMap;
    distortionMap.resize((size_t)pixel_count);
    for (size_t i = 0; i < distortionMap.size(); ++i) {
        distortionMap[i] = glm::vec2(-1.0, -1.0);
    }
    // gazebo
    double u_inc = 1.0 / image_width;
    double v_inc = 1.0 / image_height;

    glm::vec2 image_size = {(double)image_width, (double)image_height};

    // obtain bounds of the distorted image points.
    glm::vec2 boundA = distort(glm::vec2(0, 0), image_size, dist_coeff, intrinsics);
    glm::vec2 boundB = distort(glm::vec2(1, 1), image_size, dist_coeff, intrinsics);

    for (unsigned int i = 0; i < image_height; ++i)
    {
        double v = i*v_inc;
        for (unsigned int j = 0; j < image_width; ++j)
        {
            double u = j*u_inc;
            glm::vec2 uv = {u, v};
            glm::vec2 out = distort(uv, image_size, dist_coeff, intrinsics);

            auto idxU = static_cast<int>(out.x * image_width);
            auto idxV = static_cast<int>(out.y * image_height);

            if (out.x < 0 || out.x > 1.0 || out.y < 0 || out.y > 1.0) {
                continue;
            }
            if (idxU < 0 || idxV < 0 || idxU >= image_width || idxV >= image_height) {
                continue;
            }

            auto mapIdx = static_cast<unsigned int>(idxV * image_width + idxU);
            distortionMap[mapIdx] = uv;
        }
    }


    cv::Mat filled(1080, 1920, CV_8UC1);
    std::vector<glm::vec3> texture_data;
    texture_data.resize(distortionMap.size());
    float *pDest = &texture_data[0][0];
    for (unsigned int i = 0; i < image_height; ++i)
    {
        for (unsigned int j = 0; j < image_width; ++j)
        {
            glm::vec2 vec = distortionMap[i*image_width+j];

            if (vec.x < 0 && vec.y < 0)
            {
                glm::vec2 left = distortion_map_clamped(distortionMap, image_width, image_height, j-1, i);
                glm::vec2 right = distortion_map_clamped(distortionMap, image_width, image_height, j+1, i);
                glm::vec2 bottom = distortion_map_clamped(distortionMap, image_width, image_height, j, i+1);
                glm::vec2 top = distortion_map_clamped(distortionMap, image_width, image_height, j, i-1);
                glm::vec2 topLeft = distortion_map_clamped(distortionMap, image_width, image_height, j-1, i-1);
                glm::vec2 topRight = distortion_map_clamped(distortionMap, image_width, image_height, j+1, i-1);
                glm::vec2 bottomLeft = distortion_map_clamped(distortionMap, image_width, image_height, j-1, i+1);
                glm::vec2 bottomRight = distortion_map_clamped(distortionMap, image_width, image_height, j+1, i+1);

                glm::vec2 interpolated = glm::vec2(0.0);
                double divisor = 0;
                if (right.x > 0)
                {
                    divisor++;
                    interpolated += right;
                }
                if (left.x > 0)
                {
                    divisor++;
                    interpolated += left;
                }
                if (top.x > 0)
                {
                    divisor++;
                    interpolated += top;
                }
                if (bottom.x > 0)
                {
                    divisor++;
                    interpolated += bottom;
                }

                if (bottomRight.x > 0)
                {
                    divisor += 0.707;
                    interpolated += bottomRight * glm::vec2(0.707);
                }
                if (bottomLeft.x > 0)
                {
                    divisor += 0.707;
                    interpolated += bottomLeft * glm::vec2(0.707);
                }
                if (topRight.x > 0)
                {
                    divisor += 0.707;
                    interpolated += topRight * glm::vec2(0.707);
                }
                if (topLeft.x > 0)
                {
                    divisor += 0.707;
                    interpolated += topLeft * glm::vec2(0.707);
                }

                float invalid = 0;
                if (divisor > 0.5)
                {
                    interpolated /= divisor;
                    interpolated = glm::clamp(interpolated, 0.0f, 1.0f);
                    invalid = 0;
                } else {
                    interpolated = glm::vec2(-1.0f);
                    invalid = -1;
                }
                *pDest++ = interpolated.x;
                *pDest++ = interpolated.y;
                *pDest++ = invalid;
            } else {
                *pDest++ = vec.x;
                *pDest++ = vec.y;
                *pDest++ = 0;
            }
        }
    }

    if (texture_id) {
        GLuint id = *texture_id;
        *texture_id = 0;
        glDeleteTextures(1, &id);
    }

    glGenTextures(1, texture_id);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, *texture_id);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, // pyramid level
                 GL_RGB32F, image_width, image_height,
                 0,  // border
                 GL_RGB, GL_FLOAT,
                 &texture_data[0][0]);
    glBindTexture(GL_TEXTURE_2D, 0);

    return true;
}


