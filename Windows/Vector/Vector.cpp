#include "pch.h"
#include <string>
#include <sstream>
#include <cmath>
#include "Vector.h"
#include "Number.h"

// Внутренняя структура для скрытия реализации
struct VectorImpl {
    Number::Number* x;
    Number::Number* y;

    VectorImpl(double x_val, double y_val) :
        x(new Number::Number(x_val)),
        y(new Number::Number(y_val)) {}

    VectorImpl(const VectorImpl& other) :
        x(new Number::Number(other.x->getValue())),
        y(new Number::Number(other.y->getValue())) {}

    ~VectorImpl() {
        delete x;
        delete y;
    }
};

// Реализация методов (остается такой же как в предыдущей версии)
Vector::Vector() : impl(new VectorImpl(0.0, 0.0)) {}
Vector::Vector(double x, double y) : impl(new VectorImpl(x, y)) {}
Vector::Vector(const Vector& other) : impl(new VectorImpl(*static_cast<VectorImpl*>(other.impl))) {}
Vector::~Vector() { delete static_cast<VectorImpl*>(impl); }

Vector Vector::add(const Vector& other) const {
    VectorImpl* v1 = static_cast<VectorImpl*>(impl);
    VectorImpl* v2 = static_cast<VectorImpl*>(other.impl);
    Number::Number new_x = *(v1->x) + *(v2->x);
    Number::Number new_y = *(v1->y) + *(v2->y);
    return Vector(new_x.getValue(), new_y.getValue());
}

Vector Vector::subtract(const Vector& other) const {
    VectorImpl* v1 = static_cast<VectorImpl*>(impl);
    VectorImpl* v2 = static_cast<VectorImpl*>(other.impl);
    Number::Number new_x = *(v1->x) - *(v2->x);
    Number::Number new_y = *(v1->y) - *(v2->y);
    return Vector(new_x.getValue(), new_y.getValue());
}

double Vector::getMagnitude() const {
    VectorImpl* v = static_cast<VectorImpl*>(impl);
    Number::Number x_squared = *(v->x) * *(v->x);
    Number::Number y_squared = *(v->y) * *(v->y);
    Number::Number sum = x_squared + y_squared;
    return std::sqrt(sum.getValue());
}

double Vector::getAngle() const {
    VectorImpl* v = static_cast<VectorImpl*>(impl);
    return std::atan2(v->y->getValue(), v->x->getValue());
}

double Vector::getX() const {
    VectorImpl* v = static_cast<VectorImpl*>(impl);
    return v->x->getValue();
}

double Vector::getY() const {
    VectorImpl* v = static_cast<VectorImpl*>(impl);
    return v->y->getValue();
}

const char* Vector::toString() const {
    VectorImpl* v = static_cast<VectorImpl*>(impl);
    std::stringstream ss;
    ss << "(" << v->x->getValue() << ", " << v->y->getValue() << ")";
    std::string result = ss.str();
    char* cstr = new char[result.length() + 1];
    strcpy_s(cstr, result.length() + 1, result.c_str());
    return cstr;
}

const char* Vector::toPolarString() const {
    std::stringstream ss;
    ss << "r=" << getMagnitude() << ", angle=" << getAngle() << " rad";
    std::string result = ss.str();
    char* cstr = new char[result.length() + 1];
    strcpy_s(cstr, result.length() + 1, result.c_str());
    return cstr;
}

extern "C" VECTOR_API Vector Vector_Zero() {
    static Vector zeroVector(0.0, 0.0);
    return zeroVector;
}

extern "C" VECTOR_API Vector Vector_One() {
    static Vector oneVector(1.0, 1.0);
    return oneVector;
}