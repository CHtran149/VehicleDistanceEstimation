#pragma once

#include "detection.hpp"

#include <queue>
#include <mutex>
#include <condition_variable>


class DetectionQueue
{
public:

    explicit DetectionQueue(
        size_t max_size
    );

    void push(
        const DetectionResult& result
    );

    DetectionResult pop();

    void stop();

private:

    std::queue<DetectionResult>
        queue_;

    std::mutex
        mutex_;

    std::condition_variable
        cv_;

    size_t
        max_size_;

    bool
        stopped_;
};