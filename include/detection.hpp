#pragma once

#include <opencv2/opencv.hpp>

#include <vector>

struct Detection
{
    int class_id;

    float confidence;

    cv::Rect bbox;
    
    float depth;
    
    float distance_m;
};

struct DetectionResult
{
    cv::Mat frame;

    std::vector<Detection> detections;
};