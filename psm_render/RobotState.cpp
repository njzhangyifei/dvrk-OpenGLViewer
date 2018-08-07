//
// CreatED BY YIfei on 2/3/18.
//

#include "RobotState.h"
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/core/eigen.hpp>
#include <iostream>

PsmState::PsmState() {
    bHe.setIdentity();
    bHj4.setIdentity();
    bHj5.setIdentity();
    bHeL.setIdentity();
    bHeR.setIdentity();
    eHeL.setIdentity();
    eHeR.setIdentity();
    bHc.setIdentity();
    bHc_corr.setIdentity();
}

void PsmState::update_jaw() {
    jaw = jaw < 0.0 ? 0.0 : jaw;
    eHeL = Eigen::MatrixXd::Identity(4, 4);
    eHeR = Eigen::MatrixXd::Identity(4, 4);
    eHeL(1, 1) = cos(jaw/2); eHeL(1, 2) = -sin(jaw/2);
    eHeL(2, 1) = sin(jaw/2); eHeL(2, 2) = cos(jaw/2);

    eHeR(1, 1) = cos(jaw/2); eHeR(1, 2) = sin(jaw/2);
    eHeR(2, 1) = -sin(jaw/2); eHeR(2, 2) = cos(jaw/2);

    bHeL = bHe * eHeL;
    bHeR = bHe * eHeR;
}

PsmState::PsmState(const PsmState &c) :
    bHe(c.bHe), bHj4(c.bHj4), bHj5(c.bHj5),
    bHeL(c.bHeL), bHeR(c.bHeR), eHeL(c.eHeL),
    eHeR(c.eHeR), bHc(c.bHc), bHc_corr(c.bHc_corr)
{
}

PsmState & PsmState::operator = (const PsmState & c) {
    bHe  = c.bHe;
    bHj4 = c.bHj4;
    bHj5 = c.bHj5;
    bHeL = c.bHeL;
    bHeR = c.bHeR;
    eHeL = c.eHeL;
    eHeR = c.eHeR;
    bHc  = c.bHc;
    bHc_corr = c.bHc_corr;

    return *this;
}

RobotState::RobotState() {
    b1Hb2.setIdentity();
    b1Hb2_corr.setIdentity();
    psm1 = PsmState();
    psm2 = PsmState();
}

RobotState::RobotState(const RobotState &r) :
        psm1(r.psm1), psm2(r.psm2), b1Hb2(r.b1Hb2), b1Hb2_corr(r.b1Hb2_corr) {
}

RobotState & RobotState::operator = (const RobotState & r) {
    psm1 = r.psm1;
    psm2 = r.psm2;

    b1Hb2 = r.b1Hb2;
    b1Hb2_corr = r.b1Hb2_corr;
    return *this;
}

void RobotState::update_handeye(cv::Mat bHc_psm1, cv::Mat bHc_psm2){
    if(!bHc_psm1.empty())
    {
        std::cout << "PSM1 Handeye" << std::endl;
        std::cout <<  bHc_psm1 << std::endl;
        cv::cv2eigen(bHc_psm1, psm1.bHc);
        cv::cv2eigen(bHc_psm1, psm1.bHc_corr);
    }

    if(!bHc_psm2.empty())
    {
        std::cout << "PSM2 Handeye" << std::endl;
        std::cout <<  bHc_psm2 << std::endl;
        cv::cv2eigen(bHc_psm2, psm2.bHc);
        cv::cv2eigen(bHc_psm2, psm2.bHc_corr);
        cv::Mat b1Hb2_cv = bHc_psm1 * bHc_psm2.inv();
        cv::cv2eigen(b1Hb2_cv, b1Hb2);
        cv::cv2eigen(b1Hb2_cv, b1Hb2_corr);
    }
}

void RobotState::load_handeye(std::string handeye_file) {
    cv::FileStorage fs;
    cv::Mat bHc_psm1, bHc_psm2;
    fs.open(handeye_file, cv::FileStorage::READ);
    fs["bHc_psm1"] >> bHc_psm1;
    fs["bHc_psm2"] >> bHc_psm2;
    if(!bHc_psm1.empty())
    {
        std::cout << "PSM1 Handeye" << std::endl;
        std::cout <<  bHc_psm1 << std::endl;
        cv::cv2eigen(bHc_psm1, psm1.bHc);
        cv::cv2eigen(bHc_psm1, psm1.bHc_corr);
    }
    if(!bHc_psm2.empty())
    {
        std::cout << "PSM2 Handeye" << std::endl;
        std::cout <<  bHc_psm2 << std::endl;
        cv::cv2eigen(bHc_psm2, psm2.bHc);
        cv::cv2eigen(bHc_psm2, psm2.bHc_corr);
        cv::Mat b1Hb2_cv = bHc_psm1 * bHc_psm2.inv();
        cv::cv2eigen(b1Hb2_cv, b1Hb2);
        cv::cv2eigen(b1Hb2_cv, b1Hb2_corr);
    }
    fs.release();
}
