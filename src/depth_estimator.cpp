#include "depth_estimator.hpp"

#include <iostream>

DepthEstimator::DepthEstimator()
    :
    initialized_(false)
{
}

bool DepthEstimator::initialize(
    const std::string& model_path
)
{
    try
    {
        env_ =
            std::make_unique<Ort::Env>(
                ORT_LOGGING_LEVEL_WARNING,
                "MiDaS"
            );

        Ort::SessionOptions options;

        options.SetIntraOpNumThreads(4);

        options.SetGraphOptimizationLevel(
            GraphOptimizationLevel::
            ORT_ENABLE_ALL
        );

        session_ =
            std::make_unique<Ort::Session>(
                *env_,
                model_path.c_str(),
                options
            );

        input_shape_ =
        {
            1,
            3,
            INPUT_HEIGHT,
            INPUT_WIDTH
        };

        Ort::AllocatorWithDefaultOptions
            allocator;

        {
            auto name =
                session_->GetInputNameAllocated(
                    0,
                    allocator
                );

            input_name_ =
                name.get();
        }

        {
            auto name =
                session_->GetOutputNameAllocated(
                    0,
                    allocator
                );

            output_name_ =
                name.get();
        }

        std::cout
            << "MiDaS model loaded successfully"
            << std::endl;

        std::cout
            << "Input Name : "
            << input_name_
            << std::endl;

        std::cout
            << "Output Name: "
            << output_name_
            << std::endl;

        std::cout
            << "Input Shape: ";

        for(auto dim : input_shape_)
        {
            std::cout
                << dim
                << " ";
        }

        std::cout
            << std::endl;

        initialized_ = true;

        return true;
    }
    catch(const std::exception& e)
    {
        std::cerr
            << "Failed to load MiDaS model: "
            << e.what()
            << std::endl;

        return false;
    }
}

cv::Mat DepthEstimator::estimate(
    const cv::Mat& frame
)
{
    cv::Mat depth_map;

    if(
        !initialized_ ||
        frame.empty()
    )
    {
        return depth_map;
    }

    auto input_data =
        preprocess(frame);

    Ort::MemoryInfo memory_info =
        Ort::MemoryInfo::CreateCpu(
            OrtArenaAllocator,
            OrtMemTypeDefault
        );

    Ort::Value input_tensor =
        Ort::Value::CreateTensor<float>(
            memory_info,
            input_data.data(),
            input_data.size(),
            input_shape_.data(),
            input_shape_.size()
        );

    const char* input_names[] =
    {
        input_name_.c_str()
    };

    const char* output_names[] =
    {
        output_name_.c_str()
    };

    auto output_tensors =
        session_->Run(
            Ort::RunOptions{nullptr},
            input_names,
            &input_tensor,
            1,
            output_names,
            1
        );

    float* output =
        output_tensors[0]
            .GetTensorMutableData<float>();

    auto output_shape =
        output_tensors[0]
        .GetTensorTypeAndShapeInfo()
        .GetShape();

    const int height =
        static_cast<int>(
            output_shape[1]
        );

    const int width =
        static_cast<int>(
            output_shape[2]
        );

    depth_map =
        cv::Mat(
            height,
            width,
            CV_32F,
            output
        ).clone();

    static bool first_run = true;
    if(first_run)
    {
        first_run = false;

        std::cout
            << "Depth map: "
            << depth_map.cols
            << "x"
            << depth_map.rows
            << std::endl;

        double min_val;
        double max_val;

        cv::minMaxLoc(
            depth_map,
            &min_val,
            &max_val
        );

        std::cout
            << "Depth range: "
            << min_val
            << " -> "
            << max_val
            << std::endl;
    }

    return depth_map;
}


std::vector<float>
DepthEstimator::preprocess(
    const cv::Mat& frame
)
{
    cv::Mat resized;

    cv::resize(
        frame,
        resized,
        cv::Size(
            INPUT_WIDTH,
            INPUT_HEIGHT
        )
    );

    cv::Mat rgb;

    cv::cvtColor(
        resized,
        rgb,
        cv::COLOR_BGR2RGB
    );

    rgb.convertTo(
        rgb,
        CV_32F,
        1.0f / 255.0f
    );

    std::vector<float>
        input_tensor(
            3 * INPUT_WIDTH * INPUT_HEIGHT
        );

    const int channels = 3;

    const int height = INPUT_HEIGHT;

    const int width = INPUT_WIDTH;

    for(
        int c = 0;
        c < channels;
        ++c
    )
    {
        for(
            int h = 0;
            h < height;
            ++h
        )
        {
            for(
                int w = 0;
                w < width;
                ++w
            )
            {
                input_tensor[
                    c * height * width +
                    h * width +
                    w
                ]
                =
                rgb.at<cv::Vec3f>(
                    h,
                    w
                )[c];
            }
        }
    }

    return input_tensor;
}

float DepthEstimator::estimateDepth(
    const cv::Mat& depth_map,
    const cv::Rect& bbox
)
{
    if(
        depth_map.empty() ||
        bbox.width <= 0 ||
        bbox.height <= 0
    )
    {
        return 0.0f;
    }

    int center_x =
        bbox.x +
        bbox.width / 2;

    int center_y =
        bbox.y +
        bbox.height / 2;

    int roi_width =
        static_cast<int>(
            bbox.width * 0.5f
        );

    int roi_height =
        static_cast<int>(
            bbox.height * 0.5f
        );

    int left =
        std::max(
            0,
            center_x - roi_width / 2
        );

    int top =
        std::max(
            0,
            center_y - roi_height / 2
        );

    int right =
        std::min(
            depth_map.cols,
            left + roi_width
        );

    int bottom =
        std::min(
            depth_map.rows,
            top + roi_height
        );

    if(
        right <= left ||
        bottom <= top
    )
    {
        return 0.0f;
    }

    cv::Rect roi(
        left,
        top,
        right - left,
        bottom - top
    );

    cv::Mat depth_roi =
        depth_map(roi);

    std::vector<float> values;

    values.reserve(
        depth_roi.rows *
        depth_roi.cols
    );

    for(
        int y = 0;
        y < depth_roi.rows;
        ++y
    )
    {
        for(
            int x = 0;
            x < depth_roi.cols;
            ++x
        )
        {
            float depth =
                depth_roi.at<float>(
                    y,
                    x
                );

            if(
                std::isfinite(
                    depth
                )
            )
            {
                values.push_back(
                    depth
                );
            }
        }
    }

    if(values.empty())
    {
        return 0.0f;
    }

    size_t mid =
        values.size() / 2;

    std::nth_element(
        values.begin(),
        values.begin() + mid,
        values.end()
    );

    return values[mid];
}