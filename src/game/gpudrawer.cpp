#include "gpu.h"

GPUDrawer::GPUDrawer()
{
    gpu = NULL;
    pass = NULL;
    z = 0.0f;
}

GPUDrawer::~GPUDrawer()
{
}

void GPUDrawer::startRenderPass(SDL_GPUDevice* gpu, SDL_GPURenderPass* pass)
{
    this->gpu = gpu;
    this->pass = pass;
    z = 0.0f;
}

void GPUDrawer::drawSprite(const SpriteTexture& sprite, int sprite_id, float x, float y, float scale_x, float scale_y, float angle, const ppl7::grafix::Color& color_modulation)
{
    SpriteCommand cmd(&sprite, sprite_id, x, y, z, scale_x, scale_y, angle, color_modulation);
    z += 0.0001f; // Slightly increase Z to ensure correct layering
    spriteCommands[sprite.getUniqueTextureId(sprite_id)].push_back(cmd);
}

void GPUDrawer::drawLine(float x1, float y1, float x2, float y2, const ppl7::grafix::Color& color, float thickness)
{
    PrimitiveCommand cmd(PrimitiveCommand::Type::Line, x1, y1, x2, y2, color, thickness);
    primitiveCommands.push_back(cmd);
}

void GPUDrawer::drawRect(float x, float y, float w, float h, const ppl7::grafix::Color& color, float thickness)
{
    PrimitiveCommand cmd(PrimitiveCommand::Type::Rect, x, y, w, h, color);
    primitiveCommands.push_back(cmd);
}
void GPUDrawer::fillRect(float x, float y, float w, float h, const ppl7::grafix::Color& color)
{
    PrimitiveCommand cmd(PrimitiveCommand::Type::FilledRect, x, y, w, h, color);
    primitiveCommands.push_back(cmd);
}

void GPUDrawer::endRenderPass()
{
    // Nothing to do here for now
}
