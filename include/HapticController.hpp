#ifndef HAPTICCONTROLLER_HPP
#define HAPTICCONTROLLER_HPP

#include <chrono>
#include "CommonTypes.hpp"

class HapticController {
public:
    HapticController();

    void handleNavigationCommand(const NavigationCommand& command);

private:
    enum class OutputPattern {
        STOP,
        SLOW,
        FAST,
        CONTINUOUS
    };

    void applyPattern(OutputPattern pattern);
    bool shouldUpdate(OutputPattern pattern) const;
    bool runPreferredCommand(const char* preferredEnv, const char* fallbackEnv = nullptr);

    OutputPattern lastPattern;
    std::chrono::steady_clock::time_point lastCommandTime;
};

#endif
