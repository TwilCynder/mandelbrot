#pragma once 

#include <SDL3/SDL_gpu.h>
#include "result.hpp"
#include <string>

struct SDL_Window;


class MandelBrotRenderer {
    SDL_GPUDevice* device;
    SDL_GPUBuffer* vertexBuffer;
    SDL_GPUBuffer* indicesBuffer;
    SDL_GPUGraphicsPipeline* graphicsPipeline;
    
    public:

    bool init(SDL_Window* target);
    void deinit();
    bool draw(SDL_Window* target);
};