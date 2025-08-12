#pragma once
class Complex;
struct SDL_Surface;

namespace mandelbrot{
    bool isInSet(int i);
    void nextIteration(Complex& z, const Complex& c);
    int testPoint(Complex c);

    struct AreaView {
        double x;
        double y;
        double w;
        double h;
    };

    struct PointView {
        double x;
        double y;
        double w;
        double h;
    };

    std::ostream& operator<<(std::ostream& out, const AreaView& view);

    void draw(SDL_Surface*, const AreaView& view);
    void draw(SDL_Surface*, const PointView& view);
    void drawAtPoint(SDL_Surface*, const double, const double, const double, const double);
    void drawAtArea(SDL_Surface*, const double, const double, const double, const double);

    constexpr double sizeToRatio(double source_w, double source_h, int target_w, int target_h){
        double surface_aspect_ratio = (double)target_w / target_h;
        double view_aspect_ratio = source_w / source_h;

        if (surface_aspect_ratio > view_aspect_ratio){
            return source_h / target_h;
        } else {
            return source_w / target_w;
        }
    }

}
