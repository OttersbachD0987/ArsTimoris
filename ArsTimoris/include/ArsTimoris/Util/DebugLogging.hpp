#pragma once
#ifndef ARS_TIMORIS__UTIL__DEBUG_LOGGING_HPP
#define ARS_TIMORIS__UTIL__DEBUG_LOGGING_HPP
#include <filesystem>
#include <fstream>

namespace DebugLogging {
    extern std::fstream ParsingOut;

    void OpenParserLog(std::filesystem::path a_path);

    void CloseParserLog(void);
}
#endif