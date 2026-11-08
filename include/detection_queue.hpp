#pragma once

#include "detection.hpp"

#include <queue>
#include <mutex>
#include <condition_variable>

class DetectionQueue
{
public:

    explicit DetectionQueue(
        size_t max_size = 5
    );

    void push(
        const DetectionResult& result
    );

    DetectionResult pop();

    size_t size();

private:

    std::queue<DetectionResult> queue_;

    std::mutex mutex_;

    std::condition_variable condition_;

    size_t max_size_;
};