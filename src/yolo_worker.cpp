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
}

YOLOWorker::~YOLOWorker()
{
    stop();
}

void YOLOWorker::start()
{
    running_ = true;

    thread_ =
        std::thread(
            &YOLOWorker::workerLoop,
            this
        );
}

void YOLOWorker::stop()
{
    running_ = false;

    if(thread_.joinable())
    {
        thread_.join();
    }
}

void YOLOWorker::workerLoop()
{
    while(running_)
    {
        cv::Mat frame =
            frame_queue_.pop();
                        
        DetectionResult result;

        result.frame =
            frame.clone();

        result.detections =
            detector_.detect(frame);

        detection_queue_.push(
            result
        );
    }
}