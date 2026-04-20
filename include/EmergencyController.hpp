#ifndef EMERGENCYCONTROLLER_HPP
#define EMERGENCYCONTROLLER_HPP

class EmergencyController {
public:
    EmergencyController();

    void handleFallDetected();

private:
    void beepOneSecond();
    void playHelpNeededMessage();
};

#endif
