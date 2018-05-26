
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
#include "ROSStereoImageProvider.h"
#include <ros/init.h>
#endif
#include "shader.h"
#include "IImageAligned.h"
#include "ImageProvider.h"
#include "StereoWindow.h"
#include "TextureRenderer.h"
#include "CameraTextureRenderer.h"
#include "VTKRenderProcedure.h"
#include "VTKCameraManager.h"

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
//    cv::namedWindow("test", CV_WINDOW_AUTOSIZE);
//    cv::imshow("test", img.image);
//    cv::waitKey(0);

    VTKCameraManager::get()->load_camera_calibration(
            "C:/Users/Yifei/unixhome/develop/arclab/dvrk-OpenGLViewer/camera_info/camera_calibration.yaml"
    );

#ifdef __WITH_ROS
    ros::init(argc, argv, "dvrk_OpenGLViewer");
    ros::NodeHandlePtr nh(new ros::NodeHandle());
    ros::AsyncSpinner spinner(2);
    spinner.start();

    std::unique_ptr<ROSStereoImageProvider> stereo_image_provider = std::make_unique<ROSStereoImageProvider>(nh);
#endif

    // monocular image provider
    std::shared_ptr<ImageProvider> image_provider_left = std::make_shared<ImageProvider>(1920, 1080);
    std::shared_ptr<ImageProvider> image_provider_right = std::make_shared<ImageProvider>(1920, 1080);


#ifdef __WITH_ROS
    // register to ROS stereo image
    stereo_image_provider->image_provider_left = image_provider_left;
    stereo_image_provider->image_provider_right = image_provider_right;
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

    std::shared_ptr<CameraTextureRenderer> camera_renderer = std::make_shared<CameraTextureRenderer>();
    camera_renderer->image_provider_left = image_provider_left;
    camera_renderer->image_provider_right = image_provider_right;
    stereo_window->left_procedures.push_back(camera_renderer);
    stereo_window->right_procedures.push_back(camera_renderer);

    std::shared_ptr<VTKRenderProcedure> vtk_renderer = std::make_shared<VTKRenderProcedure>();
    stereo_window->left_procedures.push_back(vtk_renderer);
    stereo_window->right_procedures.push_back(vtk_renderer);

    image_provider_left->image_aligned_subscribers.push_back(vtk_renderer);


//    cv::namedWindow("test");
//    while (true) {
//        cv::imshow("test", image_provider_left->image);
//        cv::waitKey(1);
//    }

    stereo_window->event_loop();
    stereo_window = nullptr;
    glfwTerminate();

//    GLFWwindow * window_L = glfwCreateWindow(400, 400, "test_l", NULL, NULL);
//    GLFWwindow * window_R = glfwCreateWindow(400, 400, "test_r", NULL, window_L);
//
//
//    glfwSetKeyCallback(window_L, key_callback);
//
//    glfwMakeContextCurrent(window_L);
//    glfwSwapInterval(1);
//    glfwMakeContextCurrent(window_R);
//    glfwSwapInterval(1);
//
//    glfwMakeContextCurrent(window_R);
//    glewInit();
//    auto shadow_map_quad_shader = LoadShaders(SHADOW_MAP_QUAD_V_SHADER_PATH, SHADOW_MAP_QUAD_F_SHADER_PATH);
//    auto shadow_map_quad = std::unique_ptr<ShadowMapQuad>(new ShadowMapQuad({0.0, 1.0}, {1.0, 0.0}));
//    glfwMakeContextCurrent(window_L);
//    shadow_map_quad->load_data();
//    shadow_map_quad->shaderProgram = shadow_map_quad_shader;
//
//
//    std::mutex gl_lock;
//
//    std::cout << "Main Loops Begin" << std::endl;
//    glfwMakeContextCurrent(window_L);
//    std::thread t([&]() {
//        glfwMakeContextCurrent(window_R);
//        while (!glfwWindowShouldClose(window_R)) {
//            gl_lock.lock();
//            int width, height;
//
//            static int i = 0;
//            std::cerr << "uploading" << std::endl;
//            img_l.image = cv::Mat(img_l.height, img_l.width, CV_8UC3, {0,0,0});
//            cv::putText(img_l.image, std::to_string((i++)%100),
//                        {10, height/2},
//                        cv::FONT_HERSHEY_SIMPLEX, 2, {0,255,0}, 2, cv::LINE_AA);
//            auto now = std::chrono::high_resolution_clock::now();
//            img_l.upload();
//            img_r.upload();
//            auto now_ = std::chrono::high_resolution_clock::now();
//            std::cerr << "time: "
//                      << std::chrono::duration_cast<std::chrono::milliseconds>(now_ - now).count()
//                      << std::endl;
//
//            glfwGetFramebufferSize(window_R, &width, &height);
//            glViewport(0, 0, width, height);
//
//            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//            shadow_map_quad->draw(img_r.texture_id);
//            glFlush();
//            gl_lock.unlock();
//
//            glfwSwapBuffers(window_R);
//        }
//    });
//    while (!glfwWindowShouldClose(window_L)){
//        gl_lock.lock();
//        glfwPollEvents();
//        int width, height;
//        glfwGetFramebufferSize(window_L, &width, &height);
//        glViewport(0, 0, width, height);
//
//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//        shadow_map_quad->draw(img_l.texture_id);
//        glFlush();
//        gl_lock.unlock();
//
//        glfwSwapBuffers(window_L);
//    }
//    t.join();
//
//    glfwDestroyWindow(window_L);
//    glfwDestroyWindow(window_R);
//    glfwTerminate();
//    glDeleteProgram(shadow_map_quad_shader);
    return EXIT_SUCCESS;
