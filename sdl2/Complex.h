#pragma once
#include <cmath>
#include <iostream>

struct Complex {
    double im;
    double rl;

    Complex(double rl_, double im_);

    Complex();

    void set(double r, double i);

    Complex operator+(Complex& rhs);

    Complex& operator+=(Complex other);

    void square();

    double modulus();
    double modSquare();

};

std::ostream& operator<<(std::ostream& out, Complex c);