#ifndef INCLUDE_LIGHTMAP_H_
#define INCLUDE_LIGHTMAP_H_

class SpriteTexture;
class LightLayer;

enum class LightType {
    Static=0,
    Fire
};

class LightObject {
public:
    LightObject();
    LightLayer* lightsystem;
    int id;
    int x;			// 2 Byte
    int y;			// 2 Byte
    float scale_x;	// 4 Byte
    float scale_y;  // 4 Byte
    float angle;    // 4 Byte
    int sprite_no;	// 2 Byte
    uint8_t color_index; // 1 Byte
    uint8_t intensity;   // 1 Byte
    LightType   type;   // 1 Byte ==> 21 Byte
    ppl7::grafix::Rect boundary;
};


class LightLayer
{
public:

private:
    int maxid;
    const ColorPalette& palette;
    ppl7::Mutex mutex;
    std::map<int, LightObject> light_list;
    std::map<uint32_t, const LightObject&> visible_lights_map;
    SpriteTexture* spriteset, * tex_object;
    bool bLightsVisible;

public:
    LightLayer(const ColorPalette& palette);
    ~LightLayer();

    void clear();
    void setSpriteTexture(SpriteTexture* texture);
    void addLight(int x, int y, int sprite_no, float scale_x, float scale_y, float angle, uint8_t color_index, uint8_t intensity, LightType type);
    void deleteLight(int id);
    void modifyLight(const LightObject& item);
    void setVisible(bool visible);
    bool isVisible() const;
    void setSpriteset(SpriteTexture* spriteset, SpriteTexture* objects);
    void updateVisibleLightList(const ppl7::grafix::Point& worldcoords, const ppl7::grafix::Rect& viewport);
    void draw(SDL_Renderer* renderer, const ppl7::grafix::Rect& viewport, const ppl7::grafix::Point& worldcoords) const;
    void drawObjects(SDL_Renderer* renderer, const ppl7::grafix::Rect& viewport, const ppl7::grafix::Point& worldcoords) const;
    void save(ppl7::FileObject& file, unsigned char id) const;
    void load(const ppl7::ByteArrayPtr& ba);
    bool findMatchingLight(const ppl7::grafix::Point& p, LightObject& light) const;
    void drawSelectedLightOutline(SDL_Renderer* renderer, const ppl7::grafix::Rect& viewport, const ppl7::grafix::Point& worldcoords, int id);
    void drawSelectedLightObject(SDL_Renderer* renderer, const ppl7::grafix::Rect& viewport, const ppl7::grafix::Point& worldcoords, int id);

    size_t count() const;
    size_t countVisible() const;
};


#endif // INCLUDE_LIGHTMAP_H_
