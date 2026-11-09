#include "depth_estimator.hpp"

#include <iostream>

int main()
{
    DepthEstimator estimator;

    if(
        !estimator.initialize(
            "../models/midas_v21_small_256_int8.onnx"
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