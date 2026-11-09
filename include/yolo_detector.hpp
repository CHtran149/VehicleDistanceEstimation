#pragma once

#include "detection.hpp"

#include <opencv2/opencv.hpp>

#include <onnxruntime_cxx_api.h>

#include <array>
#include <memory>
#include <string>
#include <vector>

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

    std::vector<float> preprocess(
        const cv::Mat& frame
    );

private:

    static constexpr int INPUT_WIDTH = 640;

    static constexpr int INPUT_HEIGHT = 640;

    static constexpr float CONF_THRESHOLD =
        0.25f;

    static constexpr float NMS_THRESHOLD =
        0.45f;

private:

    bool initialized_;

    std::unique_ptr<Ort::Env>
        env_;

    std::unique_ptr<Ort::Session>
        session_;

    std::array<int64_t, 4>
        input_shape_;

    std::string input_name_;

    std::string output_name_;
};
