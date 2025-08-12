#include <iostream>
#include "SDL2/SDL.h"
#include "app.h"
#include "mandelbrot.h"
#include "complex.h"


using namespace std;

constexpr mandelbrot::AreaView default_view = {-2, 0.47, -1.12, 1.12};

int main(int argc, char **argv)
{

    App app = App();
    app.initSDL();

    SDL_Surface* surf = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0, 0, 0, 0);

    mandelbrot::draw(surf, default_view);
    app.setTexture(surf);

    SDL_FreeSurface(surf);
    app.loop();
    return 0;
}
