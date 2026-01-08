#include <iostream>
#include <sstream>
#include <string>

int main() {
    std::string line;

    while (std::getline(std::cin, line)) {
        std::stringstream ss(line);
        double number;
        bool first = true;

        while (ss >> number) {
            if (!first) {
                std::cout << " ";
            }
            std::cout << number + 2;
            first = false;
        }
        std::cout << "\n";
    }

    return 0;
}