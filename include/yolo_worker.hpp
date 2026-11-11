#pragma once

#include "frame_queue.hpp"
#include "detection_queue.hpp"

#include "yolo_detector.hpp"
#include "depth_estimator.hpp"
#include "distance_estimator.hpp"

#include <atomic>
#include <thread>

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

    DetectionQueue& detection_queue_;

    YOLODetector detector_;

    DepthEstimator depth_estimator_;

    DistanceEstimator distance_estimator_;

    std::atomic<bool>
        running_;

    std::thread worker_thread_;
};
