#include <string.h>
#include <stdlib.h>
#include "texmaker.h"

static int texid=0;

FreeSpace::FreeSpace()
{

}

FreeSpace::FreeSpace(int x, int y, int width, int height)
{
	r.x1=x;
	r.y1=y;
	r.setWidth(width);
	r.setHeight(height);
}

FreeSpace* FreeSpace::findSmallestMatch(const ppl7::grafix::Size& size)
{
	if (mySpaces.size() == 0) {
		if (size.width <= r.width() && size.height <= r.height()) {
			return this;
		}
		return NULL;
	}
	FreeSpace* best_match=NULL;
	std::list<FreeSpace>::iterator it;
	for (it=mySpaces.begin();it != mySpaces.end();++it) {
		FreeSpace* fs=(*it).findSmallestMatch(size);
		if (fs) {
			if (best_match) {
				if (fs->r.width() * fs->r.height() < best_match->r.width() * best_match->r.height()) best_match=fs;
			} else best_match=fs;
		}
	}
	return best_match;
}

ppl7::grafix::Rect FreeSpace::occupy(const ppl7::grafix::Size& size)
{
	FreeSpace s1(r.x1, r.y1, size.width, size.height);  // Hier geht das Sprite rein
	FreeSpace s2(r.x1 + size.width, r.y1, r.width() - size.width, size.height);  // right to sprite
	FreeSpace s3(r.x1, r.y1 + size.height, r.width(), r.height() - size.height);  // under sprite
	//if (s2.r.width() > 0 && s2.r.height() > 0) mySpaces.push_back(s2);
	//if (s3.r.width() > 0 && s3.r.height() > 0) mySpaces.push_back(s3);
	mySpaces.push_back(s2);
	mySpaces.push_back(s3);

	return ppl7::grafix::Rect(r.x1, r.y1, size.width, size.height);
}

bool FreeSpace::findMatch(const ppl7::grafix::Size& size, ppl7::grafix::Rect& found)
{

	FreeSpace* best=findSmallestMatch(size);
	if (best) {
		found=best->occupy(size);
		return true;
	}
	return false;

	if (mySpaces.size() == 0) {
		if (size.width <= r.width() && size.height <= r.height()) {
			// passt
			// wir teilen das Rechteck auf in 4 Unterteile
			FreeSpace s1(r.x1, r.y1, size.width, size.height);  // Hier geht das Sprite rein
			FreeSpace s2(r.x1 + size.width, r.y1, r.width() - size.width, size.height);  // right to sprite
			FreeSpace s3(r.x1, r.y1 + size.height, r.width(), r.height() - size.height);  // under sprite
			//if (s2.r.width() > 0 && s2.r.height() > 0) mySpaces.push_back(s2);
			//if (s3.r.width() > 0 && s3.r.height() > 0) mySpaces.push_back(s3);
			mySpaces.push_back(s2);
			mySpaces.push_back(s3);

			found=s1.r;
			return true;
		}
		return false;
	}
	std::list<FreeSpace>::iterator it;
	for (it=mySpaces.begin();it != mySpaces.end();++it) {
		if ((*it).findMatch(size, found)) return true;
	}
	return false;
}

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
	spaces.r.setWidth(width);
	spaces.r.setHeight(height);

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
	ppl7::grafix::ImageFilter_BMP bmp;
	bmp.saveFile(filename, texture);
}


size_t Texture::findSmallestMatch(const ppl7::grafix::Size& size)
{
	size_t bytes=0;
	FreeSpace* fs=spaces.findSmallestMatch(size);
	if (fs) bytes=fs->r.width() * fs->r.height();
	return bytes;
}

int Texture::add(const ppl7::grafix::Drawable& surface, const ppl7::grafix::Rect& r, ppl7::grafix::Rect& tgt)
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

int Texture::add(const ppl7::grafix::Drawable& surface, ppl7::grafix::Rect& tgt)
{
	if (spaces.findMatch(surface.size(), tgt)) {
		texture.blt(surface, tgt.x1, tgt.y1);
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
