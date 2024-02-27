#include "decker.h"
#include <stdio.h>
#include <stdlib.h>
#include <ppl7.h>
#include <ppl7-grafix.h>

/*!\page PFPSpriteTexture1 Format PFP Sprite-Textures, Version 1
 *
Eine Texture-Sprite-Datei wird zum Speichern von Sprites verwendet, die mit der Klasse ppl7::grafix::Sprite
geladen und dargestellt werden k�nnen. Dabei werden viele einzelne kleine Sprite-Grafiken auf einer
oder mehreren gr��eren Texturen zusammengefasst.
Die Datei verwendet als Basisformat das \ref PFPFileVersion3 "PFP-File-Format Version 3" mit seinen Chunks.
Das File tr�gt als ID "TEXS", Hauptversion 1, Unterversion 0. Eine Texturedatei enth�lt immer einen
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
S�mtliche Werte m�ssen im Little-Endian-Format angegeben werden.

Beschreibung:
<ul>
<li><b>ItemId</b>\n
Jedes Sprite hat seine eigene ID. Diese muss nicht zwangsl�ufig ein fortlaufender Z�hler sein, sondern
kann eine beliebige Ziffer sein. Sie muss nur eindeutig sein.
</li>
<li><b>TextureId</b>\n
Eine Sprite-Datei kann mehrere Texturen enthalten. Dieser Wert stellt eine Referenz auf die
ID der Textur dar.
</li>
<li><b>TextureRect</b>\n
Da pro Textur mehrere Sprites gespeichert werden, m�ssen dessen Koordinaten innerhalb der Textur
festgehalten werden. Diese geschieht innerhalb dieses Datenblocks, bei dem jeder Wert mit 2 Byte
im Little-Endian Format dargestellt wird.
</li>
<li><b>Pivot-Punkt</b>\n
Der Pivot-Punkt oder auch Ursprung ist die Position innerhalb des Sprites, aber der mit dem Zeichnen
begonnen wird. Bei einer Figur k�nnte dies z.B. der Mittelpunkt der F��e sein, bei einem Baum das
unterste Ende des Stamms.
</li>
<li><b>Offset</b>\n
Falls das Sprite in der Quellgrafik nicht b�ndig mit der ersten Pixelreihe bzw. Pixelspalte
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
Byte 6:  H�he                                         2 Byte
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
Die Bittiefe eines einzelnen Farbwertes. Ist abh�ngig vom RGB-Format und in der Regel 32 Bit.
</li>
<li><b>Breite/H�he</b>\n
Gibt Breite und H�he der Grafik in Pixel an. Texturen unterliegen gewissen Einschr�nkungen, daher
sollten H�he und Breite m�glichst identisch und ein Vielfaches von 2 sein, also beispielsweise 32, 64,
128, 256, 512, 1024, 2048. Die meisten Grafikkarten unterst�tzen Texturen bis zu einer Gr��e von 4096 x 4096
Pixel.
</li>
</ul>

Die Pixel-Daten werden mit der Funktion ppl6::CCompression::CompressZlib komprimiert. Sie beginnen daher
mit einem 9-Byte langen Header, gefolgt von den eigentlichen Zlib-komprimierten Daten:
\code
Byte 0:  Komprimierunsart, ist immer 1 f�r Zlib       1 Byte
Byte 1:  Anzahl Bytes unkomprimiert (litte endian)    4 Byte
Byte 5:  Anzahl Bytes komprimiert (litte endian)      4 Byte
Byte 9:  Beginn des komprimierten Datenstroms
\endcode

 */

using namespace ppl7;




SpriteTexture::SpriteTexture()
{
	bMemoryBufferd=false;
	bOutlinesEnabled=false;
	bCollisionDetectionEnabled=false;
	bSDLBufferd=true;
	defaultBlendMode=SDL_BLENDMODE_BLEND;
}

SpriteTexture::~SpriteTexture()
{
	clear();
}

