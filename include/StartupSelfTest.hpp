#ifndef STARTUPSELFTEST_HPP
#define STARTUPSELFTEST_HPP

#include <string>

class StartupSelfTest {
public:
    explicit StartupSelfTest(std::string lidarDevice);

    void run() const;

private:
    std::string lidarDevicePath;
};

#endif
