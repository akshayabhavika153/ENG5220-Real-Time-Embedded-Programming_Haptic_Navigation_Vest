#include "BatteryMonitor.hpp"
#include <chrono>
#include <fstream>
#include <cstdlib>

BatteryMonitor::BatteryMonitor() {
    const char* envPath = std::getenv("HVEST_BATTERY_CAPACITY_PATH");
    if (envPath) {
        capacityPath = envPath;
    } else {
        capacityPath = "/sys/class/power_supply/BAT0/capacity";
    }
    simulatedBatteryPercent = 25;
    lastReportedLevel = AlertLevel::NONE;
}

BatteryMonitor::~BatteryMonitor() {
    stop();
}

void BatteryMonitor::registerEventCallback(EventCallback cb) {
    eventCallback = cb;
}

void BatteryMonitor::start() {
    running = true;
    workerThread = std::thread(&BatteryMonitor::worker, this);
}

void BatteryMonitor::stop() {
    running = false;
    if (workerThread.joinable()) {
        workerThread.join();
    }
}

void BatteryMonitor::worker() {
    while (running) {
        const int batteryPercent = readBatteryPercent();

        SystemEvent event;
        AlertLevel currentLevel = AlertLevel::NONE;
        bool shouldSend = false;

        if (batteryPercent < 10) {
            event.type = EventType::LOW_BATTERY;
            event.priority = Priority::MEDIUM;
            event.navCommand.direction = Direction::NONE;
            event.navCommand.level = AlertLevel::CRITICAL;
            currentLevel = AlertLevel::CRITICAL;
            shouldSend = true;
        } else if (batteryPercent < 20) {
            event.type = EventType::LOW_BATTERY;
            event.priority = Priority::LOW;
            event.navCommand.direction = Direction::NONE;
            event.navCommand.level = AlertLevel::FAR;
            currentLevel = AlertLevel::FAR;
            shouldSend = true;
        }

        if (shouldSend && currentLevel != lastReportedLevel && eventCallback) {
            eventCallback(event);
        }

        lastReportedLevel = currentLevel;

        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}

int BatteryMonitor::readBatteryPercent() {
    std::ifstream batteryFile(capacityPath);
    if (batteryFile.is_open()) {
        int percent = 0;
        batteryFile >> percent;
        if (percent >= 0 && percent <= 100) {
            return percent;
        }
    }

    if (simulatedBatteryPercent <= 0) {
        return 5;
    }

    simulatedBatteryPercent -= 5;
    return simulatedBatteryPercent;
}