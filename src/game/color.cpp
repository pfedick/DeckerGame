#include "decker.h"
#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <ppl7-grafix.h>


ColorPaletteItem::ColorPaletteItem()
{
    ldraw_material=0;
}

ColorPaletteItem::ColorPaletteItem(const ppl7::grafix::Color& color, const ppl7::String& name, int ldraw_material)
{
    this->color=color;
    this->name=name;
    this->ldraw_material=ldraw_material;
}

void ColorPaletteItem::set(const ppl7::grafix::Color& color, const ppl7::String& name, int ldraw_material)
{
    this->color=color;
    this->name=name;
    this->ldraw_material=ldraw_material;
}

ColorPalette::ColorPalette()
{
    setDefaults();
}

void ColorPalette::setDefaults()
{
    for (int i=0;i < 256;i++) {
        palette[i].set(ppl7::grafix::Color(0, 0, 0, 255), "undefined", 0);
    }

    undefined.set(ppl7::grafix::Color(255, 0, 255, 255), "undefined", 0);
    palette[0].set(ppl7::grafix::Color(0, 0, 0, 255), "Total Black", 0);
    palette[1].set(ppl7::grafix::Color(0xe6, 0xe3, 0xe0, 255), "Light Stone Grey", 151);
    palette[2].set(ppl7::grafix::Color(255, 255, 255, 255), "White", 15);
    palette[3].set(ppl7::grafix::Color(0xa0, 0xa5, 0xa9, 255), "Medium Stone Grey", 71);
    palette[4].set(ppl7::grafix::Color(0x6c, 0x6e, 0x68, 255), "Dark Stone Grey", 72);
    palette[5].set(ppl7::grafix::Color(0x23, 0x78, 0x41, 255), "Green", 2);
    palette[6].set(ppl7::grafix::Color(161, 21, 7, 255), "Red", 4);
    palette[7].set(ppl7::grafix::Color(0xf2, 0xcd, 0x37, 0xff), "Yellow", 14);
    palette[8].set(ppl7::grafix::Color(0x00, 0x55, 0xbf, 0xff), "Blue", 1);
    palette[9].set(ppl7::grafix::Color(14, 25, 33, 0xff), "Black", 0);
    palette[10].set(ppl7::grafix::Color(0x0a, 0x34, 0x63, 0xff), "Dark Blue", 63);
    palette[11].set(ppl7::grafix::Color(0x5a, 0x93, 0xdb, 0xff), "Medium Blue", 43);
    palette[12].set(ppl7::grafix::Color(0x58, 0x39, 0x27, 0xff), "Brown", 6);
    palette[13].set(ppl7::grafix::Color(0x58, 0x2a, 0x12, 0xff), "Reddish Brown", 70);
    palette[14].set(ppl7::grafix::Color(0xfe, 0x8a, 0x18, 0xff), "Orange", 25);
    palette[15].set(ppl7::grafix::Color(0xfa, 0x9c, 0x1c, 0xff), "light Orange", 125);
    palette[16].set(ppl7::grafix::Color(0x95, 0x8a, 0x73, 0xff), "Dark Tan", 28);
    palette[17].set(ppl7::grafix::Color(0xe4, 0xcd, 0x9e, 0xff), "Tan", 19);
    palette[18].set(ppl7::grafix::Color(0x4b, 0x9f, 0x4a, 0xff), "Bright Green", 18);
    palette[19].set(ppl7::grafix::Color(0xbb, 0xe9, 0x0b, 0xff), "Bright Yellow Green (Lime)", 27);
    palette[20].set(ppl7::grafix::Color(0xb3, 0x10, 0x04, 0xff), "Rust", 216);
    palette[21].set(ppl7::grafix::Color(0x72, 0x0e, 0x0f, 0xff), "Dark Red", 320);
    palette[22].set(ppl7::grafix::Color(0x18, 0x46, 0x32, 0xff), "Dark Green", 0);
    palette[23].set(ppl7::grafix::Color(0, 0x8f, 0x9b, 255), "Dark Turquoise", 3);
    palette[24].set(ppl7::grafix::Color(0x4c, 0x4e, 0x48, 255), "Darker Grey", 0);
    palette[25].set(ppl7::grafix::Color(0xf2, 0xf3, 0xf2, 255), "Pearl White", 0);
    palette[26].set(ppl7::grafix::Color(0x6d, 0x6e, 0x5c, 255), "Dark Grey", 10);
}


void ColorPalette::set(uint32_t index, const ppl7::grafix::Color& color, const ppl7::String& name, int ldraw_material)
{
    if (index < 256) palette[index].set(color, name, ldraw_material);

}

void ColorPalette::set(uint32_t index, const ColorPaletteItem& item)
{
    if (index < 256) palette[index]=item;
}

const ColorPaletteItem& ColorPalette::get(uint32_t index) const
{
    if (index < 256) return palette[index];
    return undefined;
}

const ppl7::grafix::Color& ColorPalette::getColor(uint32_t index) const
{
    if (index < 256) return palette[index].color;
    return undefined.color;
}

const std::array<ColorPaletteItem, 256>& ColorPalette::getPalette() const
{
    return palette;
}

void ColorPalette::save(ppl7::FileObject& file, unsigned char id) const
{
    // Format: 0: Size of Chunk (4 Bytes)
    //         4: Chunk ID      (1 Byte)
    //         5: Color Table
    //         n: Finaly 0-Index (2 Byte)
    //
    // Entry:  0: 2-Byte Entry size
    //         2: 2-Byte Index
    //         4: 2-Byte ldraw_material
    //         6: 4-Byte Color: red,green,blue,alpha
    //        10: n-Byte name + finaly 0-Byte
    // Last entry contains only 2-Byte Entry size with value=0
    size_t size=7;
    for (int i=0;i < 256;i++) {
        size+=10 + palette[i].name.size() + 1;
    }
    printf("Total size palette: %zd Byte\n", size);
    ppl7::ByteArray ba;
    unsigned char* buffer=(unsigned char*)ba.malloc(size);
    size_t p=5;
    for (int i=0;i < 256;i++) {
        ppl7::Poke16(buffer + p, 0);
        ppl7::Poke16(buffer + p + 2, i);
        ppl7::Poke16(buffer + p + 4, palette[i].ldraw_material);
        ppl7::Poke8(buffer + p + 6, palette[i].color.red());
        ppl7::Poke8(buffer + p + 7, palette[i].color.green());
        ppl7::Poke8(buffer + p + 8, palette[i].color.blue());
        ppl7::Poke8(buffer + p + 9, palette[i].color.alpha());
        memcpy(buffer + p + 10, palette[i].name.getPtr(), palette[i].name.size());
        ppl7::Poke8(buffer + p + 10 + palette[i].name.size(), 0);
        ppl7::Poke16(buffer + p, 11 + palette[i].name.size());
        p+=11 + palette[i].name.size();
    }
    ppl7::Poke16(buffer + p, 0);
    p+=2;
    ppl7::Poke32(buffer + 0, p);
    ppl7::Poke8(buffer + 4, id);
    file.write(buffer, p);
    printf("real size: %zd\n", p);
}

void ColorPalette::load(const ppl7::ByteArrayPtr& ba)
{
    const unsigned char* buffer=(unsigned char*)ba.ptr();

}
