#include "NavigationLogic.hpp"

NavigationLogic::NavigationLogic() {
    hasPreviousReading = false;
    previousDistanceMeters = 0.0f;
    previousTimestampMs = 0;
    lastFallEventTimestampMs = 0;
}

void NavigationLogic::registerEventCallback(EventCallback cb) {
    eventCallback = cb;
}

void NavigationLogic::processDistanceReading(const DistanceReading& reading) {
    if (detectFallFromLidar(reading)) {
        SystemEvent fallEvent;
        fallEvent.type = EventType::FALL_DETECTED;
        fallEvent.priority = Priority::HIGH;
        fallEvent.navCommand.direction = Direction::AHEAD;
        fallEvent.navCommand.level = AlertLevel::CRITICAL;

        if (eventCallback) {
            eventCallback(fallEvent);
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

bool NavigationLogic::detectFallFromLidar(const DistanceReading& reading) {
    constexpr float kMinValidDistance = 0.05f;
    constexpr float kMaxValidDistance = 12.0f;
    constexpr float kPreFallDistanceMeters = 1.2f;
    constexpr float kFallDistanceMeters = 0.45f;
    constexpr std::uint64_t kRapidDropWindowMs = 700;
    constexpr std::uint64_t kFallCooldownMs = 8000;

    if (reading.direction != Direction::AHEAD ||
        reading.distanceMeters < kMinValidDistance ||
        reading.distanceMeters > kMaxValidDistance) {
        return false;
    }

    bool fallDetected = false;
    if (hasPreviousReading && reading.timestampMs > previousTimestampMs) {
        const std::uint64_t dtMs = reading.timestampMs - previousTimestampMs;
        const bool rapidDrop = (dtMs <= kRapidDropWindowMs) &&
                               (previousDistanceMeters >= kPreFallDistanceMeters) &&
                               (reading.distanceMeters <= kFallDistanceMeters);
        const bool cooldownElapsed = (lastFallEventTimestampMs == 0) ||
                                     ((reading.timestampMs - lastFallEventTimestampMs) >= kFallCooldownMs);

        if (rapidDrop && cooldownElapsed) {
            fallDetected = true;
            lastFallEventTimestampMs = reading.timestampMs;
        }
    }

    previousDistanceMeters = reading.distanceMeters;
    previousTimestampMs = reading.timestampMs;
    hasPreviousReading = true;

    return fallDetected;
}

AlertLevel NavigationLogic::distanceToAlertLevel(float distanceMeters) const {
    if (distanceMeters > 2.0f) {
        return AlertLevel::NONE;
    }
    if (distanceMeters > 1.0f) {
        return AlertLevel::FAR;
    }
    if (distanceMeters > 0.5f) {
        return AlertLevel::NEAR;
    }
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