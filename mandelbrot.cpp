#include <cmath>
#include "Complex.h"
#include "mandelbrot.h"
#include "SDL2/SDL.h"
#include <iostream>

#define DEBUG 1

#ifdef DEBUG
#include<chrono>
using namespace std::chrono;
#endif

using namespace mandelbrot;

constexpr int ITERATIONS = 40;

inline bool mandelbrot::isInSet(int i){
    return i >= ITERATIONS;
}

//unused because unoptimal
void mandelbrot::nextIteration(Complex& z, const Complex& c){
    z.square();
    z += c;
}

/**
 * @brief Tests is a point diverges
 * 
 * @param c point
 * @return int number of iterations it dook to diverge, or ITERATION if it hasn't diverged at the ITERATION-th iteration.
 */
int mandelbrot::testPoint(Complex c){
    Complex z(0, 0);
    Complex z2(0, 0); 

    int i;
    for (i = 0; i < ITERATIONS; i++){
        
        z.im = 2 * z.rl * z.im + c.im;
        z.rl =  z2.rl - z2.im + c.rl;
        z2.rl = z.rl * z.rl;
        z2.im = z.im * z.im;

        if (z2.rl + z.im >= 4) break;
    }
    return i;
}

void set_pixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
    Uint8 *target_pixel = (Uint8 *)surface->pixels + y * surface->pitch + x * 4;
    *(Uint32 *)target_pixel = pixel;
}

Uint32 hsv2rgb(const double h, const double s, const double v){
    double r, g, b;

    double C = v * s;
    double H_ = h / 60;
    long H_int = floor(H_);

    double X = C * (1- abs(((H_int & 1) + (H_ - H_int)) - 1));

    switch (H_int){
        case 0: r = C; g = X; b = 0; break;
        case 1: r = X; g = C; b = 0; break;
        case 2: r = 0; g = C; b = X; break;
        case 3: r = 0; g = X; b = C; break;
        case 4: r = X; g = 0; b = C; break;   
        case 5: r = C; g = 0; b = X; break;
        default: break;
    }

    double m = v - C;
    r += m; g += m; b += m;
    
    return ((Uint32)(r * 255)) * 65536 + ((Uint32)(g * 255)) * 256 + ((Uint32)(b * 255));
}

class time_ostream: public std::ostream {
    ~time_ostream(){}

};

template <typename Duration>
    requires ((bool)Duration::count && (bool)Duration::period::den)
double durationToSeconds(Duration duration){
    return duration.count() / (double)Duration::period::den;
}

std::ostream& mandelbrot::operator<<(std::ostream& out, const AreaView& view){
    out << view.x << " " << view.y << " " << view.w << " " << view.h;
    return out;
}

inline Uint32 get_color(int res){
    constexpr double color_step = 1.0 / ITERATIONS;

    return hsv2rgb(((color_step * res)) * 360 , 1.0, color_step * res);
}

void draw_(SDL_Surface* surface, const double x_lower, const double y_lower, const double ratio){
    
    auto start = high_resolution_clock::now();
    
    Complex point(x_lower, 0);
    for (int x = 0; x < surface->w; x++){
        point.rl += ratio;
        point.im = y_lower;
        for (int y = 0; y < surface->h; y++){
            point.im += ratio;

            int res = testPoint(point);
            if (isInSet(res))
                set_pixel(surface, x, y, 0);
            else 
                set_pixel(surface, x, y, get_color(res));
        }
    }

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);

    std::cout << "Rendered in " << durationToSeconds(duration) << "s\n";
}

void mandelbrot::drawAtArea(SDL_Surface* surface, const double x_lower, const double x_upper, const double y_lower, const double y_upper){
    draw_(surface, x_lower, y_lower, sizeToRatio(x_upper - x_lower, y_upper - y_lower, surface->w, surface->h));
}

void mandelbrot::drawAtPoint(SDL_Surface* surface, const double x, const double y, const double w, const double h){
    double ratio = sizeToRatio(w, h, surface->w, surface->h);
    draw_(surface, x - (surface->w / 2) * ratio, y - (surface->h / 2) * ratio, ratio);
}

void mandelbrot::draw(SDL_Surface* surface, const AreaView& view){
    drawAtArea(surface, view.x, view.y, view.w, view.h);
}

void mandelbrot::draw(SDL_Surface* surface, const PointView& view){
    drawAtPoint(surface, view.x, view.y, view.w, view.h);
}