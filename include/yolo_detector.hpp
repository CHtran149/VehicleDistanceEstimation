#pragma once

#include "detection.hpp"

#include <opencv2/opencv.hpp>

#include <onnxruntime_cxx_api.h>

#include <string>
#include <vector>
#include <memory>

class YOLODetector
{
public:

    YOLODetector();

    bool initialize(
        const std::string& model_path
    );

    std::vector<Detection> detect(
        const cv::Mat& frame
    );

private:

    bool initialized_;

    std::unique_ptr<Ort::Env> env_;

    std::unique_ptr<Ort::Session> session_;
};