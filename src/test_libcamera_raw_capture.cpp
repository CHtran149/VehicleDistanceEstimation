#include <libcamera/libcamera.h>
#include <libcamera/framebuffer_allocator.h>
#include <opencv2/opencv.hpp>

#include <sys/mman.h>
#include <unistd.h>
#include <iostream>
#include <memory>
#include <vector>
#include <thread>
#include <chrono>

using namespace libcamera;

static bool frame_received = false;
static bool frame_saved = false;

static void saveFrame(
    Request *request
)
{
    if(request->status() !=
       Request::RequestComplete)
    {
        return;
    }

    const Request::BufferMap &buffers =
        request->buffers();

    if(buffers.empty())
    {
        return;
    }

    FrameBuffer *buffer =
        buffers.begin()->second;

    const FrameBuffer::Plane &plane =
        buffer->planes()[0];

    int fd =
        plane.fd.get();

    void *memory =
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
        480,
        640,
        CV_8UC4,
        memory
    );

    cv::Mat bgr;

    cv::cvtColor(
        bgra,
        bgr,
        cv::COLOR_BGRA2BGR
    );

    cv::imwrite(
        "frame.jpg",
        bgr
    );

    munmap(
        memory,
        plane.length
    );

    std::cout
        << "Saved frame.jpg"
        << std::endl;

    frame_saved = true;
}

static void requestCompleteHandler(
    libcamera::Request *request
)
{
    if(request->status() ==
       libcamera::Request::RequestComplete)
    {
        static int frame_counter = 0;

        frame_counter++;

        std::cout
            << "Frame #"
            << frame_counter
            << " received"
            << std::endl;

        frame_received = true;
    }
}

int main()
{
    CameraManager cm;

    int ret = cm.start();

    if(ret)
    {
        std::cerr
            << "CameraManager start failed"
            << std::endl;

        return -1;
    }

    auto cameras = cm.cameras();

    if(cameras.empty())
    {
        std::cerr
            << "No camera found"
            << std::endl;

        return -1;
    }

    auto camera = cameras[0];

    std::cout
        << "Camera detected: "
        << camera->id()
        << std::endl;

    ret = camera->acquire();

    if(ret)
    {
        std::cerr
            << "Acquire failed"
            << std::endl;

        return -1;
    }

    auto config =
        camera->generateConfiguration(
        {
            StreamRole::Viewfinder
        });

    if(!config)
    {
        std::cerr
            << "Configuration failed"
            << std::endl;

        return -1;
    }

    StreamConfiguration &cfg =
        config->at(0);

    cfg.size.width = 640;
    cfg.size.height = 480;

    config->validate();

    ret = camera->configure(
        config.get());

    if(ret)
    {
        std::cerr
            << "Configure failed"
            << std::endl;

        return -1;
    }

    std::cout
        << "Configuration success"
        << std::endl;

    Stream *stream =
        cfg.stream();

    FrameBufferAllocator allocator(
        camera);

    ret =
        allocator.allocate(
            stream);

    if(ret < 0)
    {
        std::cerr
            << "Buffer allocation failed"
            << std::endl;

        return -1;
    }

    const auto &buffers =
        allocator.buffers(stream);

    std::cout
        << "Buffers allocated: "
        << buffers.size()
        << std::endl;

    camera->requestCompleted.connect(
        saveFrame);

    std::vector<
        std::unique_ptr<Request>
    > requests;

    for(const auto &buffer : buffers)
    {
        std::unique_ptr<Request>
            request =
                camera->createRequest();

        if(!request)
        {
            continue;
        }

        request->addBuffer(
            stream,
            buffer.get());

        requests.push_back(
            std::move(request));
    }

    ret = camera->start();

    if(ret)
    {
        std::cerr
            << "Camera start failed"
            << std::endl;

        return -1;
    }

    for(auto &request : requests)
    {
        camera->queueRequest(
            request.get());
    }

    auto start =
        std::chrono::steady_clock::now();

    while(true)
    {
        if(frame_saved)
        {
            break;
        }

        auto now =
            std::chrono::steady_clock::now();

        auto elapsed =
            std::chrono::duration_cast
            <
                std::chrono::seconds
            >
            (
                now - start
            );

        if(elapsed.count() > 5)
        {
            break;
        }

        std::this_thread::sleep_for(
            std::chrono::milliseconds(100));
    }

    camera->stop();

    camera->release();

    cm.stop();

    return 0;
}