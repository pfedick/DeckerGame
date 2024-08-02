#ifndef INCLUDE_LIGHTMAP_H_
#define INCLUDE_LIGHTMAP_H_

#include "decker_sdl.h"
class SpriteTexture;
class LightLayer;

enum class LightType {
    Static=0,
    Fire,
    Flicker,
    Fade,
    Candle
};


enum class LightPlaneId {
    Near=6,
    Player=0,
    Middle=4,
    Far=2,
    Horizon=5,
    Max=7
};

enum class LightPlayerPlaneMatrix {
    None=0,
    Front=1,
    Player=2,
    Back=4,
    All=7
};

class LightSystem;

class LightObject {
    friend class LightSystem;
private:
    float current_intensity;
    float target_intensity;
    double next_change;
    float intensity_increment;
    int animation_state;


    void updateFire(double time, float frame_rate_compensation);
    void updateCandle(double time, float frame_rate_compensation);
    void updateFlicker(double time, float frame_rate_compensation);
    void updateFade(double time, float frame_rate_compensation);

public:
    LightObject();
    virtual ~LightObject();
    size_t save_size;
    LightLayer* lightsystem;
    uint32_t id;    // 4 Byte
    int x;			// 4 Byte
    int y;			// 4 Byte
    float scale_x;	// 4 Byte
    float scale_y;  // 4 Byte
    float angle;    // 4 Byte
    int sprite_no;	// 2 Byte
    //uint8_t color_index; // 1 Byte
    ppl7::grafix::Color color;
    uint8_t intensity;   // 1 Byte
    uint8_t flare_intensity;   // 1 Byte
    LightType   myType;   // 1 Byte
    uint8_t     plane;   // 1 Byte
    uint8_t     playerPlane; // 1 Byte
    uint8_t     flarePlane; // 1 Byte
    float       typeParameter; // 4 Byte, used for light type
    SpriteTexture* custom_texture;

    ppl7::grafix::Rect boundary;
    bool enabled;
    bool initial_state;     // Flags 1 Byte
    bool has_lensflare;     // Flags
    bool flare_useLightColor;

    virtual size_t save(unsigned char* buffer, size_t size) const;
    virtual size_t load(const unsigned char* buffer, size_t size);
    virtual void trigger();

    void update(double time, float frame_rate_compensation);
};

class ColorPalette;

class LightSystem
{
private:
    uint32_t nextid;
    std::map<uint32_t, LightObject*> light_map;
    std::map<uint32_t, LightObject*> visible_light_map[static_cast<int>(LightPlaneId::Max)];
    SpriteTexture* lightmaps, * light_objects, * lensflares;
    double time;
    float frame_rate_compensation;
    bool visibility[static_cast<int>(LightPlaneId::Max)];
    void updatePlane(LightPlaneId plane, double time, float frame_rate_compensation);

public:
    LightSystem();
    ~LightSystem();
    void loadSpritesets(SDL& sdl);
    void clear();
    void update(double time, float frame_rate_compensation);
    void updateVisibleLightList(const ppl7::grafix::Point& worldcoords, const ppl7::grafix::Rect& viewport);
    void save(ppl7::FileObject& file, unsigned char id) const;
    void load(const ppl7::ByteArrayPtr& ba);

    void loadLegacyLightLayer(const ppl7::ByteArrayPtr& ba, LightPlaneId plane, int pplane);

    void addLight(LightObject* light);
    void addObjectLight(LightObject* light);
    LightObject* getLight(uint32_t light_id);
    void deleteLight(uint32_t light_id);

    size_t count() const;
    size_t countVisible() const;
    void setVisible(LightPlaneId plane, bool visible);

    void draw(SDL_Renderer* renderer, const ppl7::grafix::Rect& viewport, const ppl7::grafix::Point& worldcoords, LightPlaneId plane, LightPlayerPlaneMatrix pplane=LightPlayerPlaneMatrix::None) const;
    void drawEditMode(SDL_Renderer* renderer, const ppl7::grafix::Rect& viewport, const ppl7::grafix::Point& worldcoords, LightPlaneId plane) const;
    void drawLensFlares(SDL_Renderer* renderer, const ppl7::grafix::Rect& viewport, const ppl7::grafix::Point& worldcoords, LightPlaneId plane, LightPlayerPlaneMatrix pplane=LightPlayerPlaneMatrix::None) const;

    void drawSelectedLight(SDL_Renderer* renderer, const ppl7::grafix::Rect& viewport, const ppl7::grafix::Point& worldcoords, int id);

    LightObject* findMatchingLight(const ppl7::grafix::Point& p, LightPlaneId plane);
};


#endif // INCLUDE_LIGHTMAP_H_
