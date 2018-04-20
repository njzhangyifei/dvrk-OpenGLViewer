//
// Created by Yifei on 4/18/2018.
//

#ifndef OPENGLVIEWER_STEREOWINDOW_H
#define OPENGLVIEWER_STEREOWINDOW_H


#include <GLFW/glfw3.h>
#include <mutex>
#include <vector>

class IRenderProcedure;

class StereoWindow {
public:
    StereoWindow(GLFWmonitor * monitor_left, GLFWmonitor * monitor_right);
    GLFWwindow * window_L;
    GLFWwindow * window_R;

    int width;
    int height;

    std::mutex gl_lock;

    std::vector<std::shared_ptr<IRenderProcedure>> left_procedures;
    std::vector<std::shared_ptr<IRenderProcedure>> right_procedures;

    void event_loop();

    void render_left();

    void render_right();
};


#endif //OPENGLVIEWER_STEREOWINDOW_H
