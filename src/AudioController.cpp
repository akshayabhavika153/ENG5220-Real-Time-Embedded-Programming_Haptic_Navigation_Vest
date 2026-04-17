#include "AudioController.hpp"
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

AudioController::AudioController()
    : lastPattern(AudioPattern::STOP),
      lastCommandTime(std::chrono::steady_clock::now() - std::chrono::seconds(5)) {
}

void AudioController::handleNavigationCommand(const NavigationCommand& command) {
    AudioPattern pattern = AudioPattern::STOP;

    switch (command.level) {
        case AlertLevel::NONE:
            pattern = AudioPattern::STOP;
            break;
        case AlertLevel::FAR:
            pattern = AudioPattern::SLOW;
            break;
        case AlertLevel::NEAR:
            pattern = AudioPattern::FAST;
            break;
        case AlertLevel::CRITICAL:
            pattern = AudioPattern::CONTINUOUS;
            break;
    }

    if (!shouldUpdate(pattern)) {
        return;
    }

    applyPattern(pattern);
    lastPattern = pattern;
    lastCommandTime = std::chrono::steady_clock::now();
}

bool AudioController::shouldUpdate(AudioPattern pattern) const {
    const auto now = std::chrono::steady_clock::now();
    const auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastCommandTime).count();

    if (pattern != lastPattern) {
        return true;
    }

    switch (pattern) {
        case AudioPattern::STOP:
            return false;
        case AudioPattern::SLOW:
            return elapsedMs >= 900;
        case AudioPattern::FAST:
            return elapsedMs >= 450;
        case AudioPattern::CONTINUOUS:
            return elapsedMs >= 700;
    }

    return true;
}

void AudioController::applyPattern(AudioPattern pattern) {
    switch (pattern) {
        case AudioPattern::STOP:
            if (runCommand("HVEST_AUDIO_STOP_CMD")) {
                return;
            }
            std::cout << "[AUDIO] Stop" << std::endl;
            return;
        case AudioPattern::SLOW:
            if (runCommand("HVEST_AUDIO_SLOW_CMD")) {
                return;
            }
            std::cout << "[AUDIO] Slow beep" << std::endl;
            return;
        case AudioPattern::FAST:
            if (runCommand("HVEST_AUDIO_FAST_CMD")) {
                return;
            }
            std::cout << "[AUDIO] Fast beep" << std::endl;
            return;
        case AudioPattern::CONTINUOUS:
            if (runCommand("HVEST_AUDIO_CONTINUOUS_CMD")) {
                return;
            }
            std::cout << "[AUDIO] Continuous alert" << std::endl;
            return;
    }
}
