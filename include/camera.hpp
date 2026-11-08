#pragma once

#include "frame_queue.hpp"

#include <libcamera/libcamera.h>
#include <libcamera/framebuffer_allocator.h>

#include <thread>
#include <atomic>
#include <memory>
#include <vector>

class Camera
{
public:

    explicit Camera(
        FrameQueue& frame_queue
    );

    ~Camera();

    bool initialize();

    bool start();

    void stop();

private:

    void requestComplete(
        libcamera::Request* request
    );

private:

    FrameQueue& frame_queue_;

    std::unique_ptr<
        libcamera::CameraManager
    > camera_manager_;

    std::shared_ptr<
        libcamera::Camera
    > camera_;

    std::unique_ptr<
        libcamera::CameraConfiguration
    > configuration_;

    std::unique_ptr<
        libcamera::FrameBufferAllocator
    > allocator_;

    libcamera::Stream* stream_;

    std::vector<
        std::unique_ptr<
            libcamera::Request
        >
    > requests_;

    std::atomic<bool> running_;

    uint32_t frame_width_;

    uint32_t frame_height_;

    uint32_t frame_stride_;
};