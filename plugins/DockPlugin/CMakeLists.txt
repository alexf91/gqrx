cmake_minimum_required(VERSION 3.2.0)

project(dockplugin)

if(NOT DEFINED GQRX_BASE_DIR)
    message(FATAL_ERROR "GQRX_BASE_DIR is not defined")
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5Widgets REQUIRED)

include_directories(${GQRX_BASE_DIR}/src/plugins)

QT5_WRAP_UI(UIS_HDRS dockplugin.ui)

set(SRCS_LIST
    dockplugin.cpp
    dockplugin.h
)

add_library(dockplugin SHARED ${SRCS_LIST} ${UIS_HDRS})
target_link_libraries(dockplugin PRIVATE Qt5::Widgets)
target_compile_options(dockplugin PRIVATE ${COMPILE_OPTIONS})
set_target_properties(dockplugin PROPERTIES PREFIX "")
