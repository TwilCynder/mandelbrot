#include <iostream>
#include "SDL2/SDL.h"
#include "app.h"
#include "mandelbrot.h"
#include "complex.h"


using namespace std;

constexpr mandelbrot::AreaView default_view = {-2, 0.47, -1.12, 1.12};

int main(int argc, char **argv)
{
    App app = App(SCREEN_WIDTH, SCREEN_HEIGHT, 32);
    app.initSDL();

    app.setView(default_view);

    app.loop();
    return 0;
}
