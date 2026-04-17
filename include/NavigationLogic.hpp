#ifndef NAVIGATIONLOGIC_HPP
#define NAVIGATIONLOGIC_HPP

#include <functional>
#include <cstdint>
#include "CommonTypes.hpp"

class NavigationLogic {
public:
    using EventCallback = std::function<void(const SystemEvent&)>;

    NavigationLogic();

    void registerEventCallback(EventCallback cb);
    void processDistanceReading(const DistanceReading& reading);

private:
    bool detectFallFromLidar(const DistanceReading& reading);
    AlertLevel distanceToAlertLevel(float distanceMeters) const;
    Direction mapSensorDirection(Direction sensorDirection) const;
    Priority alertLevelToPriority(AlertLevel level) const;

    EventCallback eventCallback;
    bool hasPreviousReading;
    float previousDistanceMeters;
    std::uint64_t previousTimestampMs;
    std::uint64_t lastFallEventTimestampMs;
};

#endif