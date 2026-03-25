#include <iostream>

int main(int* argc, char** argv) {
    std::cout << "\x1B[3;1;38;5;88;101mIntroduction\x1B[22;49m\n\n\x1B[2;107mThis is\tthe\ttab\t\x1B[9mit does tab things.\x1B[22;49m\n\n\x1B[4mThis \x1B[42mis the\t\x1B[30mvertical\x1B[45m tab\x1B[23;24;39;49m\v\v\v\x1b[31mI don't know why this exists it\x1B[29;39m\vis rather\v weird and stuff.\n\n" << std::flush;

    return 0;
}