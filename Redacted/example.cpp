#include <iostream>

int main(void) {
    float gradeInput;
    std::cout << "";
    std::cin >> gradeInput;
    if (gradeInput >= 90) {
        std::cout << "Grade is A" << std::endl;
    } else if (gradeInput >= 80) {
        std::cout << "Grade is B" << std::endl;
    } else if (gradeInput >= 70) {
        std::cout << "Grade is C" << std::endl;
    } else if (gradeInput >= 60) {
        std::cout << "Grade is D" << std::endl;
    } else {
        std::cout << "Grade is F" << std::endl;
    }
}