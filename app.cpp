#include "app.h"
void App::setTexture(SDL_Surface* surf){
	setTexture(SDL_CreateTextureFromSurface(renderer, surf));

}

void App::setTexture(SDL_Texture* tex){
	current_tex = tex;
}

void App::draw(){
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, current_tex, NULL, NULL);
	SDL_RenderPresent(renderer);
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
					default:
						break;
				}
			}
		}
		draw();
	}
}