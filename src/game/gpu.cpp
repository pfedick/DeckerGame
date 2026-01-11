#include <ppl7.h>
#include <ppl7-grafix.h>
#include "gpu.h"
#include "sprite.h"

static GPUContext* globalGPUContext = NULL;

GPUContext& getGlobalGPUContext()
{
    if (!globalGPUContext) {
        throw GPUException("Global GPUContext is not initialized");
    }
    return *globalGPUContext;
}

GPUContext::GPUContext()
{
    gpu = NULL;
    window = NULL;
    pass = NULL;
    z = 0.0f;
    globalGPUContext = this;
}

GPUContext::~GPUContext()
{
    shutdown();
}

void GPUContext::init(SDL_Window* window)
{
    shutdown();
    gpu = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, false, NULL);
    if (!gpu) {
        throw GPUException("SDL_CreateGPUDevice failed: %s", SDL_GetError());
    }
    // window für GPUDevice beanspruchen und Swapchain initialisieren
    if (!SDL_ClaimWindowForGPUDevice(gpu, window)) {
        SDL_DestroyGPUDevice(gpu);
        gpu = NULL;
        throw GPUException("SDL_ClaimWindowForGPUDevice failed: %s", SDL_GetError());
    }
    this->window = window;

}

void GPUContext::shutdown()
{
    if (gpu) {
        if (window) SDL_ReleaseWindowFromGPUDevice(gpu, window);
        SDL_DestroyGPUDevice(gpu);
        gpu = NULL;
    }
}


SDL_GPUTexture* GPUContext::createGPUTexture(const ppl7::grafix::Drawable& surface)
{
    if (!gpu) {
        throw GPUException("GPU device is not initialized");
    }
    // Textur-Beschreibung
    SDL_GPUTextureCreateInfo texture_info = {
    .type = SDL_GPU_TEXTURETYPE_2D,
    .format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,  // Filterbar, Sampling-fähig
    .usage = SDL_GPU_TEXTUREUSAGE_SAMPLER,  // Für Shader-Sampling
    .width = (Uint32)surface.width(),
    .height = (Uint32)surface.height(),
    };
    // Textur erstellen
    SDL_GPUTexture* texture = SDL_CreateGPUTexture(gpu, &texture_info);
    if (!texture) {
        throw GPUException("SDL_CreateGPUTexture failed: %s", SDL_GetError());
    }
    // Daten in GPU hochladen
    SDL_GPUTransferBufferCreateInfo  transfer_info = {
        .size = (Uint32)surface.width() * (Uint32)surface.height() * 4,  // RGBA8 = 4 Bytes/Pixel
    };
    SDL_GPUTransferBuffer* transfer_buffer = SDL_CreateGPUTransferBuffer(gpu, &transfer_info);
    if (!transfer_buffer) {
        SDL_ReleaseGPUTexture(gpu, texture);
        throw GPUException("SDL_CreateGPUTransferBuffer failed: %s", SDL_GetError());
    }

    // Pixel-Daten kopieren
    void* mapped = SDL_MapGPUTransferBuffer(gpu, transfer_buffer, false);
    memcpy(mapped, surface.adr(), surface.width() * surface.height() * 4);
    SDL_UnmapGPUTransferBuffer(gpu, transfer_buffer);

    // Mit Command Buffer zur GPU transferieren
    SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(gpu);
    SDL_GPUCopyPass* copy_pass = SDL_BeginGPUCopyPass(cmd);
    SDL_GPUTextureTransferInfo transfer_region = {
        .transfer_buffer = transfer_buffer,
        .offset = 0
    };
    SDL_GPUTextureRegion texture_region = {
        .texture = texture,
        .mip_level = 0,
        .layer = 0,
        .x = 0,
        .y = 0,
        .z = 0,
        .w = (Uint32)surface.width(),
        .h = (Uint32)surface.height(),
        .d = 1
    };
    SDL_UploadToGPUTexture(copy_pass, &transfer_region, &texture_region, false);
    SDL_EndGPUCopyPass(copy_pass);
    SDL_SubmitGPUCommandBuffer(cmd);

    SDL_ReleaseGPUTransferBuffer(gpu, transfer_buffer);
    return texture;
}

