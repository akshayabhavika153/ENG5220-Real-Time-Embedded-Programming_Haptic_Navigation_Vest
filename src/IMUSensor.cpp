#include "IMUSensor.hpp"
#include <chrono>
#include <cmath>
#include <iostream>
#include <thread>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

namespace {
constexpr std::uint8_t REG_CTRL1_XL = 0x10;
constexpr std::uint8_t REG_OUTX_L_A = 0x28;
}

IMUSensor::IMUSensor() {
    i2cDevicePath = "/dev/i2c-1";
    i2cAddress = 0x6A;
    i2cFd = -1;
}

IMUSensor::~IMUSensor() {
    stop();
}

void IMUSensor::registerEventCallback(EventCallback cb) {
    eventCallback = cb;
}

void IMUSensor::start() {
    running = true;
    workerThread = std::thread(&IMUSensor::worker, this);
}

void IMUSensor::stop() {
    running = false;
    if (workerThread.joinable()) {
        workerThread.join();
    }
    closeDevice();
}

bool IMUSensor::openDevice() {
    closeDevice();

    i2cFd = ::open(i2cDevicePath.c_str(), O_RDWR);
    if (i2cFd < 0) {
        return false;
    }

    if (ioctl(i2cFd, I2C_SLAVE, i2cAddress) < 0) {
        closeDevice();
        return false;
    }

    return true;
}

void IMUSensor::closeDevice() {
    if (i2cFd >= 0) {
        ::close(i2cFd);
        i2cFd = -1;
    }
}

bool IMUSensor::initializeSensor() {
    if (i2cFd < 0) {
        return false;
    }

    std::uint8_t config[2];
    config[0] = REG_CTRL1_XL;
    config[1] = 0x40;

    return ::write(i2cFd, config, 2) == 2;
}

bool IMUSensor::readAccelerationG(float& ax, float& ay, float& az) {
    if (i2cFd < 0) {
        return false;
    }

    std::uint8_t reg = REG_OUTX_L_A;
    if (::write(i2cFd, &reg, 1) != 1) {
        return false;
    }

    std::uint8_t raw[6];
    if (::read(i2cFd, raw, 6) != 6) {
        return false;
    }

    const std::int16_t rawX = static_cast<std::int16_t>((raw[1] << 8) | raw[0]);
    const std::int16_t rawY = static_cast<std::int16_t>((raw[3] << 8) | raw[2]);
    const std::int16_t rawZ = static_cast<std::int16_t>((raw[5] << 8) | raw[4]);

    constexpr float scaleG = 0.061f / 1000.0f;
    ax = rawX * scaleG;
    ay = rawY * scaleG;
    az = rawZ * scaleG;

    return true;
}

bool IMUSensor::detectFall(float ax, float ay, float az) {
    const float magnitude = std::sqrt((ax * ax) + (ay * ay) + (az * az));
    return magnitude > 2.8f;
}

void IMUSensor::worker() {
    bool deviceErrorPrinted = false;
    auto lastFallEventTime = std::chrono::steady_clock::now() - std::chrono::seconds(10);

    while (running) {
        if (i2cFd < 0) {
            if (!openDevice() || !initializeSensor()) {
                closeDevice();
                if (!deviceErrorPrinted) {
                    std::cout << "[IMU] Unable to initialize LSM6DSOX on "
                              << i2cDevicePath << std::endl;
                    deviceErrorPrinted = true;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                continue;
            }
            deviceErrorPrinted = false;
        }

        float ax = 0.0f;
        float ay = 0.0f;
        float az = 0.0f;

        if (!readAccelerationG(ax, ay, az)) {
            closeDevice();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        if (detectFall(ax, ay, az)) {
            const auto now = std::chrono::steady_clock::now();
            if ((now - lastFallEventTime) >= std::chrono::seconds(5)) {
                SystemEvent fallEvent;
                fallEvent.type = EventType::FALL_DETECTED;
                fallEvent.priority = Priority::HIGH;
                fallEvent.navCommand.direction = Direction::NONE;
                fallEvent.navCommand.level = AlertLevel::NONE;

                if (eventCallback) {
                    eventCallback(fallEvent);
                }

                lastFallEventTime = now;
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
}