#include "EmergencyController.hpp"
#include <cstdlib>
#include <iostream>
#include <thread>
#include <chrono>

namespace {
void runIfSet(const char* envName) {
    if (!envName) {
        return;
    }

    const char* cmd = std::getenv(envName);
    if (!cmd || cmd[0] == '\0') {
        return;
    }

    std::system(cmd);
}
}

EmergencyController::EmergencyController() {
}

void EmergencyController::handleFallDetected() {
    std::cout << "[EMERGENCY] Fall detected" << std::endl;

    runIfSet("HVEST_AUDIO_CONTINUOUS_CMD");
    runIfSet("HVEST_HAPTIC_SINGLE_CONT_CMD");
    runIfSet("HVEST_HAPTIC_BOTH_CONT_CMD");

    beepOneSecond();
    beepOneSecond();
    playHelpNeededMessage();
}

void EmergencyController::beepOneSecond() {
    std::cout << "[EMERGENCY] BEEP (1 sec)" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

void EmergencyController::playHelpNeededMessage() {
    std::cout << "[EMERGENCY] Voice: Help needed" << std::endl;
}