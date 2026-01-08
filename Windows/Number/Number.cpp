#include "pch.h"
#include "framework.h"

#include "Number.h"
#include <stdexcept>

namespace Number {

    Number::Number() : value_(0.0) {}

    Number::Number(double val) : value_(val) {}

    double Number::getValue() const {
        return value_;
    }

    Number& Number::operator=(const Number& other) {
        if (this != &other) {
            value_ = other.value_;
        }
        return *this;
    }

    Number Number::operator+(const Number& other) const {
        return Number(value_ + other.value_);
    }

    Number Number::operator-(const Number& other) const {
        return Number(value_ - other.value_);
    }

    Number Number::operator*(const Number& other) const {
        return Number(value_ * other.value_);
    }

    Number Number::operator/(const Number& other) const {
        if (other.value_ == 0.0) {
            throw std::runtime_error("Division by zero");
        }
        return Number(value_ / other.value_);
    }

    Number createNumber(double value) {
        return Number(value);
    }

    const Number Number::Zero = Number(0.0);
    const Number Number::One = Number(1.0);

} // namespace Number