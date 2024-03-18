cmake_minimum_required(VERSION 3.14...3.22)

project(
    RedDigitizer_basic_ex
    VERSION 0.0.1
    LANGUAGES CXX
)

add_executable(${PROJECT_NAME} main.cpp)

target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_17)
target_link_libraries(${PROJECT_NAME} PUBLIC RedDigitizer++::RedDigitizer++)
target_include_directories(
        ${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include/${PROJECT_NAME}-${PROJECT_VERSION}> ${CMAKE_CURRENT_BINARY_DIR}
)