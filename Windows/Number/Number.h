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

        // Статические константы
        static const Number Zero;
        static const Number One;

    private:
        double value_;
    };

    Number createNumber(double value);

} // namespace Number