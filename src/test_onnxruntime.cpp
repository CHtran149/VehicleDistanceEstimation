#include <onnxruntime_cxx_api.h>

#include <iostream>

int main()
{
    try
    {
        Ort::Env env(
            ORT_LOGGING_LEVEL_WARNING,
            "test"
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
            << "Model loaded successfully"
            << std::endl;
    }
    catch(const std::exception& e)
    {
        std::cerr
            << e.what()
            << std::endl;
    }
}