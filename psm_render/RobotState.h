//
// Created by yifei on 2/3/18.
//

#ifndef OPENGLRENDERER_ROBOTSTATE_H
#define OPENGLRENDERER_ROBOTSTATE_H

#include <mutex>
#include <Eigen/Geometry>
#include <opencv2/core/eigen.hpp>

class PsmState {
public:
    float jaw;
    Eigen::Matrix<double, 4, 4, Eigen::RowMajor> bHe, bHj4, bHj5;
    Eigen::Matrix<double, 4, 4, Eigen::RowMajor> eHeL, eHeR;
    Eigen::Matrix<double, 4, 4, Eigen::RowMajor> bHeL, bHeR;
    Eigen::Matrix<double, 4, 4, Eigen::RowMajor> bHc;
    Eigen::Matrix<double, 4, 4, Eigen::RowMajor> bHc_corr;

    PsmState();
    PsmState(const PsmState & c);
    PsmState & operator = (const PsmState & c);

    void update_jaw();
};

class PsmStateCv {
public:
    float jaw;
    cv::Mat bHe, bHj4, bHj5;
    cv::Mat eHeL, eHeR;
    cv::Mat bHeL, bHeR;
    cv::Mat bHc, bHc_corr;

    PsmStateCv(const PsmState & state) : jaw(state.jaw) {
        cv::eigen2cv(state.bHe, bHe);
        cv::eigen2cv(state.bHj4, bHj4);
        cv::eigen2cv(state.bHj5, bHj5);
        cv::eigen2cv(state.bHc, bHc);
        cv::eigen2cv(state.bHc_corr, bHc_corr);
        cv::eigen2cv(state.eHeL, eHeL);
        cv::eigen2cv(state.eHeR, eHeR);
        cv::eigen2cv(state.bHeL, bHeL);
        cv::eigen2cv(state.bHeR, bHeR);
    };
};

class RobotState {
public:
    Eigen::Matrix<double, 4, 4, Eigen::RowMajor> b1Hb2;
    Eigen::Matrix<double, 4, 4, Eigen::RowMajor> b1Hb2_corr;
    PsmState psm1;
    PsmState psm2;
    std::mutex data_mutex;

    RobotState();
    RobotState(const RobotState & r);
    RobotState & operator = (const RobotState & c);

    void update_handeye(cv::Mat psm1_bHc, cv::Mat psm2_bHc);
    void load_handeye(std::string handeye_file);
};


#endif //OPENGLRENDERER_ROBOTSTATE_H
