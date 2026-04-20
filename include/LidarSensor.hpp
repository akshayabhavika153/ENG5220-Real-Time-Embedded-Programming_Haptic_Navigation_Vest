#ifndef LIDARSENSOR_HPP
#define LIDARSENSOR_HPP

#include <functional>
#include <thread>
#include <atomic>
#include <string>
#include "CommonTypes.hpp"

class LidarSensor {
public:
    using DistanceCallback = std::function<void(const DistanceReading&)>;

    LidarSensor(Direction sensorDirection, const std::string& serialDevice);
    ~LidarSensor();

    void registerDistanceCallback(DistanceCallback cb);

    void start();
    void stop();

private:
    bool openSerialPort();
    void closeSerialPort();
    bool readFrame(std::uint8_t frame[9]);
    float frameToDistanceMeters(const std::uint8_t frame[9]) const;
    void worker();

    Direction direction;
    std::string serialDevicePath;
    DistanceCallback distanceCallback;
    std::thread workerThread;
    std::atomic<bool> running{false};
    int serialFd;
};

#endif