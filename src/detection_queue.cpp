#include "detection_queue.hpp"

DetectionQueue::DetectionQueue(
    size_t max_size
)
    :
    max_size_(max_size)
{
}

void DetectionQueue::push(
    const DetectionResult& result
)
{
    std::unique_lock<std::mutex>
        lock(mutex_);

    if(queue_.size() >= max_size_)
    {
        queue_.pop();
    }

    queue_.push(result);

    condition_.notify_one();
}

DetectionResult DetectionQueue::pop()
{
    std::unique_lock<std::mutex>
        lock(mutex_);

    condition_.wait(
        lock,
        [this]
        {
            return !queue_.empty();
        });

    DetectionResult result =
        queue_.front();

    queue_.pop();

    return result;
}

size_t DetectionQueue::size()
{
    std::lock_guard<std::mutex>
        lock(mutex_);

    return queue_.size();
}