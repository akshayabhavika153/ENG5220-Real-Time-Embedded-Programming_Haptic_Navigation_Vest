#ifndef AUDIOCONTROLLER_HPP
#define AUDIOCONTROLLER_HPP

#include <chrono>
#include "CommonTypes.hpp"

class AudioController {
public:
    AudioController();

    void handleNavigationCommand(const NavigationCommand& command);

private:
    enum class AudioPattern {
        STOP,
        SLOW,
        FAST,
        CONTINUOUS
    };

    void applyPattern(AudioPattern pattern);
    bool shouldUpdate(AudioPattern pattern) const;

    AudioPattern lastPattern;
    std::chrono::steady_clock::time_point lastCommandTime;
};

#endif