void SpriteTexture::enableMemoryBuffer(bool enabled)
{
	bMemoryBufferd=enabled;
}

void SpriteTexture::enableSDLBuffer(bool enabled)
{
	bSDLBufferd=enabled;
}

void SpriteTexture::enableCollisionDetection(bool enabled)
{
	bCollisionDetectionEnabled=enabled;
}

void SpriteTexture::enableOutlines(bool enabled)
{
	bOutlinesEnabled=enabled;
}

void SpriteTexture::clear()
{
	std::map<int, SDL_Texture*>::const_iterator it;
	for (it=TextureMap.begin();it != TextureMap.end();++it) {
		SDL_DestroyTexture(it->second);
	}
	for (it=OutlinesTextureMap.begin();it != OutlinesTextureMap.end();++it) {
		SDL_DestroyTexture(it->second);
	}
	TextureMap.clear();
	OutlinesTextureMap.clear();
	InMemoryTextureMap.clear();
	SpriteList.clear();
}

SDL_Texture* SpriteTexture::findTexture(int id) const
{
	if (bSDLBufferd) {
		std::map<int, SDL_Texture*>::const_iterator it;
		it=TextureMap.find(id);
		if (it != TextureMap.end()) return it->second;
	}
	return NULL;
}

SDL_Texture* SpriteTexture::findOutlines(int id) const
{
	if (bOutlinesEnabled) {
		std::map<int, SDL_Texture*>::const_iterator it;
		it=OutlinesTextureMap.find(id);
		if (it != OutlinesTextureMap.end()) return it->second;
	}
	return NULL;
}


const ppl7::grafix::Drawable* SpriteTexture::findInMemoryTexture(int id) const
{
	if (bMemoryBufferd) {
		std::map<int, ppl7::grafix::Image>::const_iterator it;
		it=InMemoryTextureMap.find(id);
		if (it != InMemoryTextureMap.end()) return &it->second;
	}
	return NULL;
}

void SpriteTexture::loadIndex(ppl7::PFPChunk* chunk)
{
	char* buffer=(char*)chunk->data();
	int num=Peek32(buffer);		// Anzahl Einträge in der Tabelle
	char* p=buffer + 4;
	SpriteIndexItem item;
	for (int i=0;i < num;i++) {
		item.id=Peek32(p + 0);
		item.textureId=Peek16(p + 4);
		item.tex=findTexture(item.textureId);
		item.outlines=findOutlines(item.textureId);
		item.drawable=findInMemoryTexture(item.textureId);
		item.r.x=Peek16(p + 6 + 0);
		item.r.y=Peek16(p + 6 + 2);
		item.r.w=Peek16(p + 6 + 4) + 1 - item.r.x;
		item.r.h=Peek16(p + 6 + 6) + 1 - item.r.y;
		item.Pivot.x=Peek16(p + 14 + 0);
		item.Pivot.y=Peek16(p + 14 + 2);
		item.Offset.x=Peek16(p + 18 + 0);
		item.Offset.y=Peek16(p + 18 + 2);
		SpriteList.insert(std::pair<int, SpriteIndexItem>(item.id, item));
		//ppl7::PrintDebugTime("pivot x=%d, y=%d\n", item.Pivot.x, item.Pivot.y);
		p+=22;
	}
}

