// MathClient.cpp : Client app for MathLibrary DLL.
//#include "pch.h"// Uncomment for Visual Studio 2017 and earlier
#include <iostream>
#include "D:\OS_BSU\lab1\Vector\Vector.h"

void freeString(const char* str) {
    delete[] str;
}

void printVectorInfo(const char* name, const Vector& vec) {
    const char* cartesian = vec.toString();
    const char* polar = vec.toPolarString();

    std::cout << name << ":\n";
    std::cout << "  Cartesian coordinates: " << cartesian << std::endl;
    std::cout << "  Polar coordinates: " << polar << std::endl;
    std::cout << std::endl;

    freeString(cartesian);
    freeString(polar);
}

int main()
{
    Vector zero = Vector_Zero();
    Vector one = Vector_One();

    printVectorInfo("Zero vector (0, 0)", zero);
    printVectorInfo("One vector (1, 1)", one);

    std::cout << "1. Creating vectors:\n";
    Vector v1(3.0, 4.0);
    Vector v2(1.0, 2.0);
    Vector v3(-2.0, 3.0);

    printVectorInfo("Vector 1 (3, 4)", v1);
    printVectorInfo("Vector 2 (1, 2)", v2);
    printVectorInfo("Vector 3 (-2, 3)", v3);

    std::cout << "2. Vector addition:\n";
    Vector sum1 = v1.add(v2); // (3,4) + (1,2) = (4,6)
    Vector sum2 = v1.add(v3); // (3,4) + (-2,3) = (1,7)

    printVectorInfo("(3,4) + (1,2) = (4,6)", sum1);
    printVectorInfo("(3,4) + (-2,3) = (1,7)", sum2);

    std::cout << "3. Vector subtraction:\n";
    Vector diff1 = v1.subtract(v2); // (3,4) - (1,2) = (2,2)
    Vector diff2 = v3.subtract(v1); // (-2,3) - (3,4) = (-5,-1)

    printVectorInfo("(3,4) - (1,2) = (2,2)", diff1);
    printVectorInfo("(-2,3) - (3,4) = (-5,-1)", diff2);

    std::cout << "4. Special cases:\n";
    Vector sum_with_zero = v1.add(zero);
    Vector diff_with_zero = v1.subtract(zero);

    printVectorInfo("Zero vector", zero);
    printVectorInfo("(3,4) + (0,0) = (3,4)", sum_with_zero);
    printVectorInfo("(3,4) - (0,0) = (3,4)", diff_with_zero);

    std::cout << "5. Polar coordinates examples:\n";
    Vector unit_x(1.0, 0.0);    // r=1, angle=0
    Vector unit_y(0.0, 1.0);    // r=1, angle=π/2
    Vector diagonal(1.0, 1.0);  // r=√2, angle=π/4

    printVectorInfo("Unit vector X (1, 0)", unit_x);
    printVectorInfo("Unit vector Y (0, 1)", unit_y);
    printVectorInfo("Diagonal vector (1, 1)", diagonal);

    std::cout << "6. Operation chaining:\n";
    Vector result = v1.add(v2).subtract(v3); // (3,4)+(1,2)-(-2,3) = (6,3)
    printVectorInfo("(3,4)+(1,2)-(-2,3) = (6,3)", result);

    return 0;
}