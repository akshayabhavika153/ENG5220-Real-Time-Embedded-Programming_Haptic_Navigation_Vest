#include "PriorityManager.hpp"

PriorityManager::PriorityManager()
    : hasActiveEvent(false),
      emergencyActive(false) {
    currentEvent.type = EventType::STARTUP;
    currentEvent.priority = Priority::LOW;
    currentEvent.navCommand.direction = Direction::NONE;
    currentEvent.navCommand.level = AlertLevel::NONE;
}

void PriorityManager::registerNavigationOutputCallback(NavigationOutputCallback cb) {
    navigationOutputCallback = cb;
}

void PriorityManager::registerEmergencyOutputCallback(EmergencyOutputCallback cb) {
    emergencyOutputCallback = cb;
}

void PriorityManager::registerSystemOutputCallback(SystemOutputCallback cb) {
    systemOutputCallback = cb;
}

void PriorityManager::handleEvent(const SystemEvent& event) {
    NavigationOutputCallback navCb;
    EmergencyOutputCallback emergencyCb;
    SystemOutputCallback systemCb;
    NavigationCommand navCommand{Direction::NONE, AlertLevel::NONE};
    SystemEvent systemEvent = event;
    bool shouldSendNavigation = false;
    bool shouldSendEmergency = false;
    bool shouldSendSystem = false;

    {
        std::lock_guard<std::mutex> lock(stateMutex);

        if (emergencyActive) {
            return;
        }

        if (event.type == EventType::FALL_DETECTED) {
            emergencyActive = true;
            currentEvent = event;
            hasActiveEvent = true;

            // Stop navigation vibration when emergency starts; fall alert output is handled
            // by the dedicated emergency callback path.
            navCommand.direction = Direction::NONE;
            navCommand.level = AlertLevel::NONE;
            shouldSendNavigation = true;
            shouldSendEmergency = true;
            navCb = navigationOutputCallback;
            emergencyCb = emergencyOutputCallback;
        } else if (event.type == EventType::OBSTACLE) {
            // Navigation must track latest distance state continuously.
            // Do not latch a prior higher-priority obstacle command, otherwise
            // vibration can remain ON even after distance becomes safe.
            currentEvent = event;
            hasActiveEvent = true;
            navCommand = currentEvent.navCommand;
            shouldSendNavigation = true;
            navCb = navigationOutputCallback;
        } else if (event.type == EventType::SENSOR_ERROR) {
            currentEvent = event;
            hasActiveEvent = true;

            navCommand.direction = Direction::NONE;
            navCommand.level = AlertLevel::NONE;
            shouldSendNavigation = true;
            shouldSendSystem = true;
            navCb = navigationOutputCallback;
            systemCb = systemOutputCallback;
            systemEvent = event;
        } else if (event.type == EventType::LOW_BATTERY) {
            shouldSendSystem = true;
            systemCb = systemOutputCallback;
            systemEvent = event;
        } else if (!hasActiveEvent ||
                   priorityToInt(event.priority) >= priorityToInt(currentEvent.priority)) {
            currentEvent = event;
            hasActiveEvent = true;
            navCommand = currentEvent.navCommand;
            shouldSendNavigation = true;
            navCb = navigationOutputCallback;
        }
    }

    if (shouldSendNavigation && navCb) {
        navCb(navCommand);
    }

    if (shouldSendSystem && systemCb) {
        systemCb(systemEvent);
    }

    if (shouldSendEmergency && emergencyCb) {
        emergencyCb();
    }
}

int PriorityManager::priorityToInt(Priority priority) const {
    if (priority == Priority::HIGH) {
        return 3;
    }
    if (priority == Priority::MEDIUM) {
        return 2;
    }
    return 1;
}