#include "render.hpp"

struct Vertex {
    float x, y, z;
};

constexpr Vertex vertices[] = {
    {-1.0f, -1.0f, 0.0f},     // bottom left vertex
    {-1.0f, 1.0f, 0.0f},   // top left vertex
    {1.0f, -1.0f, 0.0f},     // bottom right vertex
    {1.0f, 1.0f, 0.0f},     // top right vertex
};

constexpr size_t verticesSize = sizeof(vertices);
constexpr size_t verticesN = verticesSize / sizeof(Vertex);

typedef unsigned int Index;
constexpr Index indices[] = {
    0, 1, 2,
    3, 1, 2
};
constexpr size_t indicesSize = sizeof(indices);
constexpr size_t indicesN = indicesSize / sizeof(Index);

bool MandelBrotRenderer::init(SDL_Window* target){
    device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, nullptr);
    if (!device) return false;

    SDL_ClaimWindowForGPUDevice(device, target);

     size_t vertexCodeSize; 
    void* vertexCode = SDL_LoadFile("shaders/vertex.spv", &vertexCodeSize);

    // configure the vertex shader
    SDL_GPUShaderCreateInfo vertexInfo{};
    vertexInfo.code = (Uint8*)vertexCode; //vvvv Describing the code of the buffer vvvv
    vertexInfo.code_size = vertexCodeSize;
    vertexInfo.entrypoint = "main";
    vertexInfo.format = SDL_GPU_SHADERFORMAT_SPIRV; 
    vertexInfo.stage = SDL_GPU_SHADERSTAGE_VERTEX; // vertex shader
    vertexInfo.num_samplers = 0; //jspppp
    vertexInfo.num_storage_buffers = 0;
    vertexInfo.num_storage_textures = 0;
    vertexInfo.num_uniform_buffers = 0;
    SDL_GPUShader* vertexShader = SDL_CreateGPUShader(device, &vertexInfo);
    // free the file
    SDL_free(vertexCode);

    // ------- Fragment Shader --------

    //pareil que précédemment
    size_t fragmentCodeSize; 
    void* fragmentCode = SDL_LoadFile("shaders/fragment.spv", &fragmentCodeSize);

    // create the fragment shader
    SDL_GPUShaderCreateInfo fragmentInfo{};
    fragmentInfo.code = (Uint8*)fragmentCode;
    fragmentInfo.code_size = fragmentCodeSize;
    fragmentInfo.entrypoint = "main";
    fragmentInfo.format = SDL_GPU_SHADERFORMAT_SPIRV;
    fragmentInfo.stage = SDL_GPU_SHADERSTAGE_FRAGMENT; // fragment shader
    fragmentInfo.num_samplers = 0;
    fragmentInfo.num_storage_buffers = 0;
    fragmentInfo.num_storage_textures = 0;
    fragmentInfo.num_uniform_buffers = 0;

    SDL_GPUShader* fragmentShader = SDL_CreateGPUShader(device, &fragmentInfo);

    // free the file
    SDL_free(fragmentCode);

    SDL_GPUBufferCreateInfo buffer_info{};
    buffer_info.size = verticesSize; //Taille nécessaire pour nos 3 vertices
    buffer_info.usage = SDL_GPU_BUFFERUSAGE_VERTEX; //Contiendra des vertices
    //Création du buffer en utilisant la config 
    vertexBuffer = SDL_CreateGPUBuffer(device, &buffer_info);

    buffer_info.size = indicesSize;
    buffer_info.usage = SDL_GPU_BUFFERUSAGE_INDEX; //Contiendra des indices de vertex (dans un autre buffer)
    indicesBuffer = SDL_CreateGPUBuffer(device, &buffer_info);

    SDL_GPUTransferBufferCreateInfo transfer_info{};
    transfer_info.size = verticesSize; //Même taille que le précédent, car on va y faire passer les mêmes données
    transfer_info.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD; //Sera utilisé pour uploader vers le GPU
    SDL_GPUTransferBuffer* transfer_buffer = SDL_CreateGPUTransferBuffer(device, &transfer_info);

    SDL_GPUTransferBufferCreateInfo indices_transfer_info{};
    indices_transfer_info.size = indicesSize; //Même taille que le précédent, car on va y faire passer les mêmes données
    indices_transfer_info.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD; //Sera utilisé pour uploader vers le GPU
    SDL_GPUTransferBuffer* indices_transfer_buffer = SDL_CreateGPUTransferBuffer(device, &indices_transfer_info);

    Vertex* data = static_cast<Vertex*>(SDL_MapGPUTransferBuffer(device, transfer_buffer, false)); //on demande une mémoire (dans mon espace mémoire) qui sera mappée au buffer
    SDL_memcpy(data, vertices, verticesSize); //on écrit dedans
    SDL_UnmapGPUTransferBuffer(device, transfer_buffer); //important

    Index* index_data = static_cast<Index*>(SDL_MapGPUTransferBuffer(device, indices_transfer_buffer, false));
    SDL_memcpy(index_data, indices, indicesSize);
    SDL_UnmapGPUTransferBuffer(device, indices_transfer_buffer); //important

    //Une fois que c'est fait on lande une Copy Pass
    SDL_GPUCommandBuffer* cbuf = SDL_AcquireGPUCommandBuffer(device); //on ouvre un command buffer juste pour cette opération
    SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(cbuf); //début d'une opération de copie
    SDL_GPUTransferBufferLocation transfer_loc  //description du transfer buffer source
    { 
        .transfer_buffer = transfer_buffer, 
        .offset = 0
    };

    SDL_GPUBufferRegion region  //description du buffer cible (ou plutôt une région d'un buffer)
    {
        .buffer = vertexBuffer,
        .offset = 0,
        .size = verticesSize
    };

    SDL_UploadToGPUBuffer(copyPass, &transfer_loc, &region, false); //Ici on configure réellement la copy pass

    transfer_loc.transfer_buffer = indices_transfer_buffer;
    region.buffer = indicesBuffer;
    region.size = indicesSize;
    SDL_UploadToGPUBuffer(copyPass, &transfer_loc, &region, false);

    SDL_EndGPUCopyPass(copyPass);

    //IMPORTANT : libérer le cbuffer mais surtout faire en sorte qu'il s'exécute
    SDL_SubmitGPUCommandBuffer(cbuf);

    SDL_ReleaseGPUTransferBuffer(device, transfer_buffer);

    SDL_GPUGraphicsPipelineCreateInfo pipelineInfo{};

    // bind shaders
    pipelineInfo.vertex_shader = vertexShader;
    pipelineInfo.fragment_shader = fragmentShader;

    // Indique que les vertices correcspondent à des sommets de triangles
    pipelineInfo.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;

    //Ici on décrit l'utilisation du vertex buffer qu'on avait déclaré plus tôt
    SDL_GPUVertexBufferDescription vertexBufferDesctiptions[1];
    vertexBufferDesctiptions[0].slot = 0; //C'est le buffer n°1

    //Il faut configurer comment les données dans ce buffer seront passées au shader, comment les données vont "cycler" (c'est à dire être passées une à une au shader)
    vertexBufferDesctiptions[0].input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX; //On va cycler sur les données vertex par vertex (et pas tout d'un coup)
    vertexBufferDesctiptions[0].instance_step_rate = 0;  
    vertexBufferDesctiptions[0].pitch = sizeof(Vertex); //Indique la taille des données sur lesquelles on cycle


    //On ajoute la config qu'on vient de créer à celle du pipeline
    pipelineInfo.vertex_input_state.num_vertex_buffers = 1;
    pipelineInfo.vertex_input_state.vertex_buffer_descriptions = vertexBufferDesctiptions;

    //On définit les attributs de chaque vertex (qui sera passé au vertex shader)
    SDL_GPUVertexAttribute vertexAttributes[1];
    //Dans le code du vertex shader, les "layout(position = <n>)" font référence au numéro d'un attribut tel qu'on les définit ici

    // Attribut 1 : apos
    vertexAttributes[0].buffer_slot = 0; //On parle du buffer n°1, voir ligne "vertexBufferDesctiptions[0].slot = ..."
    vertexAttributes[0].location = 0; // layout (location = 0) in shader, donc apos va récup cet attribut car déclaré avec ce layout
    vertexAttributes[0].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3; //vec3
    vertexAttributes[0].offset = 0; // start from the first byte from current buffer position*

    pipelineInfo.vertex_input_state.num_vertex_attributes = 1;
    pipelineInfo.vertex_input_state.vertex_attributes = vertexAttributes;

    //NOTE : les buffers qu'on a défini plus haut ne sont pas liés à la pipeline, on a juste décrits ceux qu'on va utiliser. La lisaison est à faire à chaque rendu 

    //Comme pour les buffers, ici on ne fait que décrire la color target, qui est à créer à chaque render à partir de la swapchain texture (qui change à chaque fois)
    SDL_GPUColorTargetDescription colorTargetDescriptions[1];
    colorTargetDescriptions[0] = {};
    colorTargetDescriptions[0].format = SDL_GetGPUSwapchainTextureFormat(device, target);
    //description TRES SIMPLE, en général y a plein de trucs à décrire genre le color blending etc

    //On ajoute la config qu'on vient de créer à celle du pipeline
    pipelineInfo.target_info.num_color_targets = 1;
    pipelineInfo.target_info.color_target_descriptions = colorTargetDescriptions;

    //On a fini de décrire la pipeline, on peut la créer 
    graphicsPipeline = SDL_CreateGPUGraphicsPipeline(device, &pipelineInfo);

    //Les shaders ont été ajoutés au pipeline, plus besoind de les garder en mem
    SDL_ReleaseGPUShader(device, vertexShader);
    SDL_ReleaseGPUShader(device, fragmentShader);

    return true;
}

