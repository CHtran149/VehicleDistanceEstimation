#include "camera.hpp"

#include <opencv2/opencv.hpp>

#include <sys/mman.h>
#include <unistd.h>
#include <iostream>

Camera::Camera(
    FrameQueue& frame_queue
)
    :
    frame_queue_(frame_queue),
    running_(false),
    stream_(nullptr),
    frame_width_(0),
    frame_height_(0),
    frame_stride_(0)
{
}

Camera::~Camera()
{
    stop();
}

bool Camera::initialize()
{
    camera_manager_ =
        std::make_unique<
            libcamera::CameraManager>();

    if(camera_manager_->start())
        return false;

    if(camera_manager_->cameras().empty())
        return false;

    camera_ =
        camera_manager_->cameras()[0];

    if(camera_->acquire())
        return false;

    configuration_ =
        camera_->generateConfiguration(
        {
            libcamera::StreamRole::Viewfinder
        });

    if(!configuration_)
        return false;

    auto& cfg =
        configuration_->at(0);

    cfg.size.width = 640;
    cfg.size.height = 480;

    configuration_->validate();

    if(camera_->configure(
        configuration_.get()))
        return false;

    stream_ = cfg.stream();

    frame_width_ =
        cfg.size.width;

    frame_height_ =
        cfg.size.height;

    frame_stride_ =
        cfg.stride;

    std::cout
        << "Frame size: "
        << frame_width_
        << "x"
        << frame_height_
        << std::endl;

    std::cout
        << "Stride: "
        << frame_stride_
        << std::endl;

    allocator_ =
        std::make_unique<
            libcamera::FrameBufferAllocator>(
                camera_);

    if(allocator_->allocate(stream_) < 0)
        return false;

    const auto& buffers =
        allocator_->buffers(stream_);

    for(const auto& buffer : buffers)
    {
        auto request =
            camera_->createRequest();

        if(!request)
            continue;

        request->addBuffer(
            stream_,
            buffer.get());

        requests_.push_back(
            std::move(request));
    }

    camera_->requestCompleted.connect(
        this,
        &Camera::requestComplete);

    return true;
}

bool Camera::start()
{
    running_ = true;

    if(camera_->start())
        return false;

    for(auto& request : requests_)
    {
        camera_->queueRequest(
            request.get());
    }

    return true;
}

void Camera::stop()
{
    if(!running_)
    {
        return;
    }

    running_ = false;

    if(camera_)
    {
        camera_->requestCompleted
            .disconnect(
                this,
                &Camera::requestComplete
            );

        camera_->stop();

        camera_->release();
    }

    if(camera_manager_)
    {
        camera_manager_->stop();
    }

    std::cout
        << "Camera stopped"
        << std::endl;
}

void Camera::requestComplete(
    libcamera::Request* request
)
{
    if(!running_)
    {
        return;
    }

    if(request->status() !=
       libcamera::Request::RequestComplete)
    {
        return;
    }

    const auto& buffers =
        request->buffers();

    if(buffers.empty())
    {
        return;
    }

    libcamera::FrameBuffer* buffer =
        buffers.begin()->second;

    const auto& plane =
        buffer->planes()[0];

    int fd =
        plane.fd.get();

    void* memory =
        mmap(
            nullptr,
            plane.length,
            PROT_READ,
            MAP_SHARED,
            fd,
            plane.offset
        );

    if(memory == MAP_FAILED)
    {
        std::cerr
            << "mmap failed"
            << std::endl;

        return;
    }

    cv::Mat bgra(
        frame_height_,
        frame_width_,
        CV_8UC4,
        memory,
        frame_stride_
    );

    cv::Mat bgr;

    cv::cvtColor(
        bgra,
        bgr,
        cv::COLOR_BGRA2BGR
    );

    frame_queue_.push(
        bgr.clone()
    );

    munmap(
        memory,
        plane.length
    );

    static uint64_t frame_count = 0;

    frame_count++;

    if(frame_count % 30 == 0)
    {
        std::cout
            << "[Camera] Frame "
            << frame_count
            << std::endl;
    }

    if(!running_)
    {
        return;
    }

    request->reuse(
        libcamera::Request::ReuseBuffers
    );

    if(!running_)
    {
        return;
    }
    
    if(camera_->queueRequest(request))
    {
        std::cerr
            << "queueRequest failed"
            << std::endl;
    }
}