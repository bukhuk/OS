#include <iostream>
#include <sstream>
#include <string>

int main() {
    std::string line;

    while (std::getline(std::cin, line)) {
        std::stringstream ss(line);
        double number;
        bool is_first = true;

        while (ss >> number) {
            if (!is_first) {
                std::cout << " ";
            }
            std::cout << number * 7;
            is_first = false;
        }
        std::cout << "\n";
    }

    return 0;
}