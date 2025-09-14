#define SDL_MAIN_USE_CALLBACKS

#include <iostream>
#include <SDL3/SDL_main.h>
#include "app.hpp"

App app;

SDL_AppResult SDL_AppInit(void **, int argc, char** argv){
    if (!app.init()) return SDL_APP_FAILURE;
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void*){
    bool result = app.draw();
    if (!result) return SDL_APP_FAILURE;
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void*, SDL_Event *event){
    switch (event->type){
        case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
            return SDL_APP_SUCCESS;
        case SDL_EVENT_KEY_DOWN:
            if (event->key.key == SDLK_ESCAPE)
                return SDL_APP_SUCCESS;
    }

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void*, SDL_AppResult result){
    app.deinit();
}