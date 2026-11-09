#pragma once

#include <opencv2/opencv.hpp>

#include <onnxruntime_cxx_api.h>

#include <array>
#include <memory>
#include <string>

class DepthEstimator
{
public:

    DepthEstimator();

    bool initialize(
        const std::string& model_path
    );

    cv::Mat estimate(
        const cv::Mat& frame
    );

private:

    cv::Mat preprocess(
        const cv::Mat& frame
    );

private:

    bool initialized_;

    std::unique_ptr<Ort::Env>
        env_;

    std::unique_ptr<Ort::Session>
        session_;

    std::string input_name_;

    std::string output_name_;

    std::array<int64_t, 4>
        input_shape_;
};