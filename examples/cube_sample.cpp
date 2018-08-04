
#include <iostream>
#include <vtkCylinderSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkProperty.h>
#include <vtkCamera.h>
#include <vtkSmartPointer.h>
#include <memory>
#include <thread>
#include <vtk_glew.h>
#include <GLFW/glfw3.h>
#include <mutex>
#ifdef __WITH_ROS
#include "dvrk_OpenGLViewer/ROSStereoImageProvider.h"
#include <ros/init.h>
#endif
#include "dvrk_OpenGLViewer/shader.h"
#include "dvrk_OpenGLViewer/IImageAligned.h"
#include "dvrk_OpenGLViewer/ImageProvider.h"
#include "dvrk_OpenGLViewer/StereoWindow.h"
#include "dvrk_OpenGLViewer/TextureRenderer.h"
#include "dvrk_OpenGLViewer/VTKRenderProcedure.h"
#include "dvrk_OpenGLViewer/VTKCameraManager.h"
#include "dvrk_OpenGLViewer/StaticStereoImageSource.h"
#include "CubeAxesVTKRenderProcedure.h"
#include "CubeCameraTextureRenderer.h"

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main(int argc, char * argv [])
{

#ifdef __WITH_ROS

    ros::init(argc, argv, "dvrk_OpenGLViewer");
    ros::NodeHandlePtr nh(new ros::NodeHandle());
    ros::AsyncSpinner spinner(2);
    spinner.start();

    std::unique_ptr<ROSStereoImageProvider> stereo_image_provider = std::make_unique<ROSStereoImageProvider>(nh);

#else
    // when using without ROS, use static image source
    std::unique_ptr<StaticStereoImageSource> static_stereo_image_source =
            std::make_unique<StaticStereoImageSource>(100,
                                                      "~/catkin_ws/src/dvrk-OpenGLViewer/image_sets/1/left.png",
                                                      "~/catkin_ws/src/dvrk-OpenGLViewer/image_sets/1/right.png"
                                                      );
#endif
    glfwInit();
    glfwSetErrorCallback(&error_callback);

    GLFWmonitor * left_monitor = nullptr;
    GLFWmonitor * right_monitor = nullptr;
#ifdef __ARCLAB
    int count;
    GLFWmonitor** monitors = glfwGetMonitors(&count);
    for (int i = 0; i < count; i++) {
        std::cerr << " ";
        if (i == 1) {
            left_monitor = monitors[i];
            std::cerr << "L";
        } else if (i == 2) {
            right_monitor = monitors[i];
            std::cerr << "R";
        } else {
            std::cerr << " ";
        }
        std::cerr << " ";
        std::cerr << "Monitor [" << std::to_string(i) << "] : "
                  << glfwGetMonitorName(monitors[i])
                  << std::endl;
    }
#endif

    std::unique_ptr<StereoWindow> stereo_window = std::make_unique<StereoWindow>(left_monitor, right_monitor);

    VTKCameraManager::get()->load_camera_calibration(
#ifdef __ARCLAB
            "/home/arclab/.ros/camera_info/camera_calibration.yaml"
#else
            "camera_info/camera_calibration.yaml"
#endif
    );
    // monocular image provider
    std::shared_ptr<ImageProvider> image_provider_left = std::make_shared<ImageProvider>(1920, 1080);
    std::shared_ptr<ImageProvider> image_provider_right = std::make_shared<ImageProvider>(1920, 1080);


#ifdef __WITH_ROS
    // register to ROS stereo image
    stereo_image_provider->image_provider_left = image_provider_left;
    stereo_image_provider->image_provider_right = image_provider_right;
#else
    static_stereo_image_source->image_provider_left = image_provider_left;
    static_stereo_image_source->image_provider_right = image_provider_right;
#endif

    // add renderer for camera
    std::shared_ptr<CubeCameraTextureRenderer> camera_renderer = std::make_shared<CubeCameraTextureRenderer>();
    camera_renderer->image_provider_left = image_provider_left;
    camera_renderer->image_provider_right = image_provider_right;
    stereo_window->left_procedures.push_back(camera_renderer);
    stereo_window->right_procedures.push_back(camera_renderer);

    // add renderer for vtk overlay scene
    std::shared_ptr<VTKRenderProcedure> demo_vtk_renderer = std::make_shared<CubeAxesVTKRenderProcedure>();
    stereo_window->left_procedures.push_back(demo_vtk_renderer);
    stereo_window->right_procedures.push_back(demo_vtk_renderer);
    // subscribe for image size changes, follow left image
    image_provider_left->image_aligned_subscribers.push_back(demo_vtk_renderer);

    stereo_window->event_loop();
    stereo_window = nullptr;
    glfwTerminate();

    return EXIT_SUCCESS;
}
