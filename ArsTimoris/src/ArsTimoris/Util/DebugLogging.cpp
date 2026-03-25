#include <ArsTimoris/Util/DebugLogging.hpp>
#include <iostream>

namespace DebugLogging {
    std::fstream ParsingOut;

    void OpenParserLog(std::filesystem::path a_path) {
        std::cout << "Opening Parser" << std::endl;
        DebugLogging::ParsingOut = std::fstream(a_path, std::ios::out);
        std::cout << "Opened Parser" << std::endl;
    }

    void CloseParserLog(void) {
        DebugLogging::ParsingOut.close();
    }
}