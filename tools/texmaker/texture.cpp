#include <string.h>
#include <stdlib.h>
#include "texmaker.h"

static int texid=0;

Texture::Texture(int width, int height)
{
	this->width=width;
	this->height=height;
	texture.create(width, height, ppl7::grafix::RGBFormat::A8R8G8B8);
	x=0;
	y=0;
	maxy=0;
	texid++;
	id=texid;
}

Texture::~Texture()
{

}

int Texture::GetId()
{
	return id;
}

void Texture::SaveTexture(const char* filename) const
{
	ppl7::grafix::ImageFilter_PNG png;
	png.saveFile(filename, texture);
}


int Texture::Add(const ppl7::grafix::Drawable& surface, const ppl7::grafix::Rect& r, ppl7::grafix::Rect& tgt)
{
	int w=r.width();
	int h=r.height();
	if (x + w <= width && y + h <= height) {
		// Passt an dieser Stelle
		tgt.x1=x;
		tgt.y1=y;
		tgt.x2=x + w;
		tgt.y2=y + h;
		texture.blt(surface, r, x, y);
		x=x + w;
		if (y + h > maxy) maxy=y + h;
		return 1;
	}
	// Vielleicht in die nächste Reihe?
	if (maxy + h <= height) {
		x=0;
		y=maxy;
		tgt.x1=x;
		tgt.y1=y;
		tgt.x2=x + w;
		tgt.y2=y + h;
		texture.blt(surface, r, x, y);
		x=x + w;
		if (y + h > maxy) maxy=y + h;
		return 1;
	}
	return 0;
}

ppl7::PFPChunk* Texture::MakeChunk() const
{
	// Sprite-Daten komprimieren
	ppl7::ByteArray comp;

	//ppl7::CompressBZip2(comp, texture);
	//printf("Size with bzip2: %10zd, ", comp.size());
	ppl7::CompressZlib(comp, texture);
	//printf("zlib: %10zd\n", comp.size());
	// Wir benötigen auch noch ein paar Byte für den Chunk-Header
	// Byte 0: TextureId			(2 Byte)
	// Byte 2: RGBFormat			(1 Byte)
	// Byte 3: Bitdepth				(1 Byte)
	// Byte 4: width				(2 Byte)
	// Byte 6: height				(2 Byte)
	// Byte 8: Beginn der komprimierten Daten
	char* buffer=(char*)malloc(comp.size() + 8);
	ppl7::Poke16(buffer + 0, id);
	ppl7::Poke8(buffer + 2, 9);	// Farbformat
	ppl7::Poke8(buffer + 3, 32);	// Bitdepth
	ppl7::Poke16(buffer + 4, texture.width());
	ppl7::Poke16(buffer + 6, texture.height());
	memcpy(buffer + 8, comp.adr(), comp.size());

	ppl7::PFPChunk* chunk=new ppl7::PFPChunk;
	chunk->setName("SURF");
	chunk->setData(buffer, comp.size() + 8);
	free(buffer);
	return chunk;
}
