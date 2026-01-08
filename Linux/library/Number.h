#pragma once

namespace Number {

    class Number {
    public:
        Number();
        explicit Number(double val);

        Number& operator=(const Number& other);

        Number operator+(const Number& other) const;
        Number operator-(const Number& other) const;
        Number operator*(const Number& other) const;
        Number operator/(const Number& other) const;

        double getValue() const;

        static Number Create(double value);

        static const Number Zero;
        static const Number One;

    private:
        double value;
    };

    Number createNumber(double value);

} // namespace Number