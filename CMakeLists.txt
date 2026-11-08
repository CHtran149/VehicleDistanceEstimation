cmake_minimum_required(VERSION 3.16)

project(VehicleDistanceEstimation)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
    src/camera.cpp
    src/fps_counter.cpp
    src/frame_queue.cpp
    src/detection_queue.cpp
    src/yolo_worker.cpp
)

add_executable(
    test_libcamera_capture
    src/test_libcamera_capture.cpp
)

add_executable(
    test_libcamera_raw_capture
    src/test_libcamera_raw_capture.cpp
)

target_link_libraries(
    VehicleDistanceEstimation
    ${OpenCV_LIBS}
    ${LIBCAMERA_LIBRARIES}
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
