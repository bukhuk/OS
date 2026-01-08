#include "Number.h"

#include <stdexcept>

namespace Number {

    Number::Number() : value(0.0) {}

    Number::Number(double val) : value(val) {}

    double Number::getValue() const {
        return value;
    }

    Number& Number::operator=(const Number& other) {
        if (this != &other) {
            value = other.value;
        }
        return *this;
    }

    Number Number::operator+(const Number& other) const {
        return Number(value + other.value);
    }

    Number Number::operator-(const Number& other) const {
        return Number(value - other.value);
    }

    Number Number::operator*(const Number& other) const {
        return Number(value * other.value);
    }

    Number Number::operator/(const Number& other) const {
        if (other.value == 0.0) {
            throw std::runtime_error("Division by zero");
        }
        return Number(value / other.value);
    }

    Number Number::Create(double value) {
        return Number(value);
    }

    Number createNumber(double value) {
        return Number(value);
    }

    const Number Number::Zero = Number(0.0);
    const Number Number::One = Number(1.0);

} // namespace Number