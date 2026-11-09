#include "yolo_detector.hpp"

#include <opencv2/opencv.hpp>

#include <iostream>

int main()
{
    const std::string image_path =
        "../images/20261027_143947_001.jpg";

    const std::string model_path =
        "../models/yolov8n.onnx";

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

    YOLODetector detector;

    if(
        !detector.initialize(
            model_path
        )
    )
    {
        std::cerr
            << "Failed to initialize YOLO"
            << std::endl;

        return -1;
    }

    auto detections =
        detector.detect(image);

    std::cout
        << "Detections: "
        << detections.size()
        << std::endl;

    for(const auto& det : detections)
    {
        std::cout
            << "class="
            << det.class_id
            << " conf="
            << det.confidence
            << std::endl;

        cv::rectangle(
            image,
            det.bbox,
            cv::Scalar(
                0,
                255,
                0
            ),
            2
        );

        std::string label =
            std::to_string(
                det.class_id
            )
            +
            " "
            +
            cv::format(
                "%.2f",
                det.confidence
            );

        cv::putText(
            image,
            label,
            cv::Point(
                det.bbox.x,
                det.bbox.y - 5
            ),
            cv::FONT_HERSHEY_SIMPLEX,
            0.5,
            cv::Scalar(
                0,
                255,
                0
            ),
            2
        );
    }

    const std::string result_path =
        "../results/result.jpg";

    cv::imwrite(
        result_path,
        image
    );

    std::cout
        << "Saved: "
        << result_path
        << std::endl;

    return 0;
}