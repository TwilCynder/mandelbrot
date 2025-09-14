#pragma once
#include <SDL3/SDL.h>
#include "render.hpp"

class App {
    SDL_Window* window;
    MandelBrotRenderer renderer;

    struct PointView {
        double x;
        double y;
        double w;
        double h;
    };

    PointView current_view;

    public:
    bool init();
    void deinit();

    void zoom(Sint32 x, Sint32 y, double mult);    
    bool draw();

    App() = default;
    ~App() = default;
};