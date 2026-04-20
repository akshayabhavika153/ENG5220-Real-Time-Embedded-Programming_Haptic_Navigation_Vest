#ifndef BATTERYMONITOR_HPP
#define BATTERYMONITOR_HPP

#include <functional>
#include <thread>
#include <atomic>
#include <string>
#include "CommonTypes.hpp"

class BatteryMonitor {
public:
    using EventCallback = std::function<void(const SystemEvent&)>;

    BatteryMonitor();
    ~BatteryMonitor();

    void registerEventCallback(EventCallback cb);

    void start();
    void stop();

private:
    int readBatteryPercent();
    void worker();

    EventCallback eventCallback;
    std::thread workerThread;
    std::atomic<bool> running{false};
    std::string capacityPath;
    int simulatedBatteryPercent;
    AlertLevel lastReportedLevel;
};

#endif