#include "renderstate.h"
#include "decker_sdl.h"

RenderState::RenderState()
{
    gpu_device = nullptr;
    renderer = nullptr;
    blurHorizontalShader = nullptr;
    blurVerticalShader = nullptr;
    blurHorizontalState = nullptr;
    blurVerticalState = nullptr;
}

RenderState::~RenderState()
{
    if (blurHorizontalState) {
        SDL_DestroyGPURenderState(blurHorizontalState);
    }
    if (blurVerticalState) {
        SDL_DestroyGPURenderState(blurVerticalState);
    }
    releaseShader(blurHorizontalShader);
    releaseShader(blurVerticalShader);
}


void RenderState::init(SDL_GPUDevice* gpu, SDL_Renderer* renderer)
{
    gpu_device = gpu;
    this->renderer = renderer;
    loadShaders();
    createRenderStates();
}

SDL_GPUDevice* RenderState::getGPUDevice()
{
    return gpu_device;
}

SDL_GPUShader* RenderState::loadShader(const ppl7::String& filename, SDL_GPUShaderStage stage, int num_samplers, int num_storage_textures, int num_storage_buffers, int num_uniform_buffers)
{
    if (!gpu_device) {
        ppl7::PrintDebug("GPU device is not initialized\n");
        throw SDLException("GPU device is not initialized");
    }

    ppl7::ByteArray buffer;

    try {
        buffer = ppl7::File::load(filename);
    }
    catch (const ppl7::Exception& e) {
        ppl7::PrintDebug("Failed to load shader file: %s: %s\n", (const char*)filename, e.what());
        throw SDLException("Failed to load shader file: %s", (const char*)filename);
    }

    SDL_GPUShaderCreateInfo shaderInfo = {
        .code_size = (size_t)buffer.size(),
        .code = (const Uint8*)buffer.adr(),
        .entrypoint = "main",
        .format = SDL_GPU_SHADERFORMAT_SPIRV,
        .stage = stage,
        .num_samplers = (Uint32)num_samplers,
        .num_storage_textures = (Uint32)num_storage_textures,
        .num_storage_buffers = (Uint32)num_storage_buffers,
        .num_uniform_buffers = (Uint32)num_uniform_buffers
    };

    SDL_GPUShader* shader = SDL_CreateGPUShader(gpu_device, &shaderInfo);
    if (!shader) {
        ppl7::PrintDebug("SDL_CreateGPUShader failed for %s: %s\n", (const char*)filename, SDL_GetError());
        throw SDLException("SDL_CreateGPUShader failed for %s: %s", (const char*)filename, SDL_GetError());
    }
    return shader;
}

void RenderState::releaseShader(SDL_GPUShader* shader)
{
    if (gpu_device && shader) {
        SDL_ReleaseGPUShader(gpu_device, shader);
    }
}


void RenderState::loadShaders()
{
    blurHorizontalShader = loadShader("res/shader/vulkan/blur_horizontal.spv", SDL_GPU_SHADERSTAGE_FRAGMENT, 1, 0, 0, 1);
    blurVerticalShader = loadShader("res/shader/vulkan/blur_vertical.spv", SDL_GPU_SHADERSTAGE_FRAGMENT, 1, 0, 0, 1);
}


void RenderState::createRenderStates()
{
    SDL_GPURenderStateCreateInfo stateInfo = {};
    stateInfo.fragment_shader = blurHorizontalShader;
    stateInfo.num_sampler_bindings = 0;     // Keine zusätzlichen Sampler
    stateInfo.num_storage_textures = 0;
    stateInfo.num_storage_buffers = 0;
    stateInfo.props = 0;

    blurHorizontalState = SDL_CreateGPURenderState(renderer, &stateInfo);
    if (!blurHorizontalState) {
        ppl7::PrintDebug("SDL_CreateGPURenderState failed for horizontal blur: %s\n", SDL_GetError());
        throw SDLException("SDL_CreateGPURenderState failed for horizontal blur: %s", SDL_GetError());
    }

    stateInfo.fragment_shader = blurVerticalShader;
    blurVerticalState = SDL_CreateGPURenderState(renderer, &stateInfo);
    if (!blurVerticalState) {
        ppl7::PrintDebug("SDL_CreateGPURenderState failed for vertical blur: %s\n", SDL_GetError());
        throw SDLException("SDL_CreateGPURenderState failed for vertical blur: %s", SDL_GetError());
    }
}   