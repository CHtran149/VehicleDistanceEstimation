#include "detection_queue.hpp"

DetectionQueue::DetectionQueue(
    size_t max_size
)
    :
    max_size_(max_size),
    stopped_(false)
{
}

void DetectionQueue::push(
    const DetectionResult& result
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

    queue_.push(result);

    lock.unlock();

    cv_.notify_one();
}

DetectionResult DetectionQueue::pop()
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
        return DetectionResult{};
    }

    DetectionResult result =
        queue_.front();

    queue_.pop();

    return result;
}

void DetectionQueue::stop()
{
    {
        std::lock_guard<std::mutex>
            lock(mutex_);

        stopped_ = true;
    }

    cv_.notify_all();
}