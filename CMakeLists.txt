cmake_minimum_required(VERSION 3.10)

project(vbsa)

option(VBSA_DEBUG "Additional console & file logs in some places" ON)

if(VBSA_DEBUG)
    add_definitions(-DVBSA_DEBUG)
endif ()

include_directories(${PROJECT_SOURCE_DIR}/lib)
include_directories(${PROJECT_SOURCE_DIR}/src)

add_subdirectory(lib)
add_subdirectory(src)