void SpriteTexture::loadTexture(SDL& sdl, PFPChunk* chunk, const ppl7::grafix::Color& tint)
{
	Compression Comp;
	Comp.usePrefix(Compression::Prefix_V2);
	char* buffer=(char*)chunk->data();

	// Zunächst lesen wir dem Header
	int id=Peek16(buffer + 0);
	ppl7::grafix::RGBFormat rgbformat;
	switch (Peek8(buffer + 2)) {
	case 9: rgbformat=grafix::RGBFormat::A8R8G8B8;
		break;
	default:
		throw grafix::UnsupportedColorFormatException();
	}
	//int bitdepth=Peek8(buffer+3);
	int width=Peek16(buffer + 4);
	int height=Peek16(buffer + 6);

	// Nutzdaten dekomprimieren
	ByteArray uncompressed;
	Comp.uncompress(uncompressed, buffer + 8, chunk->size() - 8);
	buffer=(char*)uncompressed.ptr();

	// Nun erstellen wir ein neues Image
	ppl7::grafix::Image surface;
	surface.create(width, height, rgbformat);
	if (tint.rgb()) {
		//printf ("tint\n");
		for (int y=0;y < height;y++) {
			for (int x=0;x < width;x++) {
				ppl7::grafix::Color c(Peek8(buffer + 2), Peek8(buffer + 1), Peek8(buffer), Peek8(buffer + 3));
				if (c.alpha()) {
					int brightness=c.brightness();
					ppl7::grafix::Color tinted(tint.red() * brightness / 255,
						tint.green() * brightness / 255,
						tint.blue() * brightness / 255,
						c.alpha());
					surface.putPixel(x, y, tinted);
				}
				buffer+=4;
			}
		}
	} else {
		//printf ("no tint\n");
		for (int y=0;y < height;y++) {
			for (int x=0;x < width;x++) {
				ppl7::grafix::Color c(Peek8(buffer + 2), Peek8(buffer + 1), Peek8(buffer), Peek8(buffer + 3));
				surface.putPixel(x, y, c);
				buffer+=4;
			}
		}
	}
	if (bMemoryBufferd) {
		InMemoryTextureMap.insert(std::pair<int, ppl7::grafix::Image>(id, surface));
	}
	if (bOutlinesEnabled) {
		//generateOutlines(sdl.getRenderer(), id, surface);
	}
	if (bSDLBufferd) {
		SDL_Texture* tex=sdl.createTexture(surface);
		SDL_SetTextureBlendMode(tex, defaultBlendMode);
		TextureMap.insert(std::pair<int, SDL_Texture*>(id, tex));
	}
}

static inline void putOutlinePixel(ppl7::grafix::Image& surface, int x, int y, ppl7::grafix::Color& c)
{

}

SDL_Texture* SpriteTexture::generateOutlines(SDL_Renderer* renderer, int id, const ppl7::grafix::Image& src)
{
	ppl7::grafix::Image surface;
	ppl7::grafix::Color white(255, 255, 255, 255);
	surface.create(src.width(), src.height(), src.rgbformat());
	int w=src.width();
	int h=src.height();
	for (int y=0;y < h;y++) {
		for (int x=0;x < w;x++) {
			ppl7::grafix::Color c=src.getPixel(x, y);
			ppl7::grafix::Color cl=src.getPixel(x - 1, y);
			ppl7::grafix::Color cr=src.getPixel(x + 1, y);
			ppl7::grafix::Color cu=src.getPixel(x, y - 1);
			ppl7::grafix::Color cd=src.getPixel(x, y + 1);
			if (c.alpha() > 192 && (cl.alpha() <= 192 || cr.alpha() <= 192 ||
				cu.alpha() <= 192 || cd.alpha() <= 192)) {
				surface.putPixel(x, y, white);
				if (x < w - 1) {
					surface.putPixel(x + 1, y, white);
					if (y > 0) {
						surface.putPixel(x + 1, y - 1, white);
						surface.putPixel(x, y - 1, white);
					}
					if (y < h - 1) {
						surface.putPixel(x + 1, y + 1, white);
						surface.putPixel(x, y + 1, white);
					}
				}

				if (x > 0) {
					surface.putPixel(x - 1, y, white);
					if (y > 0) {
						surface.putPixel(x - 1, y - 1, white);
						surface.putPixel(x, y - 1, white);
					}
					if (y < h - 1) {
						surface.putPixel(x - 1, y + 1, white);
						surface.putPixel(x, y + 1, white);
					}

				}
			}
		}
	}
	SDL_Texture* tex=SDL::createTexture(renderer, surface);
	OutlinesTextureMap.insert(std::pair<int, SDL_Texture*>(id, tex));
	return tex;
}

