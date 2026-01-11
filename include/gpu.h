#ifndef INCLUDE_GPU_H
#define INCLUDE_GPU_H

#include <SDL3/SDL.h>

#include <ppl7.h>

class SpriteTexture;

class GPUException : public ppl7::Exception
{
public:
    using ppl7::Exception::Exception;

    GPUException(const char* msg, ...) noexcept {
        va_list args;
        va_start(args, msg);
        copyText(msg, args);
        va_end(args);
    }


    const char* what() const noexcept override {
        return "GPUException";
    }
};



/*!\class GPUContext gpu.h include/gpu.h
 * \brief Klasse zum Zeichnen von Sprites und Grafikprimitive auf einem GPU-Gerät
 *
 * Die Klasse GPUContext ermöglicht das Zeichnen von Sprites und Grafikprimitive
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
    GPUContext gpu_ctx(gpu);
    gpu_ctx.startRenderPass(gpu, pass);
    gpu_ctx.drawSprite(spriteTexture, spriteId, x, y, scaleX, scaleY, angle, colorModulation);
    gpu_ctx.endRenderPass();
\endcode
 */
class GPUContext
{
private:
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


    SDL_Window* window;
public:
    // Es wäre besser, wenn dass hier ein GPUContext wäre, den wir anstelle von
    // SDL_Renderer verwenden könnten.

    SDL_GPUDevice* gpu;

    GPUContext();
    ~GPUContext();

    void init(SDL_Window* window);
    void shutdown();

    void initGPUDevice();
    SDL_GPUTexture* createGPUTexture(const ppl7::grafix::Drawable& surface);
    void destroyGPUTexture(SDL_GPUTexture* texture);
    void updateGPUTexture(SDL_GPUTexture* texture, const ppl7::grafix::Drawable& surface);

    void clearQueues(); // temporary?
    void startRenderPass(SDL_GPURenderPass* pass);
    void endRenderPass();

    void drawSprite(const SpriteTexture& sprite, int sprite_id, float x, float y, float scale_x = 1.0f, float scale_y = 1.0f, float angle = 0.0f, const ppl7::grafix::Color& color_modulation = ppl7::grafix::Color(255, 255, 255, 255));
    void drawLine(float x1, float y1, float x2, float y2, const ppl7::grafix::Color& color, float thickness = 1.0f);
    void drawRect(float x, float y, float w, float h, const ppl7::grafix::Color& color, float thickness = 1.0f);
    void fillRect(float x, float y, float w, float h, const ppl7::grafix::Color& color);

};

GPUContext& getGlobalGPUContext();

#endif // INCLUDE_GPU_H

