#include "app.h"
#include <iostream>

App::App(int width, int height, int depth) : current_tex(nullptr)
{
	render_surface = SDL_CreateRGBSurface(0, width, height, depth, 0, 0, 0, 0);
}

App::~App()
{
	SDL_FreeSurface(render_surface);
}

void App::initSDL(){
	int rendererFlags, windowFlags;

	if (!SDL_WasInit(0)){
		if (SDL_Init(SDL_INIT_VIDEO) < 0){
            printf("Couldn't initialize SDL: %s\n", SDL_GetError());
            exit(1);
        }
	}

	rendererFlags = SDL_RENDERER_ACCELERATED;
	windowFlags = 0;

	this->window = SDL_CreateWindow("Mandelbrotte", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, windowFlags);

	if (!this->window){
		printf("Failed to open %d x %d window: %s\n", SCREEN_WIDTH, SCREEN_HEIGHT, SDL_GetError());
		exit(1);
	}

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

	this->renderer = SDL_CreateRenderer(this->window, -1, rendererFlags);

	if (!this->renderer){
		printf("Failed to create renderer: %s\n", SDL_GetError());
		exit(1);
	}
}


void App::draw(){
	if (!current_tex) return;
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, current_tex, NULL, NULL);
	SDL_RenderPresent(renderer);
}

void App::setTexture(SDL_Surface* surf){
	auto tex = SDL_CreateTextureFromSurface(renderer, surf);
	setTexture(tex);
}

void App::setTexture(SDL_Texture* tex){
	current_tex = tex;
}

void App::setView(const mandelbrot::PointView &point){
	current_view = point;
	mandelbrot::draw(render_surface, point);
	setTexture(render_surface);
}

void App::setView(const mandelbrot::AreaView &area)
{	
	double w = area.xup - area.xlow;
	double h = area.yup - area.ylow;
	current_view = mandelbrot::PointView(area.xlow + w / 2, area.ylow + h / 2, w, h);
	mandelbrot::draw(render_surface, area);
	setTexture(render_surface);
}

void App::zoom(Sint32 x, Sint32 y, double mult)
{
	//std::cout << event.button.x << ' ' << event.button.y << '\n' << std::flush;
	//std::cout << current_view << '\n' << std::flush;
	double new_w = current_view.w * mult;
	double new_h = current_view.h * mult;
	double new_x = ((double)(x - HALFWIDTH) / SCREEN_WIDTH * current_view.w) + current_view.x;
	double new_y = ((double)(y - HALFHEIGHT) / SCREEN_HEIGHT * current_view.h) + current_view.y;
	//std::cout << new_x << ' ' << new_y << ' ' << new_w << ' ' << new_h << '\n' << std::flush;
	setView(mandelbrot::PointView(new_x, new_y, new_w, new_h));
}

void App::loop(){
	SDL_Event event;
	while(1){
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				case SDL_QUIT:
				exit(0);
				break;

				case SDL_KEYDOWN:
				switch (event.key.keysym.scancode){
					case SDL_SCANCODE_ESCAPE:
						exit(0);
						break;;
					default: break;	
				}
				case SDL_MOUSEBUTTONDOWN: {
					double mult;
					if (event.button.button == 1) mult = 0.5; 
					else if (event.button.button == 3) mult = 2;
					else goto mousebuttondownend;

					zoom(event.button.x, event.button.y, mult);
					mousebuttondownend:
				}

				break;
				case SDL_MOUSEWHEEL: {
					double mult = 1;
					if (event.wheel.y == 0) goto mousehweelend;
					if (event.wheel.y * (event.wheel.direction == SDL_MOUSEWHEEL_FLIPPED ? -1 : 1) > 0) mult = 0.8;
					else if (event.wheel.y * (event.wheel.direction == SDL_MOUSEWHEEL_FLIPPED ? -1 : 1) < 0) mult = (1 / 0.8);
					std::cout << event.wheel.direction << ' ' << event.wheel.x << ' ' << event.wheel.y << '\n' << std::flush;
					zoom(event.wheel.mouseX, event.wheel.mouseY, mult);
					mousehweelend: 
				}
				break;

			}
		}
		draw();
	}
}