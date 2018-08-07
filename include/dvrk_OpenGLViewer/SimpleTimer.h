//
// Created by Yifei on 5/29/2018.
//

#ifndef DVRK_OPENGLVIEWER_SIMPLETIMER_H
#define DVRK_OPENGLVIEWER_SIMPLETIMER_H

#include <thread>

class SimpleTimer {
public:
    SimpleTimer(int millis);
    ~SimpleTimer();
    void start();
    void stop();
    virtual void callback();
    void worker();
    int interval;

protected:
    bool shouldStop;
    std::thread worker_thread;
};


#endif //DVRK_OPENGLVIEWER_SIMPLETIMER_H
