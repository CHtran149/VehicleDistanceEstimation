#include "yolo_detector.hpp"
#include "depth_estimator.hpp"

#include <opencv2/opencv.hpp>

#include <iostream>

int main()
{
    const std::string image_path =
        "../images/20261027_143947_001.jpg";

    const std::string yolo_model =
        "../models/yolov8n.onnx";

    const std::string midas_model =
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

    YOLODetector detector;

    if(
        !detector.initialize(
            yolo_model
        )
    )
    {
        std::cerr
            << "Failed to initialize YOLO"
            << std::endl;

        return -1;
    }

    DepthEstimator estimator;

    if(
        !estimator.initialize(
            midas_model
        )
    )
    {
        std::cerr
            << "Failed to initialize MiDaS"
            << std::endl;

        return -1;
    }

    auto detections =
        detector.detect(image);

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
        << "Detections: "
        << detections.size()
        << std::endl;

    const float scale_x =
        static_cast<float>(
            depth_map.cols
        ) / image.cols;

    const float scale_y =
        static_cast<float>(
            depth_map.rows
        ) / image.rows;

    for(auto& det : detections)
    {
        cv::Rect depth_bbox(
            static_cast<int>(
                det.bbox.x * scale_x
            ),
            static_cast<int>(
                det.bbox.y * scale_y
            ),
            static_cast<int>(
                det.bbox.width * scale_x
            ),
            static_cast<int>(
                det.bbox.height * scale_y
            )
        );

        det.depth =
            estimator.estimateDepth(
                depth_map,
                depth_bbox
            );

        std::cout
            << "class="
            << det.class_id
            << " conf="
            << det.confidence
            << " depth="
            << det.depth
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
            " d="
            +
            cv::format(
                "%.1f",
                det.depth
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

    cv::imwrite(
        "../results/yolo_midas_result.jpg",
        image
    );

    std::cout
        << "Saved: "
        << "../results/yolo_midas_result.jpg"
        << std::endl;

    return 0;
}