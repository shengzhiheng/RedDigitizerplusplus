cmake_minimum_required(VERSION 3.14...3.22)

# ---- Project ----

project(
    caen_runcontrol
    VERSION 0.0.3
    LANGUAGES CXX
)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ---- Include Directories ----
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include/RedDigitizer++
)

# ---- Source Files ----
set(SOURCES
    caen_rc.cpp
)

# ---- External CAEN Libraries ----
find_library(CAEN_DGTZ_LIB CAENDigitizer REQUIRED)
find_library(CAEN_COMM_LIB CAENComm REQUIRED)
find_library(CAEN_VME_LIB CAENVME REQUIRED)

# ---- Create Shared Library ----
add_library(${PROJECT_NAME} SHARED ${SOURCES})

# Link to CAENDigitizer and other CAEN libraries
target_link_libraries(${PROJECT_NAME}
    ${CAEN_DGTZ_LIB}
    ${CAEN_COMM_LIB}
    ${CAEN_VME_LIB}
)

# Set include directories for the shared library
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# ---- Set Install Path ----
set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR} CACHE PATH "Installation Directory")

# ---- Installation ----
install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
)

set(CMAKE_INSTALL_PREFIX "/path/to/install" CACHE PATH "Installation Directory")
