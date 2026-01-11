#ifndef DECKER_SPRITE_H
#define DECKER_SPRITE_H
#include <SDL3/SDL.h>
#include <ppl7.h>
#include <ppl7-grafix.h>
#include <map>
#include "gpu.h"

/*!\page PFPSpriteTexture1 Format PFP Sprite-Textures, Version 1
 *
Eine Texture-Sprite-Datei wird zum Speichern von Sprites verwendet, die mit der Klasse ppl7::grafix::Sprite
geladen und dargestellt werden können. Dabei werden viele einzelne kleine Sprite-Grafiken auf einer
oder mehreren größeren Texturen zusammengefasst.
Die Datei verwendet als Basisformat das \ref PFPFileVersion3 "PFP-File-Format Version 3" mit seinen Chunks.
Das File trägt als ID "TEXS", Hauptversion 1, Unterversion 0. Eine Texturedatei enthält immer einen
Index-Chunk (INDX) und beliebig viele Surface-Chunks (SURF).

\par INDX: Index-Chunk

Der INDX-Chunk beginnt mit einem 4-Byte-Wert im Little Endian Format, der angibt, wieviele Sprites
im Index vorhanden sind.
\code
Byte 0:  Anzahl Sprites                               4 Byte
\endcode

Darauf folgt dann pro Sprite ein 22 Byte langer Block mit folgenden Daten:
\code
Byte 0:  ItemId                                       4 Byte
Byte 4:  TextureId                                    2 Byte
Byte 6:  TextureRect                                  8 Byte
         Byte 6:  left          (2 Byte)
         Byte 8:  top           (2 Byte)
         Byte 10: right         (2 Byte)
         Byte 12: bottom        (2 Byte)
Byte 14: Pivot-Punkt                                  4 Byte
         Byte 14: x-Koordinate  (2 Byte)
         Byte 16: y-Koordinate  (2 Byte)
Byte 18: Offset                                       4 Byte
         Byte 18: x-Koordinate  (2 Byte)
         Byte 20: y-Koordinate  (2 Byte)
\endcode
Sämtliche Werte müssen im Little-Endian-Format angegeben werden.

Beschreibung:
<ul>
<li><b>ItemId</b>\n
Jedes Sprite hat seine eigene ID. Diese muss nicht zwangsläufig ein fortlaufender Zähler sein, sondern
kann eine beliebige Ziffer sein. Sie muss nur eindeutig sein.
</li>
<li><b>TextureId</b>\n
Eine Sprite-Datei kann mehrere Texturen enthalten. Dieser Wert stellt eine Referenz auf die
ID der Textur dar.
</li>
<li><b>TextureRect</b>\n
Da pro Textur mehrere Sprites gespeichert werden, müssen dessen Koordinaten innerhalb der Textur
festgehalten werden. Diese geschieht innerhalb dieses Datenblocks, bei dem jeder Wert mit 2 Byte
im Little-Endian Format dargestellt wird.
</li>
<li><b>Pivot-Punkt</b>\n
Der Pivot-Punkt oder auch Ursprung ist die Position innerhalb des Sprites, aber der mit dem Zeichnen
begonnen wird. Bei einer Figur könnte dies z.B. der Mittelpunkt der Füße sein, bei einem Baum das
unterste Ende des Stamms.
</li>
<li><b>Offset</b>\n
Falls das Sprite in der Quellgrafik nicht bündig mit der ersten Pixelreihe bzw. Pixelspalte
beginnt, wird der sogenannte Offset gespeichert. Er gibt ganz einfach Spalte und Zeile an, ab der
das Sprite beginnt.
</li>
</ul>
\par SURF: Surface-Chunk
Der Surface-Chunk setzt sich aus einem 8 Byte langem Header, gefolgt von den Zlib-komprimierten
Pixel-Daten zusammen:

\code
Byte 0:  TextureId                                    2 Byte
Byte 2:  RGB-Format                                   1 Byte
Byte 3:  Bittiefe                                     1 Byte
Byte 4:  Breite                                       2 Byte
Byte 6:  Höhe                                         2 Byte
\endcode
Beschreibung:
<ul>
<li><b>TextureId</b>\n
Eindeutige ID der Textur, in der Regel eine fortlaufende Nummer. Wird als Referenz im INDX-Chunk
verwendet.
</li>
<li><b>RGB-Format</b>\n
Eine eindeutige Nummer, die das Farbformat der Grafikdaten angibt. Folgende Formate sind definiert:
- 9: A8R8G8B8
</li>
<li><b>Bittiefe</b>\n
Die Bittiefe eines einzelnen Farbwertes. Ist abhängig vom RGB-Format und in der Regel 32 Bit.
</li>
<li><b>Breite/Höhe</b>\n
Gibt Breite und Höhe der Grafik in Pixel an. Texturen unterliegen gewissen Einschränkungen, daher
sollten Höhe und Breite möglichst identisch und ein Vielfaches von 2 sein, also beispielsweise 32, 64,
128, 256, 512, 1024, 2048. Die meisten Grafikkarten unterstützen Texturen bis zu einer Größe von 4096 x 4096
Pixel.
</li>
</ul>

Die Pixel-Daten werden mit der Funktion ppl6::CCompression::CompressZlib komprimiert. Sie beginnen daher
mit einem 9-Byte langen Header, gefolgt von den eigentlichen Zlib-komprimierten Daten:
\code
Byte 0:  Komprimierunsart, ist immer 1 für Zlib       1 Byte
Byte 1:  Anzahl Bytes unkomprimiert (litte endian)    4 Byte
Byte 5:  Anzahl Bytes komprimiert (litte endian)      4 Byte
Byte 9:  Beginn des komprimierten Datenstroms
\endcode


\par NORM: Normal-Map-Chunk (optional)
Ist genauso aufgebaut, wie der SURF-Chunk, enthält jedoch anstelle der Farbwerte
die Normalenvektoren für jedes Pixel. Diese werden benötigt, wenn
Licht- und Schatteneffekte auf die Sprites angewendet werden sollen.

\par SPEC: Specular-Map-Chunk (optional)
Ist genauso aufgebaut, wie der SURF-Chunk, enthält jedoch anstelle der Farbwerte
die Specular-Werte für jedes Pixel. Diese werden benötigt, wenn
Licht- und Schatteneffekte auf die Sprites angewendet werden sollen.
 */

