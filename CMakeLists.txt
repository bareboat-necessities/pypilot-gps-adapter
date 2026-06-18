cmake_minimum_required(VERSION 3.14)
project(pypilot-gps-adapter VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(PYPILOT_DATA_MODEL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../pypilot-data-model/src" CACHE PATH "Path to pypilot-data-model src")
set(PYPILOT_ALGORITHMS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../pypilot-algorithms/src" CACHE PATH "Path to pypilot-algorithms src")
set(PYPILOT_SENSORS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../pypilot-sensors/src" CACHE PATH "Path to pypilot-sensors src")

add_library(pypilot_gps_adapter STATIC src/pypilot_gps_adapter.cpp)
target_include_directories(pypilot_gps_adapter PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${PYPILOT_DATA_MODEL_DIR}
    ${PYPILOT_ALGORITHMS_DIR}
    ${PYPILOT_SENSORS_DIR})

option(PYPILOT_GPS_ADAPTER_BUILD_TESTS "Build pypilot-gps-adapter tests" ON)

if(PYPILOT_GPS_ADAPTER_BUILD_TESTS)
    enable_testing()
    function(add_gps_adapter_test name source)
        add_executable(${name} ${source})
        target_link_libraries(${name} PRIVATE pypilot_gps_adapter)
        add_test(NAME ${name} COMMAND ${name})
    endfunction()

    add_gps_adapter_test(test_gps_filter_math tests/test_gps_filter_math.cpp)
    add_gps_adapter_test(test_gpsd_json tests/test_gpsd_json.cpp)
    add_gps_adapter_test(test_gps_sensor_bridge tests/test_gps_sensor_bridge.cpp)
endif()
