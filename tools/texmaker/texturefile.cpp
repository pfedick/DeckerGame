#include <stdlib.h>
#include <stdio.h>

#include "texmaker.h"

TextureFile::TextureFile()
{
	twidth=256;
	theight=256;
	maxtnum=0;
	debug=false;
	File.setId("TEXS");
	File.setVersion(1, 0);
	File.setCompression(ppl7::Compression::Algo_NONE);
	File.setAuthor("PPL TextureMaker");
	pivot_detection=PIVOT_PARAMS;
}

TextureFile::~TextureFile()
{
	TextureList.clear();
	Index.clear();
}

void TextureFile::SetTextureSize(int width, int height)
{
	twidth=width;
	theight=height;
}

void TextureFile::SetMaxTextureNum(int num)
{
	maxtnum=num;
}


void TextureFile::SetAuthor(const char* name)
{
	File.setAuthor(name);
}
void TextureFile::SetName(const char* name)
{
	File.setName(name);
}

void TextureFile::SetCopyright(const char* copyright)
{
	File.setCopyright(copyright);
}

void TextureFile::SetDescription(const char* description)
{
	File.setDescription(description);
}

void TextureFile::SetPivotDetection(const PIVOT_DETECTION d)
{
	pivot_detection=d;
}

int TextureFile::AddFile(const ppl7::String& filename, int id, int pivotx, int pivoty)
{
	//ppl7::grafix::Grafix *gfx=ppl7::grafix::GetGrafix();
	ppl7::grafix::Image surface;
	surface.load(filename, ppl7::grafix::RGBFormat::A8R8G8B8);
	// Clipping
	ppl7::grafix::Rect r;
	ppl7::grafix::Color c;
	r.x1=999999;
	r.y1=999999;
	r.x2=0;
	r.y2=0;
	bool empty=true;
	for (int y=0;y < surface.height();y++) {
		for (int x=0;x < surface.width();x++) {
			c=surface.getPixel(x, y);
			// Wenn der AlphaWert > 0 ist, haben wir einen gültigen Pixel
			if (c.alpha() > 0) {
				if (x < r.x1) r.x1=x;
				if (x > r.x2) r.x2=x;
				if (y < r.y1) r.y1=y;
				if (y > r.y2) r.y2=y;
				empty=false;
			}
		}
	}
	if (empty) {
		printf("WARNING: image does not have any content: %s\n", (const char*)filename);
		return 1;
		/*
		r.setRect(0, 0, 0, 0);
		pivotx=0;
		return AddSurface(surface, &r, id, 0, 0);
		*/
	}
	r.x2++;
	r.y2++;
	int width=r.width();
	int height=r.height();
	if (pivot_detection == PIVOT_BRICKS) {
		detectPivotBricks(surface, pivotx, pivoty);
	} else if (pivot_detection == PIVOT_LOWER_MIDDLE) {
		detectPivotLowerMiddle(surface, pivotx, pivoty);
	}
	if (debug) {
		printf("Dimensions: (%i/%i)-(%i/%i) = %i x %i, Pivot: %d/%d\n",
			r.left(), r.top(), r.right(), r.bottom(), width, height,
			pivotx, pivoty);
	}
	return AddSurface(surface, &r, id, pivotx, pivoty);
}

void TextureFile::detectPivotBricks(const ppl7::grafix::Drawable& surface, int& px, int& py)
{
	px=999999;
	py=0;
	for (int y=0;y < surface.height();y++) {
		for (int x=0;x < surface.width();x++) {
			ppl7::grafix::Color c=surface.getPixel(x, y);
			// Wenn der AlphaWert > 0 ist, haben wir einen gültigen Pixel
			if (c.alpha() > 0) {
				if (x <= px) {
					px=x;
					py=y;
				}
			}
		}
	}
}

void TextureFile::detectPivotLowerMiddle(const ppl7::grafix::Drawable& surface, int& px, int& py)
{
	px=surface.width() / 2;
	py=999999;
	for (int y=surface.height() - 1;y >= 0;y--) {
		for (int x=0;x < surface.width();x++) {
			ppl7::grafix::Color c=surface.getPixel(x, y);
			// Wenn der AlphaWert > 0 ist, haben wir einen gültigen Pixel
			if (c.alpha() > 0) {
				py=y;
				return;
			}
		}
	}
}




