//
// Created by Yifei on 5/29/2018.
//

#include "dvrk_OpenGLViewer/SimpleTimer.h"
#include <iostream>
#include <thread>

SimpleTimer::SimpleTimer(int millis) : interval(millis){
}

SimpleTimer::~SimpleTimer(){
    if (worker_thread.joinable()) {
        shouldStop = true;
        worker_thread.join();
    }
}


void SimpleTimer::start() {
    if (worker_thread.joinable()) {
        shouldStop = true;
        worker_thread.join();
    }
    shouldStop = false;
    worker_thread = std::thread(&SimpleTimer::worker, this);
}


void SimpleTimer::stop() {
    shouldStop = true;
}

void SimpleTimer::callback(){
//    std::cerr << "timer callback run" << std::endl;
}

void SimpleTimer::worker() {
    while (!shouldStop){
        this->callback();
        std::this_thread::sleep_for(std::chrono::milliseconds(interval));
    }
}
