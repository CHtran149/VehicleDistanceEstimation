#pragma once

class DistanceEstimator
{
public:

    DistanceEstimator(
        float a = 4506.5111f,
        float b = -1.1380f
    );

    float estimate(
        float depth
    ) const;

private:

    float a_;

    float b_;
};