void SpriteTexture::load(SDL& sdl, const String& filename, const ppl7::grafix::Color& tint)
{
	File ff;
	ff.open(filename);
	load(sdl, ff, tint);
}

void SpriteTexture::load(SDL& sdl, FileObject& ff, const ppl7::grafix::Color& tint)
{
	PFPFile File;
	clear();
	File.load(ff);
	int major, minor;
	File.getVersion(&major, &minor);
	if (File.getID() != "TEXS" || major != 1 || minor != 0) throw grafix::InvalidSpriteException();
	// Texture Chunks laden
	//printf ("opened: %s\n",(const char*)ff.filename());
	PFPChunk* chunk;
	PFPFile::Iterator it;
	File.reset(it);
	while ((chunk=File.findNextChunk(it, "SURF"))) {
		//printf ("load SURF\n");
		loadTexture(sdl, chunk, tint);
	}
	// Index Chunks laden
	File.reset(it);
	//printf ("DONE SURF\n");
	while ((chunk=File.findNextChunk(it, "INDX"))) {
		//printf ("load INDX\n");
		loadIndex(chunk);
	}
}

void SpriteTexture::draw(ppl7::grafix::Drawable& target, int x, int y, int id) const
{
	if (!bMemoryBufferd) return;
	std::map<int, SpriteIndexItem>::const_iterator it;
	it=SpriteList.find(id);
	if (it == SpriteList.end()) return;
	const SpriteIndexItem& item=it->second;
	if (!item.drawable) return;
	ppl7::grafix::Rect r(item.r.x, item.r.y, item.r.w, item.r.h);
	target.bltAlpha(*item.drawable, r, x + item.Offset.x - item.Pivot.x, y + item.Offset.y - item.Pivot.y);
}

void SpriteTexture::draw(ppl7::grafix::Drawable& target, int x, int y, int id, const ppl7::grafix::Color& color_modulation) const
{
	if (!bMemoryBufferd) return;
	std::map<int, SpriteIndexItem>::const_iterator it;
	it=SpriteList.find(id);
	if (it == SpriteList.end()) return;
	const SpriteIndexItem& item=it->second;
	if (!item.drawable) return;
	ppl7::grafix::Rect r(item.r.x, item.r.y, item.r.w, item.r.h);
	target.bltAlphaMod(*item.drawable, r, color_modulation, x + item.Offset.x - item.Pivot.x, y + item.Offset.y - item.Pivot.y);
}


const ppl7::grafix::Drawable SpriteTexture::getDrawable(int id) const
{
	ppl7::grafix::Drawable draw;
	if (!bMemoryBufferd) return draw;
	std::map<int, SpriteIndexItem>::const_iterator it;
	it=SpriteList.find(id);
	if (it == SpriteList.end()) return draw;
	const SpriteIndexItem& item=it->second;
	if (!item.drawable) return draw;
	ppl7::grafix::Rect r(item.r.x, item.r.y, item.r.w, item.r.h);
	draw=(*item.drawable).getDrawable(r);
	return draw;
}

void SpriteTexture::draw(SDL_Renderer* renderer, int x, int y, int id) const
{
	if (!bSDLBufferd) return;
	std::map<int, SpriteIndexItem>::const_iterator it;
	it=SpriteList.find(id);
	if (it == SpriteList.end()) return;
	const SpriteIndexItem& item=it->second;
	SDL_Rect tr;
	tr.x=x + item.Offset.x - item.Pivot.x;
	tr.y=y + item.Offset.y - item.Pivot.y;
	tr.w=item.r.w;
	tr.h=item.r.h;
	SDL_SetTextureColorMod(item.tex, 255, 255, 255);
	SDL_SetTextureAlphaMod(item.tex, 255);
	SDL_RenderCopy(renderer, item.tex, &item.r, &tr);
}

