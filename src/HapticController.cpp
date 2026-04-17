#include "HapticController.hpp"
#include <cstdlib>
#include <iostream>

namespace {
bool runCommand(const char* envName) {
    if (!envName) {
        return false;
    }

    const char* cmd = std::getenv(envName);
    if (!cmd || cmd[0] == '\0') {
        return false;
    }

    const int rc = std::system(cmd);
    return rc == 0;
}
}

HapticController::HapticController()
    : lastPattern(OutputPattern::STOP),
      lastCommandTime(std::chrono::steady_clock::now() - std::chrono::seconds(5)) {
}

void HapticController::handleNavigationCommand(const NavigationCommand& command) {
    OutputPattern pattern = OutputPattern::STOP;

    switch (command.level) {
        case AlertLevel::NONE:
            pattern = OutputPattern::STOP;
            break;
        case AlertLevel::FAR:
            pattern = OutputPattern::SLOW;
            break;
        case AlertLevel::NEAR:
            pattern = OutputPattern::FAST;
            break;
        case AlertLevel::CRITICAL:
            pattern = OutputPattern::CONTINUOUS;
            break;
    }

    if (!shouldUpdate(pattern)) {
        return;
    }

    applyPattern(pattern);
    lastPattern = pattern;
    lastCommandTime = std::chrono::steady_clock::now();
}

bool HapticController::shouldUpdate(OutputPattern pattern) const {
    const auto now = std::chrono::steady_clock::now();
    const auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastCommandTime).count();

    if (pattern != lastPattern) {
        return true;
    }

    switch (pattern) {
        case OutputPattern::STOP:
            return false;
        case OutputPattern::SLOW:
            return elapsedMs >= 700;
        case OutputPattern::FAST:
            return elapsedMs >= 350;
        case OutputPattern::CONTINUOUS:
            return elapsedMs >= 500;
    }

    return true;
}

bool HapticController::runPreferredCommand(const char* preferredEnv, const char* fallbackEnv) {
    if (runCommand(preferredEnv)) {
        return true;
    }

    if (fallbackEnv && runCommand(fallbackEnv)) {
        return true;
    }

    return false;
}

void HapticController::applyPattern(OutputPattern pattern) {
    switch (pattern) {
        case OutputPattern::STOP:
            if (runPreferredCommand("HVEST_HAPTIC_SINGLE_STOP_CMD", "HVEST_HAPTIC_STOP_CMD")) {
                return;
            }
            std::cout << "[HAPTIC] Stop" << std::endl;
            return;
        case OutputPattern::SLOW:
            if (runPreferredCommand("HVEST_HAPTIC_SINGLE_SLOW_CMD", "HVEST_HAPTIC_BOTH_SLOW_CMD")) {
                return;
            }
            std::cout << "[HAPTIC] Single motor slow pulse" << std::endl;
            return;
        case OutputPattern::FAST:
            if (runPreferredCommand("HVEST_HAPTIC_SINGLE_FAST_CMD", "HVEST_HAPTIC_BOTH_FAST_CMD")) {
                return;
            }
            std::cout << "[HAPTIC] Single motor fast pulse" << std::endl;
            return;
        case OutputPattern::CONTINUOUS:
            if (runPreferredCommand("HVEST_HAPTIC_SINGLE_CONT_CMD", "HVEST_HAPTIC_BOTH_CONT_CMD")) {
                return;
            }
            std::cout << "[HAPTIC] Single motor continuous vibration" << std::endl;
            return;
    }
}
