#include "NavigationLogic.hpp"
#include <iostream>

NavigationLogic::NavigationLogic() {
}

void NavigationLogic::registerEventCallback(EventCallback cb) {
    eventCallback = cb;
}

void NavigationLogic::processDistanceReading(const DistanceReading& reading) {
    constexpr float kMinValidDistance = 0.05f;
    constexpr float kMaxValidDistance = 12.0f;

    if (reading.direction == Direction::AHEAD &&
        (reading.distanceMeters < kMinValidDistance || reading.distanceMeters > kMaxValidDistance)) {
        std::cout << "[NAV] Invalid LiDAR distance rejected: "
                  << reading.distanceMeters << " m" << std::endl;

        // Safety behavior: force navigation stop on invalid distance so a previous
        // vibration command does not remain latched.
        SystemEvent stopEvent;
        stopEvent.type = EventType::OBSTACLE;
        stopEvent.priority = Priority::LOW;
        stopEvent.navCommand.direction = Direction::AHEAD;
        stopEvent.navCommand.level = AlertLevel::NONE;
        if (eventCallback) {
            eventCallback(stopEvent);
        }
        return;
    }

    NavigationCommand command;
    command.direction = mapSensorDirection(reading.direction);
    command.level = distanceToAlertLevel(reading.distanceMeters);

    SystemEvent event;
    event.type = EventType::OBSTACLE;
    event.priority = alertLevelToPriority(command.level);
    event.navCommand = command;

    if (eventCallback) {
        eventCallback(event);
    }
}

AlertLevel NavigationLogic::distanceToAlertLevel(float distanceMeters) const {
    // Requested behavior: vibration ON at <= 1m, OFF when > 1m.
    if (distanceMeters > 1.0f) {
        return AlertLevel::NONE;
    }

    // Use CRITICAL so output controllers drive the continuous alert pattern.
    return AlertLevel::CRITICAL;
}

Direction NavigationLogic::mapSensorDirection(Direction sensorDirection) const {
    if (sensorDirection == Direction::LEFT) {
        return Direction::LEFT;
    }
    if (sensorDirection == Direction::RIGHT) {
        return Direction::RIGHT;
    }
    if (sensorDirection == Direction::AHEAD) {
        return Direction::AHEAD;
    }
    return Direction::NONE;
}

Priority NavigationLogic::alertLevelToPriority(AlertLevel level) const {
    if (level == AlertLevel::CRITICAL) {
        return Priority::HIGH;
    }
    if (level == AlertLevel::NEAR) {
        return Priority::MEDIUM;
    }
    return Priority::LOW;
}