//    // This creates a polygonal cylinder model with eight circumferential facets
//    // (i.e, in practice an octagonal prism).
//    vtkSmartPointer<vtkCylinderSource> cylinder =
//            vtkSmartPointer<vtkCylinderSource>::New();
//    cylinder->SetResolution(8);
//
//    // The mapper is responsible for pushing the geometry into the graphics library.
//    // It may also do color mapping, if scalars or other attributes are defined.
//    vtkSmartPointer<vtkPolyDataMapper> cylinderMapper =
//            vtkSmartPointer<vtkPolyDataMapper>::New();
//    cylinderMapper->SetInputConnection(cylinder->GetOutputPort());
//
//    // The actor is a grouping mechanism: besides the geometry (mapper), it
//    // also has a property, transformation matrix, and/or texture map.
//    // Here we set its color and rotate it around the X and Y axes.
//    vtkSmartPointer<vtkActor> cylinderActor =
//            vtkSmartPointer<vtkActor>::New();
//    cylinderActor->SetMapper(cylinderMapper);
//    cylinderActor->GetProperty()->SetColor(1.0000, 0.3882, 0.2784);
//    cylinderActor->RotateX(30.0);
//    cylinderActor->RotateY(-45.0);
//
//    vtkSmartPointer<vtkRenderer> renderer =
//            vtkSmartPointer<vtkRenderer>::New();
//    renderer->AddActor(cylinderActor);
//    renderer->SetBackground(0.1, 0.2, 0.4);
//    // Zoom in a little by accessing the camera and invoking its "Zoom" method.
//    renderer->ResetCamera();
//    renderer->GetActiveCamera()->UseOffAxisProjectionOn();
//    renderer->GetActiveCamera()->Zoom(1.5);
//
//    // The render window is the actual GUI window
//    // that appears on the computer screen
//    vtkSmartPointer<vtkRenderWindow> renderWindow =
//            vtkSmartPointer<vtkRenderWindow>::New();
//    renderWindow->SetSize(200, 200);
//    renderWindow->AddRenderer(renderer);
//
//    vtkSmartPointer<vtkRenderer> renderer_R =
//        vtkSmartPointer<vtkRenderer>::New();
//    renderer_R->AddActor(cylinderActor);
//    renderer_R->SetBackground(0.1, 0.2, 0.4);
//    renderer_R->SetActiveCamera(renderer->GetActiveCamera());
//
//    // The render window is the actual GUI window
//    // that appears on the computer screen
//    vtkSmartPointer<vtkRenderWindow> renderWindow_R =
//            vtkSmartPointer<vtkRenderWindow>::New();
//    renderWindow_R->SetSize(200, 200);
//    renderWindow_R->AddRenderer(renderer_R);
//
//    renderWindow->StereoCapableWindowOn();
//    renderWindow->SetStereoTypeToLeft();
//    renderWindow->StereoRenderOn();
//
//    renderWindow_R->StereoCapableWindowOn();
//    renderWindow_R->SetStereoTypeToRight();
//    renderWindow_R->StereoRenderOn();
//
//    // The render window interactor captures mouse events
//    // and will perform appropriate camera or actor manipulation
//    // depending on the nature of the events.
//    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
//            vtkSmartPointer<vtkRenderWindowInteractor>::New();
//    renderWindowInteractor->SetRenderWindow(renderWindow);
//
//     vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor_R =
//            vtkSmartPointer<vtkRenderWindowInteractor>::New();
//    renderWindowInteractor_R->SetRenderWindow(renderWindow_R);
//
//    // This starts the event loop and as a side effect causes an initial render.
////    renderWindow_R->Render();
//    renderWindowInteractor->Start();
//
//    return EXIT_SUCCESS;
}
