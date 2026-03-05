/**
 * @file lidar_driver.cpp
 * @brief LiDAR I2C driver with timerfd blocking
 * Disclaimer: We haven't received the lidar sensor yet, hence only a base code has been made taking the model of the lidar (Benewake TF Luna) that has been ordered. The code needs to be verified once the product is delivered
 */

#include "lidar_driver.h"

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <sys/timerfd.h>
#include <iostream>
#include <cstring>

LidarDriver::LidarDriver(const std::string& device, uint8_t address)
    : device_(device), address_(address) {}

LidarDriver::~LidarDriver() {
    stop();
    if (fd_i2c_ >= 0) close(fd_i2c_);
    if (fd_timer_ >= 0) close(fd_timer_);
}

bool LidarDriver::init() {
    fd_i2c_ = open(device_.c_str(), O_RDWR);
    if (fd_i2c_ < 0) {
        std::cerr << "[LiDAR] Error: cannot open " << device_ << std::endl;
        return false;
    }

    if (ioctl(fd_i2c_, I2C_SLAVE, address_) < 0) {
        std::cerr << "[LiDAR] Error: cannot set I2C address 0x"
                  << std::hex << (int)address_ << std::endl;
        close(fd_i2c_);
        fd_i2c_ = -1;
        return false;
    }

    fd_timer_ = timerfd_create(CLOCK_MONOTONIC, 0);
    if (fd_timer_ < 0) {
        std::cerr << "[LiDAR] Error: cannot create timerfd" << std::endl;
        close(fd_i2c_);
        fd_i2c_ = -1;
        return false;
    }

    constexpr long SAMPLE_PERIOD_MS = 100;
    struct itimerspec its;
    memset(&its, 0, sizeof(its));
    its.it_value.tv_sec = SAMPLE_PERIOD_MS / 1000;
    its.it_value.tv_nsec = (SAMPLE_PERIOD_MS % 1000) * 1000000;
    its.it_interval.tv_sec = SAMPLE_PERIOD_MS / 1000;
    its.it_interval.tv_nsec = (SAMPLE_PERIOD_MS % 1000) * 1000000;
    timerfd_settime(fd_timer_, 0, &its, NULL);

    std::cout << "[LiDAR] Initialised on " << device_
              << " addr=0x" << std::hex << (int)address_
              << " at 10Hz" << std::dec << std::endl;
    return true;
}

int LidarDriver::i2c_readWord(uint8_t reg) {
    uint8_t tmp[2] = {0};
    tmp[0] = reg;
    long int w = write(fd_i2c_, tmp, 1);
    if (w != 1) return -1;

    long int r = read(fd_i2c_, tmp, 2);
    if (r != 2) return -1;

    return (int)((tmp[0] << 8) | tmp[1]);
}


void LidarDriver::i2c_writeByte(uint8_t reg, uint8_t value) {
    uint8_t tmp[2];
    tmp[0] = reg;
    tmp[1] = value;
    write(fd_i2c_, tmp, 2);
}


void LidarDriver::registerCallback(LidarCallbackInterface cb) {
    callback_ = cb;
}

void LidarDriver::start() {
    if (running_) return;
    running_ = true;
    uthread_ = std::thread(&LidarDriver::worker, this);
    std::cout << "[LiDAR] Worker thread started" << std::endl;
}

void LidarDriver::stop() {
    if (!running_) return;
    running_ = false;
    if (uthread_.joinable()) uthread_.join();
    std::cout << "[LiDAR] Worker thread stopped" << std::endl;
}


void LidarDriver::worker() {
    while (running_) {

        uint64_t exp;
        const long int s = read(fd_timer_, &exp, sizeof(uint64_t));
        if (s != sizeof(uint64_t)) continue;
        if (!running_) break;


        int raw = i2c_readWord(0x00);
        if (raw < 0) continue;

        float distance_cm = static_cast<float>(raw);
        if (callback_) {
            callback_(distance_cm);
        }
    }
}
