#include "yolo_detector.hpp"

#include <opencv2/dnn.hpp>

#include <iostream>

YOLODetector::YOLODetector()
    :
    initialized_(false)
{
}

bool YOLODetector::initialize(
    const std::string& model_path
)
{
    try
    {
        env_ =
            std::make_unique<Ort::Env>(
                ORT_LOGGING_LEVEL_WARNING,
                "YOLO"
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
            << "YOLO model loaded successfully"
            << std::endl;

        std::cout
            << "Input Name : "
            << input_name_
            << std::endl;

        std::cout
            << "Output Name: "
            << output_name_
            << std::endl;

        initialized_ = true;

        return true;
    }
    catch(const std::exception& e)
    {
        std::cerr
            << "Failed to load model: "
            << e.what()
            << std::endl;

        return false;
    }
}

std::vector<Detection>
YOLODetector::detect(
    const cv::Mat& frame
)
{
    std::vector<Detection>
        detections;

    if(
        !initialized_ ||
        frame.empty()
    )
    {
        return detections;
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

    static bool first_run = true;

    if(first_run)
    {
        auto shape =
            output_tensors[0]
            .GetTensorTypeAndShapeInfo()
            .GetShape();

        std::cout
            << "\n=== OUTPUT SHAPE === "
            << std::endl;

        for(auto dim : shape)
        {
            std::cout
                << dim
                << " ";
        }

        std::cout
            << std::endl;

        first_run = false;
    }

    float* output =
        output_tensors[0]
            .GetTensorMutableData<float>();

    const int num_classes = 80;

    const int num_boxes = 8400;

    std::vector<cv::Rect>
        boxes;

    std::vector<float>
        scores;

    std::vector<int>
        class_ids;

    const float scale_x =
        static_cast<float>(
            frame.cols
        ) / INPUT_WIDTH;

    const float scale_y =
        static_cast<float>(
            frame.rows
        ) / INPUT_HEIGHT;

    static float max_score_seen = 0.0f;

    for(
        int i = 0;
        i < num_boxes;
        ++i
    )
    {
        float x =
            output[
                0 * num_boxes + i
            ];

        float y =
            output[
                1 * num_boxes + i
            ];

        float w =
            output[
                2 * num_boxes + i
            ];

        float h =
            output[
                3 * num_boxes + i
            ];

        float best_score =
            0.0f;

        int best_class =
            -1;

        for(
            int c = 0;
            c < num_classes;
            ++c
        )
        {
            float score =
                output[
                    (4 + c) *
                    num_boxes +
                    i
                ];

            if(
                score >
                best_score
            )
            {
                best_score =
                    score;

                best_class =
                    c;
            }

            if(score > max_score_seen)
            {
                max_score_seen = score;
            }
        }

        if(
            best_score <
            CONF_THRESHOLD
        )
        {
            continue;
        }

        float left =
            (x - w * 0.5f)
            * scale_x;

        float top =
            (y - h * 0.5f)
            * scale_y;

        float width =
            w * scale_x;

        float height =
            h * scale_y;

        boxes.emplace_back(
            static_cast<int>(left),
            static_cast<int>(top),
            static_cast<int>(width),
            static_cast<int>(height)
        );

        scores.push_back(
            best_score
        );

        class_ids.push_back(
            best_class
        );
    }

    std::vector<int>
        indices;

    cv::dnn::NMSBoxes(
        boxes,
        scores,
        CONF_THRESHOLD,
        NMS_THRESHOLD,
        indices
    );

    for(
        int idx :
        indices
    )
    {
        Detection det;

        det.class_id =
            class_ids[idx];

        det.confidence =
            scores[idx];

        det.bbox =
            boxes[idx];

        detections.push_back(
            det
        );
    }

    return detections;
}

std::vector<float>
YOLODetector::preprocess(
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
            3 *
            INPUT_WIDTH *
            INPUT_HEIGHT
        );

    for(
        int c = 0;
        c < 3;
        ++c
    )
    {
        for(
            int h = 0;
            h < INPUT_HEIGHT;
            ++h
        )
        {
            for(
                int w = 0;
                w < INPUT_WIDTH;
                ++w
            )
            {
                input_tensor[
                    c *
                    INPUT_HEIGHT *
                    INPUT_WIDTH +

                    h *
                    INPUT_WIDTH +

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