cmake_minimum_required(VERSION 3.16)
project(HapticVest LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(vest_app
    src/main.cpp
    src/AudioController.cpp
    src/BatteryMonitor.cpp
    src/EmergencyController.cpp
    src/HapticController.cpp
    src/IMUSensor.cpp
    src/LidarSensor.cpp
    src/NavigationLogic.cpp
    src/PriorityManager.cpp
    src/SensorHealthMonitor.cpp
    src/StartupSelfTest.cpp
    src/SystemAlertController.cpp
)

target_include_directories(vest_app PRIVATE include)

target_compile_options(vest_app PRIVATE -Wall -Wextra -Wpedantic)

target_link_libraries(vest_app PRIVATE pthread)
