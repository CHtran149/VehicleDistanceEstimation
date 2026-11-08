#include "frame_queue.hpp"

FrameQueue::FrameQueue(
    size_t max_size
)
    : max_size_(max_size)
{
}

void FrameQueue::push(
    const cv::Mat& frame
)
{
    std::unique_lock<std::mutex> lock(mutex_);

    if(queue_.size() >= max_size_)
    {
        queue_.pop();
    }

    queue_.push(frame.clone());

    condition_.notify_one();
}

cv::Mat FrameQueue::pop()
{
    std::unique_lock<std::mutex> lock(mutex_);

    condition_.wait(
        lock,
        [this]
        {
            return !queue_.empty();
        }
    );

    cv::Mat frame = queue_.front();

    queue_.pop();

    return frame;
}

size_t FrameQueue::size()
{
    std::lock_guard<std::mutex> lock(mutex_);

    return queue_.size();
}

bool FrameQueue::empty()
{
    std::lock_guard<std::mutex> lock(mutex_);

    return queue_.empty();
}

void FrameQueue::clear()
{
    std::lock_guard<std::mutex> lock(mutex_);

    while(!queue_.empty())
    {
        queue_.pop();
    }
}