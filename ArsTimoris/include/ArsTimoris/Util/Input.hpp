#pragma once
#ifndef ARS_TIMORIS__UTIL__INPUT_HPP
#define ARS_TIMORIS__UTIL__INPUT_HPP
#include <iostream>
#include <concepts>

/// @brief Get a validated input.
/// @tparam T An integral type.
/// @param a_value The value to feed into. 
template<std::integral T>
void SafeInput(T& a_value) {
    while (!(std::cin >> a_value)) {
        std::cout << "INVALID INPUT" << std::endl;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
}

/// @brief Get a validated input.
/// @tparam T A floating point type.
/// @param a_value The value to feed into.
template<std::floating_point T>
void SafeInput(T& a_value) {
    while (!(std::cin >> a_value)) {
        std::cout << "INVALID INPUT" << std::endl;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
}

/// @brief Get a validated input.
/// @tparam T The type to get.
/// @param a_value The value to feed into.
template<typename T>
requires(!std::integral<T> && !std::floating_point<T>)
void SafeInput(T& a_value) {
    while (!(std::cin >> a_value)) {
        std::cout << "INVALID INPUT" << std::endl;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
    }
}

/// @brief Eat one input on the same line, works like sys("pause") on windows.
inline void EatInput() {
    std::cout << "Press any key to continue..." << std::flush;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
    std::cout << "\x1B[F\x1B[0K\r" << std::flush;
}

#endif