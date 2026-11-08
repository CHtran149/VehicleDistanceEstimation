#include "yolo_detector.hpp"

#include <iostream>

YOLODetector::YOLODetector()
    :
    initialized_(false)
{
}

bool YOLODetector::initialize(
    const std::string& model_path
)
{
    try
    {
        env_ =
            std::make_unique<Ort::Env>(
                ORT_LOGGING_LEVEL_WARNING,
                "YOLO"
            );

        Ort::SessionOptions options;

        options.SetIntraOpNumThreads(4);

        options.SetGraphOptimizationLevel(
            GraphOptimizationLevel::
            ORT_ENABLE_ALL
        );

        session_ =
            std::make_unique<Ort::Session>(
                *env_,
                model_path.c_str(),
                options
            );

        std::cout
            << "YOLO model loaded successfully"
            << std::endl;

        initialized_ = true;

        return true;
    }
    catch(const std::exception& e)
    {
        std::cerr
            << "Failed to load model: "
            << e.what()
            << std::endl;

        return false;
    }
}

std::vector<Detection>
YOLODetector::detect(
    const cv::Mat& frame
)
{
    std::vector<Detection>
        detections;

    if(!initialized_)
    {
        return detections;
    }

    return detections;
}