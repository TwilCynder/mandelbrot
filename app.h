#pragma once
#include <SDL2/SDL.h>
#include "mandelbrot.h"

constexpr int SCREEN_WIDTH = 800;
constexpr int SCREEN_HEIGHT = 600;

class App {
    SDL_Window* window;
    SDL_Renderer* renderer;

    SDL_Texture* current_tex;
    SDL_Surface* render_surface;
    
    void draw();
    void setTexture(SDL_Surface* surf);
    void setTexture(SDL_Texture* tex);

    public:
    App(int width, int height, int depth);
    ~App();

    void setView(const mandelbrot::AreaView&);

    void initSDL();
    void loop();
};
