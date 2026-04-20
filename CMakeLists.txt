cmake_minimum_required(VERSION 3.16)

project(HapticNavigationVest
    VERSION 1.0.0
    DESCRIPTION "Single-hardware haptic navigation vest application"
    LANGUAGES CXX
)

# Build configuration
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    Debug
    Release
    RelWithDebInfo
    MinSizeRel
)

# Project options
option(HVEST_ENABLE_WARNINGS "Enable common compiler warnings" ON)
option(HVEST_WARNINGS_AS_ERRORS "Treat warnings as errors" OFF)
option(HVEST_ENABLE_SANITIZERS "Enable address and undefined sanitizers in Debug" OFF)
option(HVEST_ENABLE_LTO "Enable link time optimization in Release" OFF)
option(HVEST_ENABLE_TESTS "Enable test support" OFF)


# Output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


# Source files
set(HVEST_SOURCES
    src/main.cpp
    src/AudioController.cpp
    src/BatteryMonitor.cpp
    src/EmergencyController.cpp
    src/HapticController.cpp
    src/IMUSensor.cpp
    src/LidarSensor.cpp
    src/NavigationLogic.cpp
    src/PriorityManager.cpp
    src/SensorHealthMonitor.cpp
    src/StartupSelfTest.cpp
    src/SystemAlertController.cpp
)

set(HVEST_HEADERS
    include/AudioController.hpp
    include/BatteryMonitor.hpp
    include/CommonTypes.hpp
    include/EmergencyController.hpp
    include/HapticController.hpp
    include/IMUSensor.hpp
    include/LidarSensor.hpp
    include/NavigationLogic.hpp
    include/PriorityManager.hpp
    include/SensorHealthMonitor.hpp
    include/StartupSelfTest.hpp
    include/SystemAlertController.hpp
)


# Executable
add_executable(vest_app
    ${HVEST_SOURCES}
    ${HVEST_HEADERS}
)

target_include_directories(vest_app
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

find_package(Threads REQUIRED)
target_link_libraries(vest_app
    PRIVATE
        Threads::Threads
)

# Compile features and definitions
target_compile_features(vest_app PRIVATE cxx_std_17)

target_compile_definitions(vest_app PRIVATE
    $<$<CONFIG:Debug>:HVEST_DEBUG_BUILD=1>
    $<$<CONFIG:Release>:HVEST_RELEASE_BUILD=1>
    $<$<BOOL:${HVEST_ENABLE_SANITIZERS}>:HVEST_SANITIZERS_ENABLED=1>
)


# Compiler warnings
if(HVEST_ENABLE_WARNINGS)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(vest_app PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Wshadow
            -Wconversion
            -Wsign-conversion
            -Wnull-dereference
            -Wdouble-promotion
            -Wformat=2
            -Wimplicit-fallthrough
            -Wcast-align
            -Wunused
            -Woverloaded-virtual
            -Wnon-virtual-dtor
            -Wold-style-cast
        )

        if(HVEST_WARNINGS_AS_ERRORS)
            target_compile_options(vest_app PRIVATE -Werror)
        endif()

    elseif(MSVC)
        target_compile_options(vest_app PRIVATE /W4)
        if(HVEST_WARNINGS_AS_ERRORS)
            target_compile_options(vest_app PRIVATE /WX)
        endif()
    endif()
endif()


# Sanitizers for Debug builds
if(HVEST_ENABLE_SANITIZERS AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(vest_app PRIVATE
            -fsanitize=address
            -fsanitize=undefined
            -fno-omit-frame-pointer
        )
        target_link_options(vest_app PRIVATE
            -fsanitize=address
            -fsanitize=undefined
        )
    endif()
endif()


# Link Time Optimization
if(HVEST_ENABLE_LTO AND CMAKE_BUILD_TYPE MATCHES "Release|RelWithDebInfo")
    include(CheckIPOSupported)
    check_ipo_supported(RESULT HVEST_IPO_SUPPORTED OUTPUT HVEST_IPO_OUTPUT)
    if(HVEST_IPO_SUPPORTED)
        set_property(TARGET vest_app PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
    else()
        message(WARNING "IPO/LTO not supported: ${HVEST_IPO_OUTPUT}")
    endif()
endif()

# Organize files in IDEs
source_group(TREE ${PROJECT_SOURCE_DIR}/src PREFIX "Source Files" FILES ${HVEST_SOURCES})
source_group(TREE ${PROJECT_SOURCE_DIR}/include PREFIX "Header Files" FILES ${HVEST_HEADERS})


# Install rules
install(TARGETS vest_app
    RUNTIME DESTINATION bin
)

install(FILES ${HVEST_HEADERS}
    DESTINATION include
)

install(FILES
    README_SINGLE_HARDWARE.md
    DESTINATION .
    OPTIONAL
)



# Build summary
message(STATUS "")
message(STATUS "Project Name            : ${PROJECT_NAME}")
message(STATUS "Project Version         : ${PROJECT_VERSION}")
message(STATUS "Build Type              : ${CMAKE_BUILD_TYPE}")
message(STATUS "Compiler ID             : ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "C++ Standard            : ${CMAKE_CXX_STANDARD}")
message(STATUS "Warnings Enabled        : ${HVEST_ENABLE_WARNINGS}")
message(STATUS "Warnings As Errors      : ${HVEST_WARNINGS_AS_ERRORS}")
message(STATUS "Sanitizers Enabled      : ${HVEST_ENABLE_SANITIZERS}")
message(STATUS "Link Time Optimization  : ${HVEST_ENABLE_LTO}")
message(STATUS "Source File Count       : ${HVEST_SOURCES}")
message(STATUS "")