int TextureFile::AddSurface(ppl7::grafix::Drawable& surface, ppl7::grafix::Rect* r, int id, int pivotx, int pivoty)
{
	ppl7::grafix::Rect rr;
	if (!r) {
		rr.x1=0;
		rr.y1=0;
		rr.x2=surface.width();
		rr.y2=surface.height();
	} else {
		rr.x1=r->x1;
		rr.y1=r->y1;
		rr.x2=r->x2;
		rr.y2=r->y2;
	}
	int width=rr.width();
	int height=rr.height();
	// Falls das Sprite größer ist, als die Texturgröße, lehnen wir es ab
	if (width > twidth || height > theight) {
		printf("Sprite ist zu gross fuer Textur\n");
		return 0;
	}
	fflush(stdout);

	addToCache(id, surface, rr, ppl7::grafix::Point(pivotx, pivoty));
	return 1;

	// Haben wir dafür Platz in einer vorhandenen Textur?
	ppl7::grafix::Rect tgt;
	bool found=false;
	std::list<Texture>::iterator it;
	it=TextureList.begin();
	Texture* tx=NULL;
	while (it != TextureList.end()) {
		tx=&(*it);
		if (tx->add(surface, rr, tgt)) {
			found=true;
			break;
		}
		++it;
	}
	if (!found) {
		//printf("Beginne neue Textur\n");
		tx=new Texture(twidth, theight);
		if (!tx->add(surface, rr, tgt)) {
			delete tx;
			printf("Textur konnte nicht hinzugefuegt werden\n");
			return 0;
		}
		TextureList.push_back(*tx);
	}
	// Eintrag im Index machen
	IndexItem item;
	item.ItemId=id;
	item.TextureId=tx->GetId();
	item.pos=tgt;
	item.pivot.x=pivotx;
	item.pivot.y=pivoty;
	item.offset.x=rr.x1;
	item.offset.y=rr.y1;
	Index.push_back(item);
	return 1;
}

void TextureFile::addIndexChunk()
{
	// Daten zusammensetzen. Format:
	// Byte  0: Anzahl Sprites (4 Byte)
	// Byte  4: Beginn der Einträge, Format:
	//          Byte  0: ItemId         (4 Byte)
	//          Byte  4: TextureId      (2 Byte)
	//          Byte  6: TextureRECT    (8 Byte, je 2 Byte für left, top, right, bottom)
	//          Byte 14: Pivot-Punkt    (4 byte, je 2 Byte für x und y)
	//          Byte 18: Offset         (4 byte, je 2 Byte für x und y)
	//          ==> 22 Byte pro Eintrag

	size_t bytes=4 + 22 * Index.size();
	char* buffer=(char*)malloc(bytes);
	ppl7::Poke32(buffer + 0, Index.size());
	std::list<IndexItem>::const_iterator it;
	char* p=buffer + 4;
	for (it=Index.begin();it != Index.end();++it) {
		if ((size_t)(p - buffer + 22) > bytes) {
			free(buffer);
			throw ppl7::Exception("Hier stimmt was nicht!");
		}
		const IndexItem& item=(*it);
		ppl7::Poke32(p + 0, item.ItemId);
		ppl7::Poke16(p + 4, item.TextureId);
		ppl7::Poke16(p + 6 + 0, item.pos.left());
		ppl7::Poke16(p + 6 + 2, item.pos.top());
		ppl7::Poke16(p + 6 + 4, item.pos.right());
		ppl7::Poke16(p + 6 + 6, item.pos.bottom());
		ppl7::Poke16(p + 14 + 0, item.pivot.x);
		ppl7::Poke16(p + 14 + 2, item.pivot.y);
		ppl7::Poke16(p + 18 + 0, item.offset.x);
		ppl7::Poke16(p + 18 + 2, item.offset.y);
		p=p + 22;
	}
	// Chunk speichern
	// Zunächst erstellen wir den Index-Chunk
	ppl7::PFPChunk* chunk=new ppl7::PFPChunk;
	chunk->setName("INDX");
	chunk->setData(buffer, bytes);
	File.addChunk(chunk);
}

void TextureFile::addTextureChunks()
{
	// Nun die Texturen ergänzen
	std::list<Texture>::const_iterator it;
	for (it=TextureList.begin();it != TextureList.end();++it) {
		ppl7::PFPChunk* chunk=(*it).MakeChunk();
		if (!chunk) {
			throw ppl7::Exception("Texture-Chunk konnte nicht erstellt werden");
		}
		File.addChunk(chunk);
	}
}


void TextureFile::Save(const char* filename)
{
	//printCache();
	generateTexturesFromCache();
	//printf("Erstelle Datei mit %zd Sprites und %zd Textures...\n", Index.size(), TextureList.size());
	//return;
	addIndexChunk();
	addTextureChunks();
	File.save(filename);
}


void TextureFile::SaveTextures(const char* prefix)
{
	ppl7::String Filename;
	int i=0;
	std::list<Texture>::const_iterator it;
	for (it=TextureList.begin();it != TextureList.end();++it) {
		Filename.setf("%s-%02i.bmp", prefix, i);
		printf("    save texture to file: %s\n", (const char*)Filename);
		(*it).SaveTexture(Filename);
		i++;
	}
}
