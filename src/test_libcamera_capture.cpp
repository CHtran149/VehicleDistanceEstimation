#include <cstdlib>
#include <iostream>

int main()
{
    std::cout
        << "Capturing image..."
        << std::endl;

    int ret = std::system(
        "rpicam-jpeg "
        "-o test.jpg "
        "--width 640 "
        "--height 480 "
        "--nopreview"
    );

    if(ret != 0)
    {
        std::cerr
            << "Capture failed"
            << std::endl;

        return -1;
    }

    std::cout
        << "Capture success"
        << std::endl;

    std::cout
        << "Saved: test.jpg"
        << std::endl;

    return 0;
}