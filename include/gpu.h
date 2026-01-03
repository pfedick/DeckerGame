#ifndef INCLUDE_GPU_H
#define INCLUDE_GPU_H

#include <SDL3/SDL.h>
#include "sprite.h"

/*!\class GPUDrawer gpu.h include/gpu.h
 * \brief Klasse zum Zeichnen von Sprites und Grafikprimitive auf einem GPU-Gerät
 *
 * Die Klasse GPUDrawer ermöglicht das Zeichnen von Sprites und Grafikprimitive
 * auf einem SDL_GPUDevice. Dabei wird ein Render-Pass verwendet, der mit
 * startRenderPass() gestartet und mit endRenderPass() beendet wird.
 * Die Klasse unterstützt das Zeichnen von Sprites mit Skalierung, Rotation
 * und Farbmodulation.
 *
 * Alle Zeichenoperationen werden zunächst gesammelt und erst beim Beenden des Render-Passes
 * ausgeführt, um die Performance zu optimieren.
 * Sprites werden dabei nach Textur gruppiert, um die Anzahl der notwendigen
 * Texturwechsel zu minimieren.
 *
 * Beispiel:
 \code
    SDL_GPUDevice* gpu = SDL_GPU_CreateDevice(...);
    SDL_GPURenderPass* pass = SDL_GPU_CreateRenderPass(gpu, ...);
    GPUDrawer drawer;
    drawer.startRenderPass(gpu, pass);
    drawer.drawSprite(spriteTexture, spriteId, x, y, scaleX, scaleY, angle, colorModulation);
    drawer.endRenderPass();
\endcode
 */
class GPUDrawer
{
private:
    SDL_GPUDevice* gpu;
    SDL_GPURenderPass* pass;
    float z;

    class PrimitiveCommand
    {
    public:
        enum class Type {
            Line,
            Rect,
            FilledRect
        };

        Type type;
        float x1, y1, x2, y2;
        float w, h;
        ppl7::grafix::Color color;
        float thickness;

        PrimitiveCommand(Type type, float x1, float y1, float x2, float y2, const ppl7::grafix::Color& color, float thickness)
            : type(type), x1(x1), y1(y1), x2(x2), y2(y2), color(color), thickness(thickness) {
        }

        PrimitiveCommand(Type type, float x, float y, float w, float h, const ppl7::grafix::Color& color)
            : type(type), x1(x), y1(y), w(w), h(h), color(color), thickness(0.0f) {
        }
    };

    class SpriteCommand
    {
    public:
        const SpriteTexture* sprite;
        int sprite_id;
        float x, y, z;
        float scale_x, scale_y;
        float angle;
        ppl7::grafix::Color color_modulation;

        SpriteCommand(const SpriteTexture* sprite, int sprite_id, float x, float y, float z, float scale_x, float scale_y, float angle, const ppl7::grafix::Color& color_modulation)
            : sprite(sprite), sprite_id(sprite_id), x(x), y(y), z(z), scale_x(scale_x), scale_y(scale_y), angle(angle), color_modulation(color_modulation) {
        }
    };

    std::list<PrimitiveCommand> primitiveCommands;
    std::map<uint64_t, std::list<SpriteCommand>> spriteCommands;



public:
    GPUDrawer();
    ~GPUDrawer();

    void startRenderPass(SDL_GPUDevice* gpu, SDL_GPURenderPass* pass);
    void endRenderPass();
    void drawSprite(const SpriteTexture& sprite, int sprite_id, float x, float y, float scale_x = 1.0f, float scale_y = 1.0f, float angle = 0.0f, const ppl7::grafix::Color& color_modulation = ppl7::grafix::Color(255, 255, 255, 255));

    void drawLine(float x1, float y1, float x2, float y2, const ppl7::grafix::Color& color, float thickness = 1.0f);
    void drawRect(float x, float y, float w, float h, const ppl7::grafix::Color& color, float thickness = 1.0f);
    void fillRect(float x, float y, float w, float h, const ppl7::grafix::Color& color);

};

#endif // INCLUDE_GPU_H

