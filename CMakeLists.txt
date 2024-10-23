cmake_minimum_required(VERSION 3.14)

# ---- Project ----
project(red_caen 
        VERSION 0.0.4 
        LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ---- Include Directories ----
set(PYBIND11_DIR "/home/sbc/anaconda3/envs/runcontrol/include/")
set(PYTHON_DIR "/home/sbc/anaconda3/envs/runcontrol/include/python3.12")

include_directories(${PYBIND11_DIR})
include_directories(${PYTHON_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
find_package(pybind11 REQUIRED)

# ---- External CAEN Libraries ----
find_library(CAEN_DGTZ_LIB CAENDigitizer REQUIRED)
find_library(CAEN_COMM_LIB CAENComm REQUIRED)
find_library(CAEN_VME_LIB CAENVME REQUIRED)

# ---- Source Files ----
set(SOURCES
    red_digitizer_helper.cpp
)

# ---- Create Shared Library ----
add_library(${PROJECT_NAME} SHARED ${SOURCES})

# Link to CAENDigitizer and other CAEN libraries
target_link_libraries(${PROJECT_NAME}
    ${CAEN_DGTZ_LIB}
    ${CAEN_COMM_LIB}
    ${CAEN_VME_LIB}
)

# Set include directories for the shared library
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties(red_caen PROPERTIES PREFIX "")

# ---- Installation ----
install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
)