class SpriteTexture
{
public:
    class SpriteIndexItem
    {
    public:
        int id;
        int textureId;
        SDL_GPUTexture* tex;
        const ppl7::grafix::Drawable* drawable;
        SDL_FRect r;
        ppl7::grafix::Point Pivot;
        ppl7::grafix::Point Offset;

        SpriteIndexItem()
        {
            id = 0;
            textureId = 0;
            tex = NULL;
            drawable = NULL;
        }
        SpriteIndexItem(const SpriteIndexItem& other)
            :r(other.r), Pivot(other.Pivot), Offset(other.Offset)
        {
            id = other.id;
            textureId = other.textureId;
            tex = other.tex;
            drawable = other.drawable;
        }
    };
private:
    GPUContext* gpu;
    std::map<int, SDL_GPUTexture*> TextureMap;
    std::map<int, SDL_GPUTexture*> NormalMap;
    std::map<int, SDL_GPUTexture*> SpecularMap;
    std::map<int, ppl7::grafix::Image> InMemoryTextureMap;
    std::map<int, SpriteIndexItem> SpriteList;

    SDL_GPUTexture* current_outline_texture;
    uint64_t base_texture_id;
    int current_outline_sprite_id;

    bool bSDLBufferd;
    bool bMemoryBufferd;
    bool bOutlinesEnabled;
    bool bCollisionDetectionEnabled;
    bool bHasNormals;
    bool bHasSpeculars;
    SDL_BlendMode defaultBlendMode;

    ppl7::grafix::Image loadTexture(ppl7::PFPChunk* chunk, const ppl7::grafix::Color& tint);
    void loadIndex(ppl7::PFPChunk* chunk);
    SDL_GPUTexture* postGenerateOutlines(int sprite_id);
    SDL_GPUTexture* findTexture(int id) const;
    const ppl7::grafix::Drawable* findInMemoryTexture(int id) const;

public:
    SpriteTexture();
    ~SpriteTexture();
    void load(GPUContext& gpu, const ppl7::String& filename, const ppl7::grafix::Color& tint = ppl7::grafix::Color());
    void load(GPUContext& gpu, ppl7::FileObject& ff, const ppl7::grafix::Color& tint = ppl7::grafix::Color());
    void clear();
    void draw(ppl7::grafix::Drawable& target, int x, int y, int id) const;
    void draw(ppl7::grafix::Drawable& target, int x, int y, int id, const ppl7::grafix::Color& color_modulation) const;

    void draw(GPUContext& gpu, int x, int y, int id) const;
    void draw(GPUContext& gpu, int x, int y, int id, const ppl7::grafix::Color& color_modulation) const;
    void draw(GPUContext& gpu, int x, int y, int id, const SDL_Color& color_modulation) const;
    void drawBoundingBox(GPUContext& gpu, int x, int y, int id) const;
    void drawBoundingBoxWithAngle(GPUContext& gpu, int x, int y, int id, float scale_x, float scale_y, float angle) const;
    void draw(GPUContext& gpu, int id, const SDL_FRect& source, const SDL_FRect& target) const;
    void drawScaled(GPUContext& gpu, int x, int y, int id, float scale_factor) const;
    void drawScaled(GPUContext& gpu, int x, int y, int id, float scale_factor, const ppl7::grafix::Color& color_modulation) const;
    void drawScaledWithAngle(GPUContext& gpu, int x, int y, int id, float scale_x, float scale_y, float angle, const ppl7::grafix::Color& color_modulation) const;
    void drawOutlines(GPUContext& gpu, int x, int y, int id, float scale_factor);
    void drawOutlinesWithAngle(GPUContext& gpu, int x, int y, int id, float scale_x, float scale_y, float angle);


    ppl7::grafix::Size spriteSize(int id, float scale_factor) const;
    ppl7::grafix::Rect spriteBoundary(int id, float scale_factor, int x, int y) const;
    ppl7::grafix::Rect spriteBoundary(int id, float scale_factor_x, float scale_factor_y, float rotation, int x, int y) const;
    const ppl7::grafix::Drawable getDrawable(int id) const;
    void enableMemoryBuffer(bool enabled);
    void enableSDLBuffer(bool enabled);
    void enableCollisionDetection(bool enabled);
    void enableOutlines(bool enabled);
    int numTextures() const;
    int numSprites() const;
    void setTextureBlendMode(SDL_BlendMode blendMode);
    SDL_BlendMode getTextureBlendMode() const;
    void setPivot(int id, int x, int y);

    SDL_FRect getSpriteSource(int id) const;
    const SpriteIndexItem* getSpriteIndex(int id) const;
    ppl7::grafix::Point spriteOffset(int id) const;

    uint64_t getUniqueTextureId(int id) const;
};

#endif // DECKER_SPRITE_H