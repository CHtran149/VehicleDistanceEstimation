#include "camera.hpp"
#include "frame_queue.hpp"

#include <opencv2/opencv.hpp>

#include <iostream>
#include <string>

int main()
{
    FrameQueue frame_queue(5);

    Camera camera(frame_queue);

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

    uint64_t frame_count = 0;

    while(true)
    {
        cv::Mat frame =
            frame_queue.pop();

        frame_count++;

        if(frame_count % 30 == 0)
        {
            std::cout
                << "[Consumer] "
                << frame.cols
                << "x"
                << frame.rows
                << " frame="
                << frame_count
                << std::endl;
        }

        if(frame_count % 100 == 0)
        {
            std::string filename =
                "frame_" +
                std::to_string(frame_count) +
                ".jpg";

            cv::imwrite(
                filename,
                frame
            );

            std::cout
                << "Saved "
                << filename
                << std::endl;
        }
    }

    camera.stop();

    return 0;
}