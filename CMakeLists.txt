# !!! WARNING !!! AUTO-GENERATED FILE, PLEASE DO NOT MODIFY IT AND USE
# https://docs.platformio.org/page/projectconf/section_env_build.html#build-flags
#
# If you need to override existing CMake configuration or add extra,
# please create `CMakeListsUser.txt` in the root of project.
# The `CMakeListsUser.txt` will not be overwritten by PlatformIO.

cmake_minimum_required(VERSION 3.13)
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_CXX_COMPILER_WORKS 1)

project("arduino-uno-WiFi-Rev2" C CXX)

include(CMakeListsPrivate.txt)

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/CMakeListsUser.txt)
    include(CMakeListsUser.txt)
endif ()

include_directories(${includes}
        .pio/libdeps/uno_wifi_rev2/Arduino_LSM6DS3/src
        .pio/libdeps/uno_wifi_rev2/WiFiNINA/src
        .pio/libdeps/uno_wifi_rev2/Mahony/src
        .pio/libdeps/uno_wifi_rev2/ArduinoJson/src

        )
file(GLOB_RECURSE SOURCES ${sources}
        ".pio/libdeps/uno_wifi_rev2/Arduino_LSM6DS3/src/*.cpp"
        ".pio/libdeps/uno_wifi_rev2/WiFiNINA/src/*.cpp"
        ".pio/libdeps/uno_wifi_rev2/Mahony/src/*.cpp")


add_custom_target(
        Production ALL
        COMMAND platformio -c clion run "$<$<NOT:$<CONFIG:All>>:-e${CMAKE_BUILD_TYPE}>"
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_target(
        Debug ALL
        COMMAND platformio -c clion debug "$<$<NOT:$<CONFIG:All>>:-e${CMAKE_BUILD_TYPE}>"
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(Z_DUMMY_TARGET ${SRC_LIST} src/Sensor.cpp src/Sensor.h src/socket.cpp src/socket.h src/main.h)
