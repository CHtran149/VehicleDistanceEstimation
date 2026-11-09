cmake_minimum_required(VERSION 3.16)

project(VehicleDistanceEstimation)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(
    ONNXRUNTIME_ROOT
    /home/chien149/libs/onnxruntime-linux-aarch64-1.19.2
)

find_package(OpenCV REQUIRED)
find_package(PkgConfig REQUIRED)

pkg_check_modules(
    LIBCAMERA
    REQUIRED
    libcamera
)

include_directories(
    include
    ${OpenCV_INCLUDE_DIRS}
    ${LIBCAMERA_INCLUDE_DIRS}
)

add_executable(
    VehicleDistanceEstimation
    
    src/main.cpp
    src/fps_counter.cpp

    src/frame_queue.cpp
    src/camera.cpp
    
    src/detection_queue.cpp

    src/yolo_worker.cpp
    src/yolo_detector.cpp
)

add_executable(
    test_libcamera_capture
    src/test_libcamera_capture.cpp
)

add_executable(
    test_libcamera_raw_capture
    src/test_libcamera_raw_capture.cpp
)

add_executable(
    test_onnxruntime
    src/test_onnxruntime.cpp
)

add_executable(
    test_yolo_image

    src/test_yolo_image.cpp

    src/yolo_detector.cpp
)

add_executable(
    test_midas
    src/test_midas.cpp
    src/depth_estimator.cpp
)

target_link_libraries(
    VehicleDistanceEstimation

    ${OpenCV_LIBS}

    ${LIBCAMERA_LIBRARIES}

    ${ONNXRUNTIME_ROOT}/lib/libonnxruntime.so
)

target_link_libraries(
    test_libcamera_capture

    ${LIBCAMERA_LIBRARIES}

    ${OpenCV_LIBS}
)

target_link_libraries(
    test_libcamera_raw_capture

    ${LIBCAMERA_LIBRARIES}
    ${OpenCV_LIBS}
)

target_link_libraries(
    test_onnxruntime
    ${ONNXRUNTIME_ROOT}/lib/libonnxruntime.so
)

target_link_libraries(
    test_yolo_image
    ${OpenCV_LIBS}
    ${ONNXRUNTIME_ROOT}/lib/libonnxruntime.so
)

target_link_libraries(
    test_midas

    ${OpenCV_LIBS}

    ${ONNXRUNTIME_ROOT}/lib/libonnxruntime.so
)

target_include_directories(
    test_onnxruntime
    PRIVATE
    ${ONNXRUNTIME_ROOT}/include
)

target_include_directories(
    VehicleDistanceEstimation
    PRIVATE
    ${ONNXRUNTIME_ROOT}/include
)

target_include_directories(
    test_yolo_image
    PRIVATE

    include

    ${ONNXRUNTIME_ROOT}/include
)

target_include_directories(
    test_midas
    PRIVATE

    include

    ${ONNXRUNTIME_ROOT}/include
)