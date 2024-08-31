cmake_minimum_required(VERSION 3.27)

set(PROJECT_NAME MayaLinker)
project(${PROJECT_NAME})

include($ENV{DEVKIT_LOCATION}/cmake/pluginEntry.cmake)

set(SOURCE_FILES
    MayaLinker.cpp
)

set(LIBRARIES
    OpenMaya Foundation
)

build_plugin()