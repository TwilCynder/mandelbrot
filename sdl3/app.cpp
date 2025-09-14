#include "app.hpp"

#define WINDOW_W 800
#define WINDOW_H 600



bool App::init(){
    window = SDL_CreateWindow("Mandelbrotte", WINDOW_W, WINDOW_H, SDL_WINDOW_RESIZABLE);

    return renderer.init(window);
}

void App::deinit(){
    renderer.deinit();
}

bool App::draw(){
    return renderer.draw(window);
}