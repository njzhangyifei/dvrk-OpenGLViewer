//
// Created by Yifei on 4/18/2018.
//

#include <vtk_glew.h>
#include <thread>
#include <iostream>
#include <opencv2/core/mat.hpp>
#include "dvrk_OpenGLViewer/IRenderProcedure.h"
#include "dvrk_OpenGLViewer/StereoWindow.h"
#ifdef __WITH_IMGUI
#include <imgui.h>
#include <imgui_impl_glfw_gl3.h>

#endif

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
    // request resize callback
    _this->resized_L = true;
    _this->resized_R = true;
    if (window == _this->window_L) {
        glfwSetWindowSize(_this->window_R, width, height);
    }
}

StereoWindow::StereoWindow(GLFWmonitor * monitor_left, GLFWmonitor * monitor_right) {
    glfwWindowHint(GLFW_FOCUSED, false);
    glfwWindowHint(GLFW_CONTEXT_RELEASE_BEHAVIOR, GLFW_RELEASE_BEHAVIOR_FLUSH);
#ifdef __ARCLAB
    glfwWindowHint(GLFW_AUTO_ICONIFY, false);
    const GLFWvidmode* mode_left = glfwGetVideoMode(monitor_left);
    const GLFWvidmode* mode_right = glfwGetVideoMode(monitor_left);
    window_L = glfwCreateWindow(mode_left->width, mode_left->height, "Left",  monitor_left, NULL);
    window_R = glfwCreateWindow(mode_left->width, mode_right->height, "Right", monitor_right, window_L);
#else
    window_L = glfwCreateWindow(800, 400, "Left",  monitor_left, NULL);
    window_R = glfwCreateWindow(800, 400, "Right", monitor_right, window_L);
#endif
    if (!monitor_left) {
        glfwSetWindowPos(window_L, 0, 0);
        glfwSetWindowPos(window_R, 0, 450);
    }
    resized_L = true;
    resized_R = true;
    glfwSetWindowUserPointer(window_L, this);
    glfwSetWindowUserPointer(window_R, this);
    glfwSetKeyCallback(window_L, key_callback);
    glfwMakeContextCurrent(window_L);
    glewInit();

    glfwGetFramebufferSize(window_L, &width, &height);

    glfwMakeContextCurrent(window_L);
    glfwSetWindowSizeCallback(window_L, &resize_callback);
    resize_callback(window_L, width, height);
    glfwSwapInterval(1);

    glfwMakeContextCurrent(window_R);
    glfwSetWindowSizeCallback(window_R, &resize_callback);
    resize_callback(window_R, width, height);
    glfwSwapInterval(1);
}

void StereoWindow::render_left() {
//    glClearColor(1, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    for (int i = 0; i < left_procedures.size(); ++i) {
        left_procedures[i]->execute(this, window_L,  true);
    }
}

void StereoWindow::render_right() {
//    glClearColor(0, 0, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    for (int i = 0; i < right_procedures.size(); ++i) {
        right_procedures[i]->execute(this, window_R,  false);
    }
}

void StereoWindow::event_loop() {
    glfwMakeContextCurrent(window_L);

#ifdef __WITH_IMGUI
    ImGui_ImplGlfwGL3_Init(window_L, true);
    ImGui::StyleColorsClassic();
#endif

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
            if (resized_R){
                glViewport(0, 0, width, height);
                for (int i = 0; i < right_procedures.size(); ++i) {
                    right_procedures[i]->resize_callback(this, window_R, false);
                }
                resized_R = false;
            }
            render_right();
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
        if (resized_L){
            glViewport(0, 0, width, height);
            for (int i = 0; i < left_procedures.size(); ++i) {
                left_procedures[i]->resize_callback(this, window_L, true);
            }
            resized_L = false;
        }
        render_left();
        glFlush();

#ifdef __WITH_IMGUI
        ImGui_ImplGlfwGL3_NewFrame();
        {
            ImGui::Begin("test");
            ImGui::End();
        }
        for (int i = 0; i < left_procedures.size(); ++i) {
            left_procedures[i]->imgui_callback(this, window_L, true);
        }
        for (int i = 0; i < right_procedures.size(); ++i) {
            right_procedures[i]->imgui_callback(this, window_R, false);
        }
        ImGui::ShowMetricsWindow();
        ImGui::Render();
#endif
        gl_lock.unlock();
        glfwSwapBuffers(window_L);
    }


    glfwMakeContextCurrent(NULL);
    glfwSetWindowShouldClose(window_R, true);
    glfwSetWindowShouldClose(window_L, true);
    glfwPollEvents();
    t.join();
    glfwDestroyWindow(window_R);
    glfwDestroyWindow(window_L);
}


//void StereoWindow::
