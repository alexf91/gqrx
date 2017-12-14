cmake_minimum_required(VERSION 3.2.0)

project(echoplugin)

if(NOT DEFINED GQRX_BASE_DIR)
    message(FATAL_ERROR "GQRX_BASE_DIR is not defined")
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5Widgets REQUIRED)

include_directories(${GQRX_BASE_DIR}/src/plugins)

add_library(echoplugin SHARED echoplugin.cpp)
target_link_libraries(echoplugin PRIVATE Qt5::Widgets)
target_compile_options(echoplugin PRIVATE ${COMPILE_OPTIONS})
set_target_properties(echoplugin PROPERTIES PREFIX "")
