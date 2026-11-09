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

    uint64_t result_count = 0;

    while(g_running)
    {
        DetectionResult result =
            detection_queue.pop();

        result_count++;

        if(result_count % 30 == 0)
        {
            std::cout
                << "\nFrame "
                << result_count
                << std::endl;

            std::cout
                << "Detections: "
                << result.detections.size()
                << std::endl;

            for(const auto& det : result.detections)
            {
                std::cout
                    << "class="
                    << det.class_id
                    << " conf="
                    << det.confidence
                    << " depth="
                    << det.depth
                    << std::endl;
            }
        }
    }

    yolo_worker.stop();

    camera.stop();

    return 0;
}