#include "depth_estimator.hpp"

#include <iostream>

DepthEstimator::DepthEstimator()
    :
    initialized_(false)
{
}

bool DepthEstimator::initialize(
    const std::string& model_path
)
{
    try
    {
        env_ =
            std::make_unique<Ort::Env>(
                ORT_LOGGING_LEVEL_WARNING,
                "MiDaS"
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

        Ort::AllocatorWithDefaultOptions
            allocator;

        {
            auto name =
                session_->GetInputNameAllocated(
                    0,
                    allocator
                );

            input_name_ =
                name.get();
        }

        {
            auto name =
                session_->GetOutputNameAllocated(
                    0,
                    allocator
                );

            output_name_ =
                name.get();
        }

        std::cout
            << "MiDaS model loaded successfully"
            << std::endl;

        std::cout
            << "Input Name : "
            << input_name_
            << std::endl;

        std::cout
            << "Output Name: "
            << output_name_
            << std::endl;

        initialized_ = true;

        return true;
    }
    catch(const std::exception& e)
    {
        std::cerr
            << "Failed to load MiDaS model: "
            << e.what()
            << std::endl;

        return false;
    }
}

cv::Mat DepthEstimator::estimate(
    const cv::Mat& frame
)
{
    cv::Mat depth_map;

    if(
        !initialized_ ||
        frame.empty()
    )
    {
        return depth_map;
    }

    /*
     * TODO:
     *
     * preprocess()
     * create tensor
     * session->Run()
     * convert output tensor
     * return depth map
     */

    return depth_map;
}

cv::Mat DepthEstimator::preprocess(
    const cv::Mat& frame
)
{
    cv::Mat processed;

    /*
     * TODO:
     * resize
     * BGR -> RGB
     * normalize
     */

    return processed;
}