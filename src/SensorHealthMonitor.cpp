#include "SensorHealthMonitor.hpp"
#include <chrono>
#include <cstdlib>

namespace {
std::uint64_t getTimeoutMsFromEnv(const char* name, std::uint64_t fallback) {
    const char* env = std::getenv(name);
    if (!env || env[0] == '\0') {
        return fallback;
    }

    char* end = nullptr;
    const unsigned long long value = std::strtoull(env, &end, 10);
    if (end == env || (end && *end != '\0')) {
        return fallback;
    }

    return static_cast<std::uint64_t>(value);
}

bool getMonitoringEnabledFromEnv() {
    const char* env = std::getenv("HVEST_SENSOR_HEALTH_ENABLE");
    if (!env || env[0] == '\0') {
        return true;
    }

    return !(env[0] == '0' && env[1] == '\0');
}
}

SensorHealthMonitor::SensorHealthMonitor()
    : lastLeftTimestamp(0),
      lastAheadTimestamp(0),
      lastRightTimestamp(0),
      startupTimeoutMs(getTimeoutMsFromEnv("HVEST_SENSOR_STARTUP_TIMEOUT_MS", 2000)),
      runtimeTimeoutMs(getTimeoutMsFromEnv("HVEST_SENSOR_RUNTIME_TIMEOUT_MS", 1500)),
      monitoringEnabled(getMonitoringEnabledFromEnv()),
      expectLeft(true),
      expectAhead(true),
      expectRight(true),
      sensorErrorSent(false) {
}

SensorHealthMonitor::~SensorHealthMonitor() {
    stop();
}

void SensorHealthMonitor::registerEventCallback(EventCallback cb) {
    eventCallback = cb;
}

void SensorHealthMonitor::configureExpectedSensors(bool left, bool ahead, bool right) {
    std::lock_guard<std::mutex> lock(dataMutex);
    expectLeft = left;
    expectAhead = ahead;
    expectRight = right;
}

void SensorHealthMonitor::updateSensorTimestamp(Direction direction, std::uint64_t timestampMs) {
    std::lock_guard<std::mutex> lock(dataMutex);

    if (direction == Direction::LEFT) {
        lastLeftTimestamp = timestampMs;
    } else if (direction == Direction::AHEAD) {
        lastAheadTimestamp = timestampMs;
    } else if (direction == Direction::RIGHT) {
        lastRightTimestamp = timestampMs;
    }
}

void SensorHealthMonitor::start() {
    running = true;
    workerThread = std::thread(&SensorHealthMonitor::worker, this);
}

void SensorHealthMonitor::stop() {
    running = false;
    if (workerThread.joinable()) {
        workerThread.join();
    }
}

bool SensorHealthMonitor::hasTimedOut(std::uint64_t lastTimestamp,
                                      std::uint64_t nowTimestamp,
                                      std::uint64_t monitorStartTimestampMs) const {
    if (lastTimestamp == 0) {
        return nowTimestamp > monitorStartTimestampMs &&
               (nowTimestamp - monitorStartTimestampMs) > startupTimeoutMs;
    }

    return (nowTimestamp > lastTimestamp) && ((nowTimestamp - lastTimestamp) > runtimeTimeoutMs);
}

void SensorHealthMonitor::worker() {
    const std::uint64_t monitorStartTimestampMs =
        static_cast<std::uint64_t>(
            std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now().time_since_epoch()
            ).count()
        );

    while (running) {
        if (!monitoringEnabled) {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            continue;
        }

        std::uint64_t nowTimestamp =
            static_cast<std::uint64_t>(
                std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::steady_clock::now().time_since_epoch()
                ).count()
            );

        bool timeoutDetected = false;

        {
            std::lock_guard<std::mutex> lock(dataMutex);

            if ((expectLeft && hasTimedOut(lastLeftTimestamp, nowTimestamp, monitorStartTimestampMs)) ||
                (expectAhead && hasTimedOut(lastAheadTimestamp, nowTimestamp, monitorStartTimestampMs)) ||
                (expectRight && hasTimedOut(lastRightTimestamp, nowTimestamp, monitorStartTimestampMs))) {
                timeoutDetected = true;
            }
        }

        if (timeoutDetected && !sensorErrorSent) {
            SystemEvent event;
            event.type = EventType::SENSOR_ERROR;
            event.priority = Priority::HIGH;
            event.navCommand.direction = Direction::NONE;
            event.navCommand.level = AlertLevel::CRITICAL;

            if (eventCallback) {
                eventCallback(event);
            }

            sensorErrorSent = true;
        }

        if (!timeoutDetected) {
            sensorErrorSent = false;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}