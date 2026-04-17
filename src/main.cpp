#include <thread>
#include <chrono>
#include <atomic>
#include <csignal>
#include <cstdlib>
#include <string>
#include <iostream>
#include "LidarSensor.hpp"
#include "NavigationLogic.hpp"
#include "PriorityManager.hpp"
#include "HapticController.hpp"
#include "AudioController.hpp"
#include "EmergencyController.hpp"
#include "StartupSelfTest.hpp"

namespace {
std::atomic<bool> gKeepRunning{true};

void handleSignal(int) {
    gKeepRunning = false;
}
}

int main() {
    std::signal(SIGINT, handleSignal);
    std::signal(SIGTERM, handleSignal);

    const char* lidarDevEnv = std::getenv("HVEST_LIDAR_DEVICE");
    const std::string lidarDevice =
        (lidarDevEnv && lidarDevEnv[0] != '\0') ? lidarDevEnv : "/dev/serial0";

    StartupSelfTest startupSelfTest(lidarDevice);
    startupSelfTest.run();

    LidarSensor frontLidar(Direction::AHEAD, lidarDevice);
    NavigationLogic navigationLogic;
    PriorityManager priorityManager;
    HapticController hapticController;
    AudioController audioController;
    EmergencyController emergencyController;

    priorityManager.registerNavigationOutputCallback(
        [&hapticController, &audioController](const NavigationCommand& command) {
            hapticController.handleNavigationCommand(command);
            audioController.handleNavigationCommand(command);
        }
    );

    priorityManager.registerEmergencyOutputCallback(
        [&emergencyController]() {
            std::thread emergencyThread([&emergencyController]() {
                emergencyController.handleFallDetected();
            });
            emergencyThread.detach();
        }
    );

    navigationLogic.registerEventCallback(
        [&priorityManager](const SystemEvent& event) {
            priorityManager.handleEvent(event);
        }
    );

    frontLidar.registerDistanceCallback(
        [&navigationLogic](const DistanceReading& reading) {
            navigationLogic.processDistanceReading(reading);
            std::cout << "[LIDAR] " << reading.distanceMeters << " m" << std::endl;
        }
    );

    frontLidar.start();

    while (gKeepRunning) {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    frontLidar.stop();
    return 0;
}
