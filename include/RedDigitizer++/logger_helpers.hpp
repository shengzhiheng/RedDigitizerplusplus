/*
    Logger helper
    Author: Hector Hawley Herrera
    Description: Basic classes to use in red_digitizer_helper, so it can
    have some basic logging functionality.

    The best logger to use is spdlog (see: https://github.com/gabime/spdlog),
    but this is here so this library can work independently of it.
*/

#ifndef RD_LOGGER_HELPERS_H
#define RD_LOGGER_HELPERS_H
#pragma once

// C STD includes
// C 3rd party includes
// C++ STD includes
#include <iostream>
// C++ 3rd party includes
// my includes

namespace RedDigitizer {

// Use this class in red_digitizer_helper if a very basic logging is required.
// It will print directly to terminal with no timestamp, no way to discern
// the type of message, and lacks formatting features.
class iostream_wrapper {
 public:
    iostream_wrapper() = default;

    template<typename ...Args>
    void print(const std::string& out, Args&&... args) {
        std::cout << out << std::endl;
    }

    template<typename ...Args>
    void info(const std::string& out, Args&&... args) {
        print(out);
    }

    template<typename ...Args>
    void debug(const std::string& out, Args&&... args) {
        print(out);
    }

    template<typename ...Args>
    void warn(const std::string& out, Args&&... args) {
        print(out);
    }

    template<typename ...Args>
    void log(const std::string& out, Args&&... args) {
        print(out);
    }

    template<typename ...Args>
    void error(const std::string& out, Args&&... args) {
        print(out);
    }
};

// Use this class in red_digitizer_helper if no logging function is required
class no_logger {
 public:
    no_logger() = default;

    template<typename ...Args>
    void print(const std::string& out, Args&&... args) { }

    template<typename ...Args>
    void info(const std::string& out, Args&&... args) { }

    template<typename ...Args>
    void debug(const std::string& out, Args&&... args) { }

    template<typename ...Args>
    void warn(const std::string& out, Args&&... args) { }

    template<typename ...Args>
    void log(const std::string& out, Args&&... args) { }

    template<typename ...Args>
    void error(const std::string& out, Args&&... args) { }
};

}  // namespace RedDigitizer

#endif