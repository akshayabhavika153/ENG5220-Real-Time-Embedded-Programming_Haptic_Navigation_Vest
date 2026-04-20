#ifndef SYSTEMALERTCONTROLLER_HPP
#define SYSTEMALERTCONTROLLER_HPP

#include "CommonTypes.hpp"

class SystemAlertController {
public:
    SystemAlertController();

    void handleSystemEvent(const SystemEvent& event);

private:
    void handleSensorError();
    void handleLowBattery(AlertLevel level);
};

#endif