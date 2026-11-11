#include "yolo_worker.hpp"

#include <iostream>

YOLOWorker::YOLOWorker(
    FrameQueue& frame_queue,
    DetectionQueue& detection_queue
)
    :
    frame_queue_(frame_queue),
    detection_queue_(detection_queue),
    running_(false)
{
    detector_.initialize(
        "../models/yolov8n.onnx"
    );

    depth_estimator_.initialize(
        "../models/midas_v21_small_256_int8.onnx"
    );
}

YOLOWorker::~YOLOWorker()
{
    stop();
}

void YOLOWorker::start()
{
    if(running_)
    {
        return;
    }

    running_ = true;

    worker_thread_ =
        std::thread(
            &YOLOWorker::workerLoop,
            this
        );
}

void YOLOWorker::stop()
{
    if(!running_)
    {
        return;
    }

    running_ = false;

    frame_queue_.stop();

    if(worker_thread_.joinable())
    {
        worker_thread_.join();
    }
}

void YOLOWorker::workerLoop()
{
    while(running_)
    {
        cv::Mat frame =
            frame_queue_.pop();

        if(frame.empty())
        {
            continue;
        }

        auto detections =
            detector_.detect(
                frame
            );

        cv::Mat depth_map =
            depth_estimator_.estimate(
                frame
            );

        if(depth_map.empty())
        {
            continue;
        }

        const float scale_x =
            static_cast<float>(
                depth_map.cols
            ) / frame.cols;

        const float scale_y =
            static_cast<float>(
                depth_map.rows
            ) / frame.rows;

        for(auto& det : detections)
        {
            cv::Rect depth_bbox(
                static_cast<int>(
                    det.bbox.x *
                    scale_x
                ),

                static_cast<int>(
                    det.bbox.y *
                    scale_y
                ),

                static_cast<int>(
                    det.bbox.width *
                    scale_x
                ),

                static_cast<int>(
                    det.bbox.height *
                    scale_y
                )
            );

            det.depth =
                depth_estimator_
                .estimateDepth(
                    depth_map,
                    depth_bbox
                );

            det.distance_m =
                distance_estimator_
                .estimate(
                    det.depth
                );
        }

        DetectionResult result;

        result.frame =
            frame.clone();

        result.detections =
            std::move(
                detections
            );

        detection_queue_.push(
            result
        );
    }
    std::cout
        << "YOLOWorker thread exited"
        << std::endl;
}