void SpriteTexture::draw(SDL_Renderer* renderer, int x, int y, int id, const ppl7::grafix::Color& color_modulation) const
{
	if (!bSDLBufferd) return;
	std::map<int, SpriteIndexItem>::const_iterator it;
	it=SpriteList.find(id);
	if (it == SpriteList.end()) return;
	const SpriteIndexItem& item=it->second;
	SDL_Rect tr;
	tr.x=x + item.Offset.x - item.Pivot.x;
	tr.y=y + item.Offset.y - item.Pivot.y;
	tr.w=item.r.w;
	tr.h=item.r.h;
	SDL_SetTextureAlphaMod(item.tex, color_modulation.alpha());
	SDL_SetTextureColorMod(item.tex, color_modulation.red(), color_modulation.green(), color_modulation.blue());
	SDL_RenderCopy(renderer, item.tex, &item.r, &tr);
}

void SpriteTexture::drawBoundingBox(SDL_Renderer* renderer, int x, int y, int id) const
{
	if (!bSDLBufferd) return;
	std::map<int, SpriteIndexItem>::const_iterator it;
	it=SpriteList.find(id);
	if (it == SpriteList.end()) return;
	const SpriteIndexItem& item=it->second;
	SDL_Rect tr;
	tr.x=x + item.Offset.x - item.Pivot.x;
	tr.y=y + item.Offset.y - item.Pivot.y;
	tr.w=item.r.w;
	tr.h=item.r.h;
	SDL_RenderDrawRect(renderer, &tr);

}

void SpriteTexture::drawBoundingBoxWithAngle(SDL_Renderer* renderer, int x, int y, int id, float scale_x, float scale_y, float angle) const
{
	if (!bSDLBufferd) return;
	std::map<int, SpriteIndexItem>::const_iterator it;
	it=SpriteList.find(id);
	if (it == SpriteList.end()) return;
	//const SpriteIndexItem& item=it->second;
	ppl7::grafix::Rect rr=spriteBoundary(id, scale_x, scale_y, angle, x, y);

	SDL_Rect tr;
	tr.x=rr.x1;
	tr.y=rr.y1;
	tr.w=rr.width();
	tr.h=rr.height();
	SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
	SDL_RenderDrawRect(renderer, &tr);

}


void SpriteTexture::draw(SDL_Renderer* renderer, int id, const SDL_Rect& source, const SDL_Rect& target) const
{
	if (!bSDLBufferd) return;
	std::map<int, SpriteIndexItem>::const_iterator it;
	it=SpriteList.find(id);
	if (it == SpriteList.end()) return;
	const SpriteIndexItem& item=it->second;
	SDL_SetTextureColorMod(item.tex, 255, 255, 255);
	SDL_SetTextureAlphaMod(item.tex, 255);
	SDL_RenderCopy(renderer, item.tex, &source, &target);
}

void SpriteTexture::drawScaled(SDL_Renderer* renderer, int x, int y, int id, float scale_factor) const
{
	if (!bSDLBufferd) return;
	std::map<int, SpriteIndexItem>::const_iterator it;
	it=SpriteList.find(id);
	if (it == SpriteList.end()) return;
	const SpriteIndexItem& item=it->second;
	SDL_Rect tr;
	//printf ("Sprite::drawScaled %0.1f\n", scale_factor);
	if (scale_factor == 1.0) {
		tr.x=x + item.Offset.x - item.Pivot.x;
		tr.y=y + item.Offset.y - item.Pivot.y;
		tr.w=item.r.w;
		tr.h=item.r.h;
	} else {
		tr.x=x + (item.Offset.x - item.Pivot.x) * scale_factor;
		tr.y=y + (item.Offset.y - item.Pivot.y) * scale_factor;
		tr.w=(int)((float)item.r.w * scale_factor);
		tr.h=(int)((float)item.r.h * scale_factor);
	}
	SDL_SetTextureColorMod(item.tex, 255, 255, 255);
	SDL_SetTextureAlphaMod(item.tex, 255);
	SDL_RenderCopy(renderer, item.tex, &item.r, &tr);
}