void GPUContext::destroyGPUTexture(SDL_GPUTexture* texture)
{
    if (!gpu) {
        throw GPUException("GPU device is not initialized");
    }
    if (texture) {
        SDL_ReleaseGPUTexture(gpu, texture);
    }
}

void GPUContext::updateGPUTexture(SDL_GPUTexture* texture, const ppl7::grafix::Drawable& surface)
{
    if (!gpu) {
        throw GPUException("GPU device is not initialized");
    }
    // Daten in GPU hochladen
    SDL_GPUTransferBufferCreateInfo  transfer_info = {
        .size = (Uint32)surface.width() * (Uint32)surface.height() * 4,  // RGBA8 = 4 Bytes/Pixel
    };
    SDL_GPUTransferBuffer* transfer_buffer = SDL_CreateGPUTransferBuffer(gpu, &transfer_info);
    if (!transfer_buffer) {
        throw GPUException("SDL_CreateGPUTransferBuffer failed: %s", SDL_GetError());
    }

    // Pixel-Daten kopieren
    void* mapped = SDL_MapGPUTransferBuffer(gpu, transfer_buffer, false);
    memcpy(mapped, surface.adr(), surface.width() * surface.height() * 4);
    SDL_UnmapGPUTransferBuffer(gpu, transfer_buffer);

    // Mit Command Buffer zur GPU transferieren
    SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(gpu);
    SDL_GPUCopyPass* copy_pass = SDL_BeginGPUCopyPass(cmd);
    SDL_GPUTextureTransferInfo transfer_region = {
        .transfer_buffer = transfer_buffer,
        .offset = 0
    };
    SDL_GPUTextureRegion texture_region = {
        .texture = texture,
        .mip_level = 0,
        .layer = 0,
        .x = 0,
        .y = 0,
        .z = 0,
        .w = (Uint32)surface.width(),
        .h = (Uint32)surface.height(),
        .d = 1
    };
    SDL_UploadToGPUTexture(copy_pass, &transfer_region, &texture_region, false);
    SDL_EndGPUCopyPass(copy_pass);
    SDL_SubmitGPUCommandBuffer(cmd);
    SDL_ReleaseGPUTransferBuffer(gpu, transfer_buffer);
}

void GPUContext::clearQueues()
{
    primitiveCommands.clear();
    spriteCommands.clear();
}

void GPUContext::startRenderPass(SDL_GPURenderPass* pass)
{
    this->pass = pass;
    z = 0.0f;
    primitiveCommands.clear();
    spriteCommands.clear();
}

void GPUContext::drawSprite(const SpriteTexture& sprite, int sprite_id, float x, float y, float scale_x, float scale_y, float angle, const ppl7::grafix::Color& color_modulation)
{
    SpriteCommand cmd(&sprite, sprite_id, x, y, z, scale_x, scale_y, angle, color_modulation);
    z += 0.0001f; // Slightly increase Z to ensure correct layering
    spriteCommands[sprite.getUniqueTextureId(sprite_id)].push_back(cmd);
}

void GPUContext::drawLine(float x1, float y1, float x2, float y2, const ppl7::grafix::Color& color, float thickness)
{
    PrimitiveCommand cmd(PrimitiveCommand::Type::Line, x1, y1, x2, y2, color, thickness);
    primitiveCommands.push_back(cmd);
}

void GPUContext::drawRect(float x, float y, float w, float h, const ppl7::grafix::Color& color, float thickness)
{
    PrimitiveCommand cmd(PrimitiveCommand::Type::Rect, x, y, w, h, color);
    primitiveCommands.push_back(cmd);
}
void GPUContext::fillRect(float x, float y, float w, float h, const ppl7::grafix::Color& color)
{
    PrimitiveCommand cmd(PrimitiveCommand::Type::FilledRect, x, y, w, h, color);
    primitiveCommands.push_back(cmd);
}

void GPUContext::endRenderPass()
{
    // Nothing to do here for now
}
