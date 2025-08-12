#pragma once
#include <SDL2/SDL.h>
#include "mandelbrot.h"

constexpr int SCREEN_WIDTH = 800;
constexpr int SCREEN_HEIGHT = 600;
constexpr int HALFWIDTH = SCREEN_WIDTH / 2;
constexpr int HALFHEIGHT = SCREEN_HEIGHT / 2;

class App {
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Surface* render_surface;

    mandelbrot::PointView current_view;
    SDL_Texture* current_tex;

    void zoom(Sint32 x, Sint32 y, double mult);    
    void draw();
    void setTexture(SDL_Surface* surf);
    void setTexture(SDL_Texture* tex);

    public:
    App(int width, int height, int depth);
    ~App();

    void setView(const mandelbrot::AreaView&);
    void setView(const mandelbrot::PointView&);

    void initSDL();
    void loop();
};