void SpriteTexture::drawScaled(SDL_Renderer* renderer, int x, int y, int id, float scale_factor, const ppl7::grafix::Color& color_modulation) const
{
	if (!bSDLBufferd) return;
	std::map<int, SpriteIndexItem>::const_iterator it;
	it=SpriteList.find(id);
	if (it == SpriteList.end()) return;
	const SpriteIndexItem& item=it->second;
	SDL_Rect tr;
	if (scale_factor == 1.0) {
		tr.x=x + item.Offset.x - item.Pivot.x;
		tr.y=y + item.Offset.y - item.Pivot.y;
		tr.w=item.r.w;
		tr.h=item.r.h;
	} else {
		tr.x=x + (item.Offset.x - item.Pivot.x) * scale_factor;
		tr.y=y + (item.Offset.y - item.Pivot.y) * scale_factor;
		tr.w=(int)((float)item.r.w * scale_factor);
		tr.h=(int)((float)item.r.h * scale_factor);
	}
	SDL_SetTextureAlphaMod(item.tex, color_modulation.alpha());
	SDL_SetTextureColorMod(item.tex, color_modulation.red(), color_modulation.green(), color_modulation.blue());
	SDL_RenderCopy(renderer, item.tex, &item.r, &tr);
}

void SpriteTexture::drawScaledWithAngle(SDL_Renderer* renderer, int x, int y, int id, float scale_x, float scale_y, float angle, const ppl7::grafix::Color& color_modulation) const
{
	if (!bSDLBufferd) return;
	std::map<int, SpriteIndexItem>::const_iterator it;
	it=SpriteList.find(id);
	if (it == SpriteList.end()) return;
	const SpriteIndexItem& item=it->second;
	SDL_Rect tr;
	tr.x=x + (item.Offset.x - item.Pivot.x) * scale_x;
	tr.y=y + (item.Offset.y - item.Pivot.y) * scale_y;
	tr.w=(int)((float)item.r.w * scale_x);
	tr.h=(int)((float)item.r.h * scale_y);
	SDL_Point center;
	center.x=(item.Pivot.x - item.Offset.x) * scale_x;
	center.y=(item.Pivot.y - item.Offset.y) * scale_y;

	SDL_SetTextureAlphaMod(item.tex, color_modulation.alpha());
	SDL_SetTextureColorMod(item.tex, color_modulation.red(), color_modulation.green(), color_modulation.blue());
	SDL_RenderCopyEx(renderer, item.tex, &item.r, &tr, angle, &center, SDL_FLIP_NONE);
}


SDL_Rect SpriteTexture::getSpriteSource(int id) const
{
	SDL_Rect r;
	r.x=0;r.y=0;r.w=0;r.h=0;
	std::map<int, SpriteIndexItem>::const_iterator it;
	it=SpriteList.find(id);
	if (it == SpriteList.end()) return r;
	return (*it).second.r;
}

SDL_Texture* SpriteTexture::postGenerateOutlines(SDL_Renderer* renderer, int id)
{
	if (!bMemoryBufferd || !bOutlinesEnabled) return NULL;
	std::map<int, SpriteIndexItem>::iterator it;
	it=SpriteList.find(id);
	if (it == SpriteList.end()) return NULL;
	SpriteIndexItem& item=it->second;
	if (item.outlines) return item.outlines;
	if (!item.drawable) return NULL;

	SDL_Texture* tex=findOutlines(item.textureId);
	if (tex) {
		//ppl7::PrintDebugTime("found already generated outlines for texture %d!\n", item.textureId);
		item.outlines=tex;
		return tex;
	}
	//ppl7::PrintDebugTime("postGenerate outlines for texture %d!\n", item.textureId);
	item.outlines=generateOutlines(renderer, item.textureId, *item.drawable);
	return item.outlines;
}

