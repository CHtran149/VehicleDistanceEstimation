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
    std::unique_lock<std::mutex>
        lock(mutex_);

    if(stopped_)
    {
        return;
    }

    if(queue_.size() >= max_size_)
    {
        queue_.pop();
    }

    queue_.push(
        frame.clone()
    );

    lock.unlock();

    cv_.notify_one();
}

cv::Mat FrameQueue::pop()
{
    std::unique_lock<std::mutex>
        lock(mutex_);

    cv_.wait(
        lock,
        [this]
        {
            return
                !queue_.empty()
                ||
                stopped_;
        }
    );

    if(
        stopped_
        &&
        queue_.empty()
    )
    {
        return cv::Mat();
    }

    cv::Mat frame =
        queue_.front();

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

void FrameQueue::stop()
{
    {
        std::lock_guard<std::mutex>
            lock(mutex_);

        stopped_ = true;
    }

    cv_.notify_all();
}