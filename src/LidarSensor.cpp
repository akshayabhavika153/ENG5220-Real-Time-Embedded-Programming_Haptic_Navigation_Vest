#include "LidarSensor.hpp"
#include <chrono>
#include <iostream>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>

LidarSensor::LidarSensor(Direction sensorDirection, const std::string& serialDevice)
    : direction(sensorDirection),
      serialDevicePath(serialDevice),
      serialFd(-1) {
}

LidarSensor::~LidarSensor() {
    stop();
}

void LidarSensor::registerDistanceCallback(DistanceCallback cb) {
    distanceCallback = cb;
}

void LidarSensor::start() {
    running = true;
    workerThread = std::thread(&LidarSensor::worker, this);
}

void LidarSensor::stop() {
    running = false;

    // Close first so any blocking read unblocks immediately.
    closeSerialPort();

    if (workerThread.joinable()) {
        workerThread.join();
    }
}

bool LidarSensor::openSerialPort() {
    closeSerialPort();

    serialFd = ::open(serialDevicePath.c_str(), O_RDONLY | O_NOCTTY);
    if (serialFd < 0) {
        return false;
    }

    termios tty;
    if (tcgetattr(serialFd, &tty) != 0) {
        closeSerialPort();
        return false;
    }

    cfsetispeed(&tty, B115200);
    cfsetospeed(&tty, B115200);

    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;

    tty.c_lflag = 0;
    tty.c_oflag = 0;
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);

    tty.c_cc[VMIN] = 0;
    tty.c_cc[VTIME] = 1;

    tcflush(serialFd, TCIFLUSH);

    if (tcsetattr(serialFd, TCSANOW, &tty) != 0) {
        closeSerialPort();
        return false;
    }

    return true;
}

void LidarSensor::closeSerialPort() {
    if (serialFd >= 0) {
        ::close(serialFd);
        serialFd = -1;
    }
}

bool LidarSensor::readFrame(std::uint8_t frame[9]) {
    if (serialFd < 0) {
        return false;
    }

    std::uint8_t byte = 0;
    while (running) {
        ssize_t bytesRead = ::read(serialFd, &byte, 1);
        if (bytesRead <= 0) {
            return false;
        }

        if (byte != 0x59) {
            continue;
        }

        bytesRead = ::read(serialFd, &byte, 1);
        if (bytesRead <= 0) {
            return false;
        }

        if (byte != 0x59) {
            continue;
        }

        frame[0] = 0x59;
        frame[1] = 0x59;

        std::uint8_t payload[7];
        bytesRead = ::read(serialFd, payload, 7);
        if (bytesRead != 7) {
            return false;
        }

        std::memcpy(&frame[2], payload, 7);

        std::uint16_t checksum = 0;
        for (int i = 0; i < 8; ++i) {
            checksum += frame[i];
        }

        if ((checksum & 0xFF) != frame[8]) {
            continue;
        }

        return true;
    }

    return false;
}

float LidarSensor::frameToDistanceMeters(const std::uint8_t frame[9]) const {
    const std::uint16_t distanceCm = static_cast<std::uint16_t>(frame[2]) |
                                     (static_cast<std::uint16_t>(frame[3]) << 8);
    return static_cast<float>(distanceCm) / 100.0f;
}

void LidarSensor::worker() {
    bool openErrorPrinted = false;

    while (running) {
        if (serialFd < 0 && !openSerialPort()) {
            if (!openErrorPrinted) {
                std::cout << "[LIDAR] Unable to open serial port " << serialDevicePath << std::endl;
                openErrorPrinted = true;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            continue;
        }

        openErrorPrinted = false;

        std::uint8_t frame[9];
        if (!readFrame(frame)) {
            closeSerialPort();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        DistanceReading reading;
        reading.direction = direction;
        reading.distanceMeters = frameToDistanceMeters(frame);
        reading.timestampMs =
            static_cast<std::uint64_t>(
                std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::steady_clock::now().time_since_epoch()
                ).count()
            );

        if (distanceCallback) {
            distanceCallback(reading);
        }
    }
}