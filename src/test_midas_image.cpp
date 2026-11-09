#include "depth_estimator.hpp"

#include <opencv2/opencv.hpp>

#include <iostream>

int main()
{
    const std::string image_path =
        "../images/20261027_143947_001.jpg";

    const std::string model_path =
        "../models/midas_v21_small_256_int8.onnx";

    cv::Mat image =
        cv::imread(image_path);

    if(image.empty())
    {
        std::cerr
            << "Failed to load image: "
            << image_path
            << std::endl;

        return -1;
    }

    DepthEstimator estimator;

    if(
        !estimator.initialize(
            model_path
        )
    )
    {
        std::cerr
            << "Failed to initialize MiDaS"
            << std::endl;

        return -1;
    }

    cv::Mat depth_map =
        estimator.estimate(image);

    if(depth_map.empty())
    {
        std::cerr
            << "Depth map is empty"
            << std::endl;

        return -1;
    }

    std::cout
        << "Depth map size: "
        << depth_map.cols
        << "x"
        << depth_map.rows
        << std::endl;

    double min_val;
    double max_val;

    cv::minMaxLoc(
        depth_map,
        &min_val,
        &max_val
    );

    std::cout
        << "Depth range: "
        << min_val
        << " -> "
        << max_val
        << std::endl;

    cv::Mat normalized;

    cv::normalize(
        depth_map,
        normalized,
        0,
        255,
        cv::NORM_MINMAX,
        CV_8U
    );

    const std::string output_path =
        "../results/depth.png";

    if(
        !cv::imwrite(
            output_path,
            normalized
        )
    )
    {
        std::cerr
            << "Failed to save depth image"
            << std::endl;

        return -1;
    }

    std::cout
        << "Saved: "
        << output_path
        << std::endl;

    return 0;
}