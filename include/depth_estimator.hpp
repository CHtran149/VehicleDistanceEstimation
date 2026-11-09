#pragma once

#include <opencv2/opencv.hpp>

#include <onnxruntime_cxx_api.h>

#include <array>
#include <memory>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>

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

    float estimateDepth(
        const cv::Mat& depth_map,
        const cv::Rect& bbox
    );

private:

    std::vector<float> preprocess(
        const cv::Mat& frame
    );

private:

    static constexpr int INPUT_WIDTH =
        256;

    static constexpr int INPUT_HEIGHT =
        256;

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