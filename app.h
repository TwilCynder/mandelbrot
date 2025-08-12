#pragma once
#include <SDL2/SDL.h>

constexpr int SCREEN_WIDTH = 800;
constexpr int SCREEN_HEIGHT = 600;

class App {
    SDL_Window* window;
    SDL_Renderer* renderer;

    SDL_Texture* current_tex;
    
    void draw();

    public:
    void setTexture(SDL_Surface* surf);
    void setTexture(SDL_Texture* tex);

    void initSDL();
    void loop();
};
