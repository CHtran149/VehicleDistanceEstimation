#include "distance_estimator.hpp"

#include <cmath>

DistanceEstimator::DistanceEstimator(
    float a,
    float b
)
    :
    a_(a),
    b_(b)
{
}

float DistanceEstimator::estimate(
    float depth
) const
{
    if(depth <= 0.0f)
    {
        return 0.0f;
    }

    return
        a_ *
        std::pow(
            depth,
            b_
        );
}