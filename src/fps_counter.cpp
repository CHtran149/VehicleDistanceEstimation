#include "fps_counter.hpp"

void FPSCounter::update()
{
    auto now =
        std::chrono::steady_clock::now();

    double dt =
        std::chrono::duration<double>(
            now - lastTime
        ).count();

    fps = 1.0 / dt;

    lastTime = now;
}

double FPSCounter::getFPS() const
{
    return fps;
}