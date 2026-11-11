#include "camera.hpp"
#include "frame_queue.hpp"

#include "detection.hpp"
#include "detection_queue.hpp"
#include "yolo_worker.hpp"

#include <iostream>
#include <atomic>
#include <csignal>

std::atomic<bool> g_running(true);

void signalHandler(int)
{
    g_running = false;
}

int main()
{
    signal(SIGINT, signalHandler);

    FrameQueue frame_queue(5);

    DetectionQueue detection_queue(5);

    Camera camera(frame_queue);

    YOLOWorker yolo_worker(
        frame_queue,
        detection_queue
    );

    if(!camera.initialize())
    {
        std::cerr
            << "Camera initialize failed"
            << std::endl;

        return -1;
    }

    if(!camera.start())
    {
        std::cerr
            << "Camera start failed"
            << std::endl;

        return -1;
    }

    yolo_worker.start();

    while(g_running)
    {
        DetectionResult result =
            detection_queue.pop();

        if(
            !g_running &&
            result.frame.empty()
        )
        {
            break;
        }

        for(const auto& det : result.detections)
        {
            std::cout
                << "class="
                << det.class_id
                << " conf="
                << det.confidence
                << " depth="
                << det.depth
                << " distance="
                << det.distance_m
                << " m"
                << std::endl;
        }
    }

    camera.stop();

    frame_queue.stop();

    detection_queue.stop();

    yolo_worker.stop();

    return 0;
}