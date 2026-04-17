#ifndef IMUSENSOR_HPP
#define IMUSENSOR_HPP

#include <functional>
#include <thread>
#include <atomic>
#include <string>
#include "CommonTypes.hpp"

class IMUSensor {
public:
    using EventCallback = std::function<void(const SystemEvent&)>;

    IMUSensor();
    ~IMUSensor();

    void registerEventCallback(EventCallback cb);

    void start();
    void stop();

private:
    bool openDevice();
    void closeDevice();
    bool initializeSensor();
    bool readAccelerationG(float& ax, float& ay, float& az);
    bool detectFall(float ax, float ay, float az);
    void worker();

    EventCallback eventCallback;
    std::thread workerThread;
    std::atomic<bool> running{false};
    std::string i2cDevicePath;
    int i2cAddress;
    int i2cFd;
};

#endif