#include "depth_estimator.hpp"

#include <iostream>

int main()
{
    DepthEstimator estimator;

    if(
        !estimator.initialize(
            "../models/midas_small.onnx"
        )
    )
    {
        return -1;
    }

    std::cout
        << "DepthEstimator ready"
        << std::endl;

    return 0;
}