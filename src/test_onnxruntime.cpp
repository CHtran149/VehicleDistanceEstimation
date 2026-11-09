#include <onnxruntime_cxx_api.h>

#include <iostream>
#include <vector>

int main()
{
    try
    {
        Ort::Env env(
            ORT_LOGGING_LEVEL_WARNING,
            "YOLO"
        );

        Ort::SessionOptions options;

        options.SetGraphOptimizationLevel(
            GraphOptimizationLevel::
            ORT_ENABLE_ALL
        );

        std::cout
            << "Before Session"
            << std::endl;

        Ort::Session session(
            env,
            "../models/yolov8n.onnx",
            options
        );

        std::cout
            << "After Session"
            << std::endl;

        std::cout
            << "\n=== Model Loaded ==="
            << std::endl;

        Ort::AllocatorWithDefaultOptions allocator;

        size_t input_count =
            session.GetInputCount();

        size_t output_count =
            session.GetOutputCount();

        std::cout
            << "Input Count : "
            << input_count
            << std::endl;

        std::cout
            << "Output Count: "
            << output_count
            << std::endl;

        for(size_t i = 0;
            i < input_count;
            ++i)
        {
            auto input_name =
                session.GetInputNameAllocated(
                    i,
                    allocator
                );

            auto input_info =
                session.GetInputTypeInfo(i);

            auto tensor_info =
                input_info
                .GetTensorTypeAndShapeInfo();

            auto shape =
                tensor_info.GetShape();

            std::cout
                << "\nInput "
                << i
                << std::endl;

            std::cout
                << "Name : "
                << input_name.get()
                << std::endl;

            std::cout
                << "Shape: ";

            for(auto dim : shape)
            {
                std::cout
                    << dim
                    << " ";
            }

            std::cout
                << std::endl;
        }

        for(size_t i = 0;
            i < output_count;
            ++i)
        {
            auto output_name =
                session.GetOutputNameAllocated(
                    i,
                    allocator
                );

            auto output_info =
                session.GetOutputTypeInfo(i);

            auto tensor_info =
                output_info
                .GetTensorTypeAndShapeInfo();

            auto shape =
                tensor_info.GetShape();

            std::cout
                << "\nOutput "
                << i
                << std::endl;

            std::cout
                << "Name : "
                << output_name.get()
                << std::endl;

            std::cout
                << "Shape: ";

            for(auto dim : shape)
            {
                std::cout
                    << dim
                    << " ";
            }

            std::cout
                << std::endl;
        }
    }
    catch(const Ort::Exception& e)
    {
        std::cerr
            << "ONNX Runtime Error: "
            << e.what()
            << std::endl;

        return -1;
    }
    catch(const std::exception& e)
    {
        std::cerr
            << "Error: "
            << e.what()
            << std::endl;

        return -1;
    }

    return 0;
}