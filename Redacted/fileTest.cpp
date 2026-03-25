
#include <iostream>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <string>
#include <string_view>
#include <format>

struct BeBop {
public:
    int32_t craggle;
    int32_t falseer;
    bool applicable;

    std::string AsString(void) {
        return "{ " + std::to_string(craggle) + ", " + std::to_string(falseer) + ", " + (applicable ? "true" : "false") + " }";
    }
};

int main(int argc, char** argv) {
    void* arbitraryData = malloc(sizeof(BeBop) * 5);
    for (size_t i = 0; i < 5; ++i) {
        ((BeBop*)arbitraryData)[i].craggle = i * i;
        ((BeBop*)arbitraryData)[i].falseer = i + i;
        ((BeBop*)arbitraryData)[i].applicable = i % 2 == 0;
        std::cout << (i + 1) << ") " << ((BeBop*)arbitraryData)[i].AsString() << std::endl;
    }
    std::ofstream writer = std::ofstream(std::filesystem::current_path().append("bonk.data"));
    writer.write((const char*)arbitraryData, sizeof(BeBop) * 5);
    writer.close();

    std::cout << "Done Writing" << std::endl;

    void* unknownData = malloc(sizeof(BeBop) * 5);
    std::ifstream reader = std::ifstream(std::filesystem::current_path().append("bonk.data"));
    reader.read((char*)unknownData, sizeof(BeBop) * 5);
    reader.close();

    for (size_t i = 0; i < 5; ++i) {
        std::cout << (i + 1) << ") " << ((BeBop*)unknownData)[i].AsString() << std::endl;
    }

    return 0;
}