#pragma once

#include "frame_queue.hpp"
#include "detection_queue.hpp"
#include "yolo_detector.hpp"

#include <thread>
#include <atomic>

class YOLOWorker
{
public:

    YOLOWorker(
        FrameQueue& frame_queue,
        DetectionQueue& detection_queue
    );

    ~YOLOWorker();

    void start();

    void stop();

private:

    void workerLoop();

private:

    FrameQueue& frame_queue_;

    YOLODetector detector_;

    DetectionQueue& detection_queue_;

    std::thread thread_;

    std::atomic<bool> running_;
};