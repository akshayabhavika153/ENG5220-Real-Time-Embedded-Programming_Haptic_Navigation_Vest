#ifndef SENSORHEALTHMONITOR_HPP
#define SENSORHEALTHMONITOR_HPP

#include <functional>
#include <thread>
#include <atomic>
#include <mutex>
#include <cstdint>
#include "CommonTypes.hpp"

class SensorHealthMonitor {
public:
    using EventCallback = std::function<void(const SystemEvent&)>;

    SensorHealthMonitor();
    ~SensorHealthMonitor();

    void registerEventCallback(EventCallback cb);
    void configureExpectedSensors(bool expectLeft, bool expectAhead, bool expectRight);

    void updateSensorTimestamp(Direction direction, std::uint64_t timestampMs);

    void start();
    void stop();

private:
    void worker();
    bool hasTimedOut(std::uint64_t lastTimestamp, std::uint64_t nowTimestamp,
                     std::uint64_t monitorStartTimestampMs) const;

    EventCallback eventCallback;
    std::thread workerThread;
    std::atomic<bool> running{false};

    std::mutex dataMutex;
    std::uint64_t lastLeftTimestamp;
    std::uint64_t lastAheadTimestamp;
    std::uint64_t lastRightTimestamp;
    std::uint64_t startupTimeoutMs;
    std::uint64_t runtimeTimeoutMs;
    bool monitoringEnabled;
    bool expectLeft;
    bool expectAhead;
    bool expectRight;
    bool sensorErrorSent;
};

#endif