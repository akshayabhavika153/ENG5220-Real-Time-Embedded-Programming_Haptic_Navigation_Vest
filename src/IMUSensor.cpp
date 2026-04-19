#include "IMUSensor.hpp"
#include <chrono>
#include <cmath>
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <thread>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

namespace {
constexpr std::uint8_t REG_CTRL1_XL = 0x10;
constexpr std::uint8_t REG_OUTX_L_A = 0x28;
constexpr float FALL_ANGLE_THRESHOLD_DEG = 60.0f;
}

IMUSensor::IMUSensor() {
    const char* imuDevEnv = std::getenv("HVEST_IMU_I2C_DEVICE");
    const char* imuBusEnv = std::getenv("HVEST_IMU_BUS");
    const char* imuAddrEnv = std::getenv("HVEST_IMU_ADDR");

    if (imuDevEnv && imuDevEnv[0] != '\0') {
        i2cDevicePath = imuDevEnv;
    } else if (imuBusEnv && imuBusEnv[0] != '\0') {
        i2cDevicePath = std::string("/dev/i2c-") + imuBusEnv;
    } else {
        i2cDevicePath = "/dev/i2c-1";
    }

    if (imuAddrEnv && imuAddrEnv[0] != '\0') {
        i2cAddress = static_cast<int>(std::strtol(imuAddrEnv, nullptr, 0));
    } else {
        i2cAddress = 0x6A;
    }

    i2cFd = -1;
    hasReferenceOrientation = false;
    referenceAx = 0.0f;
    referenceAy = 0.0f;
    referenceAz = 1.0f;
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
    constexpr float kReferenceMinG = 0.7f;
    constexpr float kReferenceMaxG = 1.3f;

    const float magnitude = std::sqrt((ax * ax) + (ay * ay) + (az * az));
    if (magnitude < 1e-6f) {
        return false;
    }

    const float nx = ax / magnitude;
    const float ny = ay / magnitude;
    const float nz = az / magnitude;

    if (!hasReferenceOrientation && magnitude >= kReferenceMinG && magnitude <= kReferenceMaxG) {
        referenceAx = nx;
        referenceAy = ny;
        referenceAz = nz;
        hasReferenceOrientation = true;
        std::cout << "[IMU] Orientation reference locked" << std::endl;
        return false;
    }

    if (!hasReferenceOrientation) {
        return false;
    }

    const float angleDeg = orientationChangeDegrees(ax, ay, az);
    return angleDeg >= FALL_ANGLE_THRESHOLD_DEG;
}

float IMUSensor::orientationChangeDegrees(float ax, float ay, float az) const {
    const float magnitude = std::sqrt((ax * ax) + (ay * ay) + (az * az));
    if (magnitude < 1e-6f) {
        return 0.0f;
    }

    const float nx = ax / magnitude;
    const float ny = ay / magnitude;
    const float nz = az / magnitude;

    float dot = (referenceAx * nx) + (referenceAy * ny) + (referenceAz * nz);
    dot = std::max(-1.0f, std::min(1.0f, dot));
    return std::acos(dot) * 180.0f / 3.14159265358979323846f;
}

void IMUSensor::worker() {
    bool deviceErrorPrinted = false;
    auto lastFallEventTime = std::chrono::steady_clock::now() - std::chrono::seconds(10);
    auto lastAnglePrintTime = std::chrono::steady_clock::now() - std::chrono::seconds(1);
    const char* logAngleEnv = std::getenv("HVEST_IMU_LOG_ANGLE");
    const bool logAngle = !(logAngleEnv && logAngleEnv[0] == '0');

    while (running) {
        if (i2cFd < 0) {
            if (!openDevice() || !initializeSensor()) {
                closeDevice();
                if (!deviceErrorPrinted) {
                    std::cout << "[IMU] Unable to initialize LSM6DSOX on "
                              << i2cDevicePath
                              << " (addr 0x" << std::hex << i2cAddress << std::dec << ")"
                              << std::endl;
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

        const auto now = std::chrono::steady_clock::now();

        if (logAngle && hasReferenceOrientation) {
            const auto elapsedSinceLastPrint =
                std::chrono::duration_cast<std::chrono::milliseconds>(now - lastAnglePrintTime).count();
            if (elapsedSinceLastPrint >= 250) {
                const float angleDeg = orientationChangeDegrees(ax, ay, az);
                std::cout << "[IMU] Angle delta: " << angleDeg
                          << " deg (fall threshold: >= "
                          << FALL_ANGLE_THRESHOLD_DEG
                          << " deg)" << std::endl;
                lastAnglePrintTime = now;
            }
        }

        if (detectFall(ax, ay, az)) {
            if ((now - lastFallEventTime) >= std::chrono::seconds(5)) {
                const float angleDeg = orientationChangeDegrees(ax, ay, az);
                std::cout << "[IMU] Fall detected: orientation change "
                          << angleDeg << " deg" << std::endl;

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
