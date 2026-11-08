#include "camera.hpp"
#include "frame_queue.hpp"

#include "detection.hpp"
#include "detection_queue.hpp"
#include "yolo_worker.hpp"

#include <iostream>

int main()
{
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

    while(true)
    {
        DetectionResult result =
            detection_queue.pop();

        result_count++;

        if(result_count % 30 == 0)
        {
            std::cout
                << "[Main] DetectionResult "
                << result.frame.cols
                << "x"
                << result.frame.rows
                << " count="
                << result_count
                << std::endl;
        }
    }

    yolo_worker.stop();

    camera.stop();

    return 0;
}