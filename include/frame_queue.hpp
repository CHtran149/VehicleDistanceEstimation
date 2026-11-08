#pragma once

#include <opencv2/opencv.hpp>

#include <queue>
#include <mutex>
#include <condition_variable>

class FrameQueue
{
public:

    explicit FrameQueue(size_t max_size = 5);

    void push(const cv::Mat& frame);

    cv::Mat pop();

    size_t size();

    bool empty();
    
    void clear();

private:

    std::queue<cv::Mat> queue_;

    std::mutex mutex_;

    std::condition_variable condition_;

    size_t max_size_;
};