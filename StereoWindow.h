//
// Created by Yifei on 4/18/2018.
//

#ifndef OPENGLVIEWER_STEREOWINDOW_H
#define OPENGLVIEWER_STEREOWINDOW_H


#include <GLFW/glfw3.h>
#include <mutex>

class StereoWindow {
public:
    StereoWindow(GLFWmonitor * monitor_left, GLFWmonitor * monitor_right);
    GLFWwindow * window_L;
    GLFWwindow * window_R;

    int width;
    int height;

    std::mutex gl_lock;

    void event_loop();
};


#endif //OPENGLVIEWER_STEREOWINDOW_H
