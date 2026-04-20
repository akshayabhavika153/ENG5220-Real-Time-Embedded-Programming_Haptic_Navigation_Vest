#ifndef COMMONTYPES_HPP
#define COMMONTYPES_HPP

#include <cstdint>

enum class Direction {
    NONE,
    LEFT,
    RIGHT,
    AHEAD
};

enum class AlertLevel {
    NONE,
    FAR,
    NEAR,
    CRITICAL
};

enum class EventType {
    OBSTACLE,
    FALL_DETECTED,
    LOW_BATTERY,
    SENSOR_ERROR,
    STARTUP
};

enum class Priority {
    LOW,
    MEDIUM,
    HIGH
};

struct DistanceReading {
    Direction direction;
    float distanceMeters;
    std::uint64_t timestampMs;
};

struct NavigationCommand {
    Direction direction;
    AlertLevel level;
};

struct SystemEvent {
    EventType type;
    Priority priority;
    NavigationCommand navCommand;
};

#endif