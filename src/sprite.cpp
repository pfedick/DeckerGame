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




Sprite::Sprite()
{
}

Sprite::~Sprite()
{
	clear();
}

void Sprite::clear()
{
	std::map<int,SDL_Texture*>::const_iterator it;
	for (it=TextureMap.begin();it!=TextureMap.end();++it) {
		SDL_DestroyTexture(it->second);
	}
	TextureMap.clear();
	SpriteList.clear();
}

SDL_Texture *Sprite::findTexture(int id) const
{
	std::map<int,SDL_Texture*>::const_iterator it;
	it=TextureMap.find(id);
	if (it!=TextureMap.end()) return it->second;
	return NULL;
}

void Sprite::loadIndex(ppl7::PFPChunk *chunk)
{
	char *buffer=(char*)chunk->data();
	int num=Peek32(buffer);		// Anzahl Eintr�ge in der Tabelle
	char *p=buffer+4;
	SpriteIndexItem item;
	for (int i=0;i<num;i++) {
		item.id=Peek32(p+0);
		item.tex=findTexture(Peek16(p+4));
		item.r.x=Peek16(p+6+0);
		item.r.y=Peek16(p+6+2);
		item.r.w=Peek16(p+6+4)+1-item.r.x;
		item.r.h=Peek16(p+6+6)+1-item.r.y;
		item.Pivot.x=Peek16(p+14+0);
		item.Pivot.y=Peek16(p+14+2);
		item.Offset.x=Peek16(p+18+0);
		item.Offset.y=Peek16(p+18+2);
		SpriteList.insert(std::pair<int,SpriteIndexItem>(item.id,item));
		p+=22;
	}
}

void Sprite::loadTexture(SDL &sdl, PFPChunk *chunk)
{
	Compression Comp;
	Comp.usePrefix(Compression::Prefix_V2);
	char *buffer=(char*)chunk->data();

	// Zunächst lesen wir dem Header
	int id=Peek16(buffer+0);
	ppl7::grafix::RGBFormat rgbformat;
	switch (Peek8(buffer+2)) {
		case 9: rgbformat=grafix::RGBFormat::A8R8G8B8;
				break;
		default:
			throw grafix::UnsupportedColorFormatException();
	}
	//int bitdepth=Peek8(buffer+3);
	int width=Peek16(buffer+4);
	int height=Peek16(buffer+6);

	// Nutzdaten dekomprimieren
	ByteArray uncompressed;
	Comp.uncompress(uncompressed,buffer+8,chunk->size()-8);
	buffer=(char*)uncompressed.ptr();

	// Nun erstellen wir ein neues Image
	ppl7::grafix::Image surface;
	surface.create(width,height,rgbformat);
	//printf ("load texture: id=%d, width=%d, height=%d\n", id, width, height);
	for (int y=0;y<height;y++) {
		for (int x=0;x<width;x++) {
			ppl7::grafix::Color c(Peek8(buffer+2),Peek8(buffer+1),Peek8(buffer),Peek8(buffer+3));
			//printf ("%X,",c.alpha());
			//if (rgbformat==grafix::RGBFormat::A8R8G8B8) {
				surface.putPixel(x,y,c);
				buffer+=4;
			//}
		}
		//printf ("\n");
	}
	//ppl7::grafix::ImageFilter_PNG png;
	//png.saveFile("tex.png", surface);
	SDL_Texture *tex=sdl.createTexture(surface);
	TextureMap.insert(std::pair<int, SDL_Texture*>(id,tex));
}

void Sprite::load(SDL &sdl, const String &filename)
{
	File ff;
	ff.open(filename);
	load(sdl, ff);
}

void Sprite::load(SDL &sdl, FileObject &ff)
{
	PFPFile File;
	clear();
	File.load(ff);
	int major, minor;
	File.getVersion(&major,&minor);
	if (File.getID()!="TEXS" || major!=1 || minor!=0) throw grafix::InvalidSpriteException();
	// Texture Chunks laden
	//printf ("opened: %s\n",(const char*)ff.filename());
	PFPChunk *chunk;
	PFPFile::Iterator it;
	File.reset(it);
	while ((chunk=File.findNextChunk(it,"SURF"))) {
		//printf ("load SURF\n");
		loadTexture(sdl, chunk);
	}
	// Index Chunks laden
	File.reset(it);
	//printf ("DONE SURF\n");
	while ((chunk=File.findNextChunk(it,"INDX"))) {
		//printf ("load INDX\n");
		loadIndex(chunk);
	}
}

void Sprite::draw(SDL_Renderer *renderer, int x, int y, int id) const
{
	// Sprite im Index finden
	std::map<int,SpriteIndexItem>::const_iterator it;
	it=SpriteList.find(id);
	if (it==SpriteList.end()) return;
	const SpriteIndexItem &item=it->second;
	SDL_Rect tr;
	tr.x=x+item.Offset.x-item.Pivot.x;
	tr.y=y+item.Offset.y-item.Pivot.y;
	//tr.x=x+item.Offset.x;
	//tr.y=y+item.Offset.y;

	tr.w=item.r.w;
	tr.h=item.r.h;
	SDL_RenderCopy(renderer, item.tex, &item.r, &tr);


	//target.bltAlpha(*item.surface,item.r,x+item.Offset.x-item.Pivot.x, y+item.Offset.y-item.Pivot.y);
}

int Sprite::numTextures() const
{
	return (int)TextureMap.size();
}

int Sprite::numSprites() const
{
	return (int)SpriteList.size();
}
