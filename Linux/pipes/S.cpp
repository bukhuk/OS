#include <iostream>
#include <sstream>
#include <string>

int main() {
    std::string line;

    while (std::getline(std::cin, line)) {
        std::stringstream ss(line);
        double number;
        double sum = 0.0;

        while (ss >> number) {
            sum += number;
        }

        std::cout << sum << "\n";
    }

    return 0;
}