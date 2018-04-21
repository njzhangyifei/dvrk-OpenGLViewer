//
// Created by Yifei on 4/18/2018.
//

#include <vtk_glew.h>
#include <thread>
#include <iostream>
#include <opencv2/core/mat.hpp>
#include "IRenderProcedure.h"
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
    GLFWwindow * previous_context = glfwGetCurrentContext();
    glfwMakeContextCurrent(window);
    _this->width = width;
    _this->height = height;
    // Set the viewport size

//    vtk stuff
//    int info[4];
//    glGetIntegerv(GL_VIEWPORT, info);
//    renderWindow->SetPosition(info[0], info[1]);
//    renderWindow->SetSize(info[2], info[3]);
    glfwMakeContextCurrent(previous_context);
    if (window == _this->window_L) {
        glfwSetWindowSize(_this->window_R, width, height);
    }
}

StereoWindow::StereoWindow(GLFWmonitor * monitor_left, GLFWmonitor * monitor_right) {
    window_L = glfwCreateWindow(1920, 1080, "Left", monitor_left, NULL);
    window_R = glfwCreateWindow(1920, 1080, "Right", monitor_right, window_L);
    glfwSetWindowUserPointer(window_L, this);
    glfwSetWindowUserPointer(window_R, this);
    glfwSetKeyCallback(window_L, key_callback);

    glfwGetFramebufferSize(window_L, &width, &height);

    glfwMakeContextCurrent(window_L);
    glfwSetWindowSizeCallback(window_L, &resize_callback);
    resize_callback(window_L, width, height);
    glfwSwapInterval(1);

    glfwMakeContextCurrent(window_R);
    glfwSetWindowSizeCallback(window_R, &resize_callback);
    resize_callback(window_R, width, height);
    glfwSwapInterval(1);


    glfwMakeContextCurrent(window_L);
    glewInit();
}

void StereoWindow::render_left() {
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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    for (int i = 0; i < left_procedures.size(); ++i) {
        left_procedures[i]->execute(this, window_L,  true);
    }
}

void StereoWindow::render_right() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    for (int i = 0; i < right_procedures.size(); ++i) {
        right_procedures[i]->execute(this, window_R,  false);
    }
}

void StereoWindow::event_loop() {
    glfwMakeContextCurrent(window_L);

    // prepare right
    std::thread t([&]() {
        glfwMakeContextCurrent(window_R);
        gl_lock.lock();
        for (int i = 0; i < right_procedures.size(); ++i) {
            right_procedures[i]->setup(this, window_R,  false);
        }
        gl_lock.unlock();
        while (!glfwWindowShouldClose(window_R)) {
            gl_lock.lock();
            glViewport(0, 0, width, height);
            render_left();
            glFlush();
            gl_lock.unlock();

            glfwSwapBuffers(window_R);
        }
    });

    // prepare left
    gl_lock.lock();
    for (int i = 0; i < left_procedures.size(); ++i) {
        left_procedures[i]->setup(this, window_L,  true);
    }
    gl_lock.unlock();
    while (!glfwWindowShouldClose(window_L)){
        gl_lock.lock();
        glfwPollEvents();
        glViewport(0, 0, width, height);
        render_right();
        glFlush();
        gl_lock.unlock();

        glfwSwapBuffers(window_L);
    }
    t.join();
}


//void StereoWindow::