void SpriteTexture::drawOutlines(SDL_Renderer* renderer, int x, int y, int id, float scale_factor)
{
	if (!bOutlinesEnabled) return;
	std::map<int, SpriteIndexItem>::const_iterator it;
	it=SpriteList.find(id);
	if (it == SpriteList.end()) return;
	const SpriteIndexItem& item=it->second;

	SDL_Texture* tex=item.outlines;
	if (!item.outlines) {
		//ppl7::PrintDebugTime("no outlines!\n");
		tex=postGenerateOutlines(renderer, id);
		if (!tex) return;
	}
	SDL_Rect tr;
	//printf ("Sprite::drawScaled %0.1f\n", scale_factor);
	if (scale_factor == 1.0) {
		tr.x=x + item.Offset.x - item.Pivot.x;
		tr.y=y + item.Offset.y - item.Pivot.y;
		tr.w=item.r.w;
		tr.h=item.r.h;
	} else {
		tr.x=x + (item.Offset.x - item.Pivot.x) * scale_factor;
		tr.y=y + (item.Offset.y - item.Pivot.y) * scale_factor;
		tr.w=(int)((float)item.r.w * scale_factor);
		tr.h=(int)((float)item.r.h * scale_factor);
	}
	SDL_RenderCopy(renderer, tex, &item.r, &tr);
}

void SpriteTexture::drawOutlinesWithAngle(SDL_Renderer* renderer, int x, int y, int id, float scale_x, float scale_y, float angle)
{
	if (!bOutlinesEnabled) return;
	std::map<int, SpriteIndexItem>::const_iterator it;
	it=SpriteList.find(id);
	if (it == SpriteList.end()) return;
	const SpriteIndexItem& item=it->second;

	SDL_Texture* tex=item.outlines;
	if (!item.outlines) {
		//ppl7::PrintDebugTime("no outlines!\n");
		tex=postGenerateOutlines(renderer, id);
		if (!tex) return;
	}
	SDL_Rect tr;
	tr.x=x + (item.Offset.x - item.Pivot.x) * scale_x;
	tr.y=y + (item.Offset.y - item.Pivot.y) * scale_y;
	tr.w=(int)((float)item.r.w * scale_x);
	tr.h=(int)((float)item.r.h * scale_y);
	SDL_Point center;
	center.x=(item.Pivot.x - item.Offset.x) * scale_x;
	center.y=(item.Pivot.y - item.Offset.y) * scale_y;

	SDL_RenderCopyEx(renderer, tex, &item.r, &tr, angle, &center, SDL_FLIP_NONE);
}



ppl7::grafix::Size SpriteTexture::spriteSize(int id, float scale_factor) const
{
	std::map<int, SpriteIndexItem>::const_iterator it;
	it=SpriteList.find(id);
	ppl7::grafix::Size s;
	if (it == SpriteList.end()) return s;
	const SpriteIndexItem& item=it->second;
	s.width=(int)((float)item.r.w * scale_factor);
	s.height=(int)((float)item.r.h * scale_factor);
	return s;
}

ppl7::grafix::Rect SpriteTexture::spriteBoundary(int id, float scale_factor, int x, int y) const
{
	std::map<int, SpriteIndexItem>::const_iterator it;
	it=SpriteList.find(id);
	ppl7::grafix::Rect r;
	if (it == SpriteList.end()) return r;
	const SpriteIndexItem& item=it->second;
	r.x1=x + (item.Offset.x - item.Pivot.x) * scale_factor;
	r.y1=y + (item.Offset.y - item.Pivot.y) * scale_factor;
	r.x2=r.x1 + (int)((float)item.r.w * scale_factor);
	r.y2=r.y1 + (int)((float)item.r.h * scale_factor);
	return r;
}

ppl7::grafix::Point SpriteTexture::spriteOffset(int id) const
{
	std::map<int, SpriteIndexItem>::const_iterator it;
	it=SpriteList.find(id);
	ppl7::grafix::Point p;
	if (it == SpriteList.end()) return p;
	const SpriteIndexItem& item=it->second;
	p.x=(item.Offset.x - item.Pivot.x);
	p.y=(item.Offset.y - item.Pivot.y);
	return p;

}

