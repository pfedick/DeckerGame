#ifndef INCLUDE_RENDERSTATE_H_
#define INCLUDE_RENDERSTATE_H_

#include <SDL3/SDL.h>
#include <ppl7.h>
#include <ppl7-grafix.h>


class RenderState
{
private:
    SDL_GPUDevice* gpu_device;
    SDL_Renderer* renderer;

    SDL_GPUShader* blurHorizontalShader;
    SDL_GPUShader* blurVerticalShader;

    SDL_GPUShader* loadShader(const ppl7::String& filename, SDL_GPUShaderStage stage, int num_samplers, int num_storage_textures, int num_storage_buffers, int num_uniform_buffers);
    void releaseShader(SDL_GPUShader* shader);

    void loadShaders();
    void createRenderStates();


public:
    struct BlurUniforms {
        float blurStrength;
        float texelSizeX;
        float texelSizeY;
        float padding;  // Align auf 16 Bytes
    };

    SDL_GPURenderState* blurHorizontalState;
    SDL_GPURenderState* blurVerticalState;

    RenderState();
    ~RenderState();
    void init(SDL_GPUDevice* gpu, SDL_Renderer* renderer);
    SDL_GPUDevice* getGPUDevice();




};

#endif // INCLUDE_RENDERSTATE_H_