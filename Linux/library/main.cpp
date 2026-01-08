#include <iostream>

#include "Number.h"
#include "Vector.h"

int main() {
    Number::Number a = Number::createNumber(10.0);
    Number::Number b = Number::createNumber(5.0);

    std::cout << "a = " << a.getValue() << "\n";
    std::cout << "b = " << b.getValue() << "\n";
    std::cout << "a + b = " << (a + b).getValue() << "\n";
    std::cout << "a - b = " << (a - b).getValue() << "\n";
    std::cout << "a * b = " << (a * b).getValue() << "\n";
    std::cout << "a / b = " << (a / b).getValue() << "\n";

    std::cout << "Zero = " << Number::Number::Zero.getValue() << "\n";
    std::cout << "One = " << Number::Number::One.getValue() << "\n\n";

    Vector v1(3.0, 4.0);
    Vector v2(1.0, 2.0);

    std::cout << "v1 = " << v1.toString() << "\n";
    std::cout << "v2 = " << v2.toString() << "\n";

    Vector v3 = v1.add(v2);
    std::cout << "v1 + v2 = " << v3.toString() << "\n\n";

    std::cout << "Длина v1 = " << v1.getMagnitude() << "\n";
    std::cout << "Угол v1 = " << v1.getAngle() << " рад" << "\n";
    std::cout << "Полярные координаты v1: " << v1.toPolarString() << "\n\n";

    Vector zero_vec = Vector_Zero();
    Vector one_vec = Vector_One();

    std::cout << "Vector_Zero() = " << zero_vec.toString() << "\n";
    std::cout << "Vector_One() = " << one_vec.toString() << "\n";

    return 0;
}