static inline ppl7::grafix::Point rotate_point(const ppl7::grafix::Point& p, const ppl7::grafix::Point& pivot, float s, float c)
{
	ppl7::grafix::Point pr=p;
	pr.x-=pivot.x;
	pr.y-=pivot.y;
	// rotate point
	float xnew = (float)pr.x * c - (float)pr.y * s;
	float ynew = (float)pr.x * s + (float)pr.y * c;
	pr.x=xnew + pivot.x;
	pr.y=ynew + pivot.y;
	return pr;
}

static inline int min_val(int v1, int v2, int v3, int v4)
{
	int v=v1;
	if (v2 < v) v=v2;
	if (v3 < v) v=v3;
	if (v4 < v) v=v4;
	return v;
}
static inline int max_val(int v1, int v2, int v3, int v4)
{
	int v=v1;
	if (v2 > v) v=v2;
	if (v3 > v) v=v3;
	if (v4 > v) v=v4;
	return v;
}

static ppl7::grafix::Rect rotate(const ppl7::grafix::Rect& r, const ppl7::grafix::Point& pivot, float angle)
{
	ppl7::grafix::Rect r2;
	float s = sin(angle * M_PI / 180.0f);
	float c = cos(angle * M_PI / 180.0f);

	ppl7::grafix::Point p1=rotate_point(r.topLeft(), pivot, s, c);
	ppl7::grafix::Point p2=rotate_point(r.topRight(), pivot, s, c);
	ppl7::grafix::Point p3=rotate_point(r.bottomLeft(), pivot, s, c);
	ppl7::grafix::Point p4=rotate_point(r.bottomRight(), pivot, s, c);
	r2.x1=min_val(p1.x, p2.x, p3.x, p4.x);
	r2.y1=min_val(p1.y, p2.y, p3.y, p4.y);
	r2.x2=max_val(p1.x, p2.x, p3.x, p4.x);
	r2.y2=max_val(p1.y, p2.y, p3.y, p4.y);
	return r2;
}


ppl7::grafix::Rect SpriteTexture::spriteBoundary(int id, float scale_factor_x, float scale_factor_y, float rotation, int x, int y) const
{
	std::map<int, SpriteIndexItem>::const_iterator it;
	it=SpriteList.find(id);
	ppl7::grafix::Rect r;
	if (it == SpriteList.end()) return r;
	const SpriteIndexItem& item=it->second;
	// TODO: add rotation into calculation
	r.x1= x + (item.Offset.x - item.Pivot.x) * scale_factor_x;
	r.y1= y + (item.Offset.y - item.Pivot.y) * scale_factor_y;
	r.x2=r.x1 + (int)((float)item.r.w * scale_factor_x);
	r.y2=r.y1 + (int)((float)item.r.h * scale_factor_y);
	ppl7::grafix::Rect r2=rotate(r, ppl7::grafix::Point(x, y), rotation);
	return r2;
}


int SpriteTexture::numTextures() const
{
	return (int)TextureMap.size();
}

int SpriteTexture::numSprites() const
{
	return (int)SpriteList.size();
}

void SpriteTexture::setTextureBlendMode(SDL_BlendMode blendMode)
{
	std::map<int, SDL_Texture*>::iterator it;
	for (it=TextureMap.begin();it != TextureMap.end();++it) {
		SDL_SetTextureBlendMode(it->second, blendMode);
	}
	defaultBlendMode=blendMode;
}

void SpriteTexture::setPivot(int id, int x, int y)
{
	std::map<int, SpriteIndexItem>::iterator it;
	it=SpriteList.find(id);
	if (it == SpriteList.end()) return;
	it->second.Pivot.setPoint(x, y);
}

const SpriteTexture::SpriteIndexItem* SpriteTexture::getSpriteIndex(int id) const
{
	std::map<int, SpriteIndexItem>::const_iterator it;
	it=SpriteList.find(id);
	if (it == SpriteList.end()) return NULL;
	return &it->second;
}
