//
// Created by Yifei on 4/18/2018.
//

#include <vtk_glew.h>
#include <thread>
#include <iostream>
#include <opencv2/core/mat.hpp>
#include "StereoWindow.h"

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

static void resize_callback(GLFWwindow* window, int width, int height)
{
#ifdef __APPLE__
    glfwGetFramebufferSize(window, &width, &height); // In case your Mac has a retina display
#endif
    StereoWindow * _this = static_cast<StereoWindow *>(glfwGetWindowUserPointer(window));
    _this->width = width;
    _this->height = height;
    // Set the viewport size

//    vtk stuff
//    int info[4];
//    glGetIntegerv(GL_VIEWPORT, info);
//    renderWindow->SetPosition(info[0], info[1]);
//    renderWindow->SetSize(info[2], info[3]);
}

StereoWindow::StereoWindow(GLFWmonitor * monitor_left, GLFWmonitor * monitor_right) {
    window_L = glfwCreateWindow(400, 400, "Left", monitor_left, NULL);
    window_R = glfwCreateWindow(400, 400, "Right", monitor_right, window_L);
    glfwSetWindowUserPointer(window_L, this);
    glfwSetWindowUserPointer(window_R, this);
    glfwSetKeyCallback(window_L, key_callback);

    glfwMakeContextCurrent(window_L);
    glfwSetWindowSizeCallback(window_L, &resize_callback);
    glfwSwapInterval(1);
    glfwMakeContextCurrent(window_R);
    glfwSetWindowSizeCallback(window_R, &resize_callback);
    glfwSwapInterval(1);

    glfwMakeContextCurrent(window_L);
    glewInit();
}

void StereoWindow::event_loop() {
    glfwMakeContextCurrent(window_L);
    std::thread t([&]() {
        glfwMakeContextCurrent(window_R);
        while (!glfwWindowShouldClose(window_R)) {
            gl_lock.lock();

            glViewport(0, 0, width, height);

            int width, height;

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

//            glfwGetFramebufferSize(window_R, &width, &height);
//            glViewport(0, 0, width, height);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//            shadow_map_quad->draw(img_r.texture_id);
            glFlush();
            gl_lock.unlock();

            glfwSwapBuffers(window_R);
        }
    });
    while (!glfwWindowShouldClose(window_L)){
        gl_lock.lock();
        glfwPollEvents();

        glViewport(0, 0, width, height);

//        int width, height;
//        glfwGetFramebufferSize(window_L, &width, &height);
//        glViewport(0, 0, width, height);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//        shadow_map_quad->draw(img_l.texture_id);
        glFlush();
        gl_lock.unlock();

        glfwSwapBuffers(window_L);
    }
    t.join();
}


//void StereoWindow::
