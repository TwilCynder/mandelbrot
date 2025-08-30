#include "Complex.h"
#include <ostream>

Complex::Complex(double rl_, double im_) : 
    im(im_), rl(rl_)
{}

Complex::Complex()
{}

void Complex::set(double r, double i){
    rl = r;
    im = i;
}

Complex Complex::operator+(Complex& rhs){
    return Complex(im+rhs.im, rl+rhs.rl);
}

Complex& Complex::operator+=(Complex other){
    im += other.im;
    rl += other.rl;
    return *this;
}

/**
 * @brief Self-modifies instead of returning for performance reasons    
 * 
 */
void Complex::square(){
    double old_rl = rl;
    rl = rl*rl - im*im;
    im = 2 * old_rl * im;
}

double Complex::modulus(){
    return sqrt(rl*rl + im*im);
}

double Complex::modSquare(){
    return rl*rl + im*im;
}

std::ostream& operator<<(std::ostream& out, Complex c){
    if (out.flags() & std::ostream::scientific){
        out << c.rl << (c.im < 0 ? 0 : '+') << c.im << 'i';
    } else {
        out << "Complex (" << c.rl << "," << c.im << ")";
    }
    
    return out;
}