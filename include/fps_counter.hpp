#pragma once

#include <chrono>

class FPSCounter
{
public:

    void update();

    double getFPS() const;

private:

    std::chrono::steady_clock::time_point
        lastTime =
            std::chrono::steady_clock::now();

    double fps = 0;
};