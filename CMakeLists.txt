## Top-level CMakeLists.txt — Haptic Navigation Vest

cmake_minimum_required(VERSION 3.10)
project(HapticNavigationVest LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable all warnings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# ---- Sensor libraries ----

add_library(lidar_driver src/lidar/lidar_driver.cpp)
target_include_directories(lidar_driver PUBLIC src/lidar)

add_library(imu_driver src/imu/imu_driver.cpp)
target_include_directories(imu_driver PUBLIC src/imu)

# ---- Actuator libraries ----

add_library(haptic_controller src/haptic/haptic_controller.cpp)
target_include_directories(haptic_controller PUBLIC src/haptic)

add_library(audio_controller src/audio/audio_controller.cpp)
target_include_directories(audio_controller PUBLIC src/audio)

# ---- Decision engine ----

add_library(decision_engine src/fusion/decision_engine.cpp)
target_include_directories(decision_engine PUBLIC src/fusion)
target_link_libraries(decision_engine PUBLIC lidar_driver imu_driver haptic_controller audio_controller)

# ---- Main executable ----

add_executable(haptic_vest src/main.cpp)
target_link_libraries(haptic_vest PRIVATE
    lidar_driver
    imu_driver
    haptic_controller
    audio_controller
    decision_engine
    pthread
)

# ---- Tests ----

enable_testing()
add_subdirectory(test)
