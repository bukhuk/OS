#pragma once

#ifdef VECTOR_EXPORTS
#define VECTOR_API __declspec(dllexport)
#else
#define VECTOR_API __declspec(dllimport)
#endif

// Класс Vector
class VECTOR_API Vector {
private:
    void* impl; // Скрытая реализация (только для сокрытия Number)

public:
    // Конструкторы и деструктор
    Vector();
    Vector(double x, double y);
    Vector(const Vector& other);
    ~Vector();

    // Базовые операции
    Vector add(const Vector& other) const;      // Сложение
    Vector subtract(const Vector& other) const; // Вычитание

    // Полярные координаты
    double getMagnitude() const;  // Модуль вектора (r)
    double getAngle() const;      // Угол вектора (θ)

    // Вывод информации
    const char* toString() const;
    const char* toPolarString() const;

    // Геттеры
    double getX() const;
    double getY() const;
};

extern "C" VECTOR_API Vector Vector_Zero();
extern "C" VECTOR_API Vector Vector_One();