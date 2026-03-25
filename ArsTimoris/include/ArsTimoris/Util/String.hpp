#pragma once
#ifndef ARS_TIMORIS__UTIL__STRING_HPP
#define ARS_TIMORIS__UTIL__STRING_HPP
#include <string>
#include <string_view>
#include <vector>

//#define STRING_UTILS_DEBUG_LOGGING 1

/// @brief Tell if a string is a boolean value.
/// @param a_string The string to check.
/// @return Whether or not the string is a boolean value.
/// @note Assumes the boolean value is lowercase.
bool IsStringBool(std::string_view a_string) {
    return a_string == "true" || a_string == "false";
}

/// @brief Tell if a string is a floating point value.
/// @param a_string The string to check.
/// @return Whether or not the string is a floating point value.
/// @note All integer values will be valid floating point values.
bool IsStringFloat(std::string_view a_string) {
    bool poppedDot = false;
    for (size_t i = (a_string.starts_with('-') ? 1 : 0); i < a_string.size(); ++i) {
        switch (a_string[i]) {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                break;
            case '.':
                if (!poppedDot) {
                    poppedDot = true;
                    break;
                }
            default:
                return false;
        }
    }
    return true;
}

/// @brief Tell if a string is an integer value.
/// @param a_string The string to check.
/// @return Whether or not the string is an integer value.
bool IsStringInt(std::string_view a_string) {
    for (size_t i = (a_string.starts_with('-') ? 1 : 0); i < a_string.size(); ++i) {
        switch (a_string[i]) {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                break;
            default:
                return false;
                break;
        }
    }
    return true;
}

size_t SplitString(const std::string& a_string, std::string_view a_seperator, std::vector<std::string>& a_vector) {
    if (a_string.empty()) {
        return 0;
    }
    size_t previous = 0;
    size_t cur = 0;
    while ((cur = a_string.find(a_seperator, cur)) < a_string.size()) {
        a_vector.push_back(a_string.substr(previous, cur - previous));
        #ifdef STRING_UTILS_DEBUG_LOGGING
        std::cout << "Cur: " << cur << " Previous: " << previous << " Substring: " << a_vector.back() << std::endl;
        #endif
        previous = ++cur;
    }
    a_vector.push_back(a_string.substr(previous, cur - previous));
    #ifdef STRING_UTILS_DEBUG_LOGGING
    std::cout << "Cur: " << cur << " Previous: " << previous << " Substring: " << a_vector.back() << std::endl;
    #endif
    return a_vector.size();
}

size_t ParseFunctionArguments(const std::string& a_string, std::vector<std::string>& a_vector) {
    #ifdef STRING_UTILS_DEBUG_LOGGING
    std::cout << "Paring Function Arguments String:\n" << a_string << std::endl;
    #endif
    if (a_string.empty()) {
        return 0;
    }
    #ifdef STRING_UTILS_DEBUG_LOGGING
    int32_t step = 0;
    #endif
    int32_t internal = 0;
    bool inString = false;
    bool escaped = false;
    std::string stringHolder = "";
    for (char character : a_string) {
        #ifdef STRING_UTILS_DEBUG_LOGGING
        std::cout << std::format("\x1b[1mFN_ARG\x1b[22m Step {:0>4}) {}InString\x1b[39m {}Escaped\x1b[39m Character: {} String [{:0>2}]: [{:0>3}] {}", step++, (inString ? "\x1b[32m" : "\x1b[31m"), (escaped ? "\x1b[32m" : "\x1b[31m"), character, a_vector.size(), stringHolder.size(), stringHolder) << std::endl;
        #endif
        switch (character) {
            case '"': {
                if (inString) {
                    if (escaped) {
                        stringHolder += character;
                    } else {
                        inString = false;
                    }
                    escaped = false;
                } else {
                    inString = true;
                }
                break;
            }
            case 'n': {
                if (inString && escaped) {
                    stringHolder += '\\';
                    escaped = false;
                }
                stringHolder += character;
                break;
            }
            case '\\': {
                if (inString) {
                    if (escaped) {
                        stringHolder += character;
                    }
                    escaped = !escaped;
                } else {
                    stringHolder += character;
                }
                break;
            }
            case ',': {
                if (!inString) {
                    if (internal > 0) {
                        stringHolder += character;
                    } else if (!stringHolder.empty()) {
                        a_vector.push_back(stringHolder);
                        stringHolder = "";
                    }
                } else {
                    stringHolder += character;
                    escaped = false;
                }
                break;
            }
            case '(': {
                stringHolder += character;
                if (inString) {
                    escaped = false;
                } else {
                    ++internal;
                }
                break;
            }
            case ')': {
                stringHolder += character;
                if (inString) {
                    escaped = false;
                } else {
                    --internal;
                }
                break;
            }
            default: {
                stringHolder += character;
                escaped = false;
                break;
            }
        }
    }
    if (!stringHolder.empty()) {
        a_vector.push_back(stringHolder);
    }
    return a_vector.size();
}

void SanitizeString(std::string& a_string) {
    size_t startOff = 0;
    size_t endOff = a_string.size();
    bool startDone = false;
    for (size_t i = 0; i < a_string.size(); ++i) {
        switch (a_string[i]) {
            case ' ':
            case '\n':
            case '\t':
            case '\v':
                break;
            default:
                if (startDone) {
                    endOff = i;
                } else {
                    startOff = i;
                    startDone = true;
                }
                break;
        }
    }
    #ifdef STRING_UTILS_DEBUG_LOGGING
    //std::cout << std::format("<{}>:<{}>:<{}>", a_string.substr(0, startOff + 1), a_string.substr(startOff, endOff - startOff + 1), a_string.substr(endOff, a_string.size() - endOff + 1)) << std::endl;
    #endif
    a_string = a_string.substr(startOff, endOff - startOff + 1);
}
#endif