void MandelBrotRenderer::deinit(){
    // destroy the GPU device
    if (!device) return;

    SDL_DestroyGPUDevice(device);

    //release buffers
    SDL_ReleaseGPUBuffer(device, vertexBuffer);
    SDL_ReleaseGPUBuffer(device, indicesBuffer);

    //release pipeline
    SDL_ReleaseGPUGraphicsPipeline(device, graphicsPipeline);
}

bool MandelBrotRenderer::draw(SDL_Window* target){
    SDL_GPUCommandBuffer* cbuf = SDL_AcquireGPUCommandBuffer(device);
    SDL_GPUTexture* swaptex;
    Uint32 w, h;

    SDL_WaitAndAcquireGPUSwapchainTexture(cbuf, target, &swaptex, &w, &h);

    if (!swaptex){
        SDL_SubmitGPUCommandBuffer(cbuf);
        return false;
    }

    SDL_GPUColorTargetInfo color_target{};
    color_target.clear_color = {0.1f, 0.1f, 0.1f, 1.0f};
    color_target.load_op = SDL_GPU_LOADOP_CLEAR;
    color_target.store_op = SDL_GPU_STOREOP_STORE;
    color_target.texture = swaptex;

    SDL_GPURenderPass* render_pass = SDL_BeginGPURenderPass(cbuf, &color_target, 1, NULL);

    //Première chose à faire : dire au GPU quelle pipeline on utilise pour ce rendu
    SDL_BindGPUGraphicsPipeline(render_pass, graphicsPipeline);

    //On configure le buffer à utiliser (on en avait qu'un seul, sur lequel on a déjà upload nos vertices)
    SDL_GPUBufferBinding buf_bindings[] = {{
        .buffer = vertexBuffer, 
        .offset = 0
    }};

    //Rappel qu'on a un système de slots de buffer (on avait défini qu'on a qu'un seul slot ici), là on précise à quels slots on bind un vrai buffer. 
    SDL_BindGPUVertexBuffers(render_pass, 0, buf_bindings, 1); //le 0 c'est le premier slot auquel on bind un truc

    SDL_GPUBufferBinding index_buf_bindings[] = {{
        .buffer = indicesBuffer, 
        .offset = 0
    }};

    SDL_BindGPUIndexBuffer(render_pass, index_buf_bindings, SDL_GPU_INDEXELEMENTSIZE_32BIT);

    //On y est : c'est la fonction qui met en marche le rendu avec notre pipeline
    SDL_DrawGPUIndexedPrimitives(render_pass, indicesN, 2, 0, 0, 0);

    SDL_EndGPURenderPass(render_pass);

    //---------------

    SDL_SubmitGPUCommandBuffer(cbuf);

    return true;
}