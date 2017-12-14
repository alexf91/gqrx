cmake_minimum_required(VERSION 3.2.0)

project(afsk1200)

if(NOT DEFINED GQRX_BASE_DIR)
    message(FATAL_ERROR "GQRX_BASE_DIR is not defined")
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5Widgets REQUIRED)

include_directories(${GQRX_BASE_DIR}/src/plugins)

QT5_WRAP_UI(UIS_HDRS afsk1200.ui)

set(SRCS_LIST
    afsk1200.cpp
    afsk1200.h
    afsk1200/cafsk12.cpp
    afsk1200/cafsk12.h
    afsk1200/costabf.c
    afsk1200/filter.h
    afsk1200/filter-i386.h
)

add_library(afsk1200 SHARED ${SRCS_LIST} ${UIS_HDRS})
target_link_libraries(afsk1200 PRIVATE Qt5::Widgets)
target_compile_options(afsk1200 PRIVATE ${COMPILE_OPTIONS})
set_target_properties(afsk1200 PROPERTIES PREFIX "")
