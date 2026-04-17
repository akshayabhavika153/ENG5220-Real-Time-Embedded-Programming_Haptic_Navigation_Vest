#ifndef PRIORITYMANAGER_HPP
#define PRIORITYMANAGER_HPP

#include <functional>
#include <mutex>
#include "CommonTypes.hpp"

class PriorityManager {
public:
    using NavigationOutputCallback = std::function<void(const NavigationCommand&)>;
    using EmergencyOutputCallback = std::function<void()>;
    using SystemOutputCallback = std::function<void(const SystemEvent&)>;

    PriorityManager();

    void registerNavigationOutputCallback(NavigationOutputCallback cb);
    void registerEmergencyOutputCallback(EmergencyOutputCallback cb);
    void registerSystemOutputCallback(SystemOutputCallback cb);

    void handleEvent(const SystemEvent& event);

private:
    int priorityToInt(Priority priority) const;

    mutable std::mutex stateMutex;

    NavigationOutputCallback navigationOutputCallback;
    EmergencyOutputCallback emergencyOutputCallback;
    SystemOutputCallback systemOutputCallback;

    SystemEvent currentEvent;
    bool hasActiveEvent;
    bool emergencyActive;
};

#endif