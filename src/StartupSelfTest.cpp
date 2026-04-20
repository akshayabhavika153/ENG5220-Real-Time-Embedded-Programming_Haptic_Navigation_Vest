#include "StartupSelfTest.hpp"
#include <cstdlib>
#include <iostream>
#include <thread>
#include <chrono>
#include <unistd.h>

namespace {
void runIfSet(const char* envName) {
    const char* cmd = std::getenv(envName);
    if (cmd && cmd[0] != '\0') {
        std::system(cmd);
    }
}
}

StartupSelfTest::StartupSelfTest(std::string lidarDevice)
    : lidarDevicePath(std::move(lidarDevice)) {
}

void StartupSelfTest::run() const {
    std::cout << "[STARTUP] Single-hardware mode enabled" << std::endl;
    std::cout << "[STARTUP] Expected hardware: 1 TF Mini S + 1 speaker + 1 vibration motor" << std::endl;

    runIfSet("HVEST_AUDIO_SLOW_CMD");
    std::this_thread::sleep_for(std::chrono::milliseconds(150));
    runIfSet("HVEST_HAPTIC_SINGLE_SLOW_CMD");

    const bool lidarAvailable = (access(lidarDevicePath.c_str(), F_OK) == 0);
    if (lidarAvailable) {
        std::cout << "[STARTUP] LiDAR interface present at " << lidarDevicePath << std::endl;
    } else {
        std::cout << "[STARTUP] LiDAR interface missing at " << lidarDevicePath << std::endl;
        std::cout << "[STARTUP] Check UART enable, wiring, and HVEST_LIDAR_DEVICE" << std::endl;
    }

    std::cout << "[STARTUP] IMU fall detection enabled (90-degree orientation-change trigger)" << std::endl;
    std::cout << "[STARTUP] System ready" << std::endl;
}
