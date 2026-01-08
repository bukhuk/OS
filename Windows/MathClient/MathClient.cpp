#include <iostream>

#include "Number.h"

int main() {
    Number::Number a = Number::createNumber(7.4);
    Number::Number b = 99;

    std::cout << "a + b = " << (a + b).getValue() << "\n";
    std::cout << "a - b = " << (a - b).getValue() << "\n";
    std::cout << "a * b = " << (a * b).getValue() << "\n";
    std::cout << "a / b = " << (a / b).getValue() << "\n";

    std::cout << "Zero: " << Number::Number::Zero.getValue() << "\n";
    std::cout << "One: " << Number::Number::One.getValue() << "\n";

    return 0;
}