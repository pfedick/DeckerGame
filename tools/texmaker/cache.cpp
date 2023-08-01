#include <string.h>
#include <stdlib.h>
#include "texmaker.h"


void TextureFile::addToCache(int id, const ppl7::grafix::Drawable& surface, const ppl7::grafix::Rect& r, const ppl7::grafix::Point& pivot)
{
	CacheItem item;
	item.texture=surface.getDrawable(r);
	item.id=id;
	item.width=item.texture.width();
	item.height=item.texture.height();
	item.pivot=pivot;
	item.offset.x=r.x1;
	item.offset.y=r.y1;

	uint64_t key=((uint64_t)item.height << 32) | ((uint64_t)item.width & 0xffff) << 16 | ((uint64_t)id & 0xffff);
	cache.insert(std::pair<uint64_t, CacheItem>(key, item));

}

void TextureFile::printCache() const
{
	printf("we have %zd sprites\n", cache.size());
	std::map<uint64_t, CacheItem>::const_reverse_iterator it;
	for (it=cache.rbegin();it != cache.rend();++it) {
		printf("    id: %4d, height: %4d, width: %4d\n", (*it).second.id, (*it).second.height, (*it).second.width);
	}
	fflush(stdout);
}


void TextureFile::generateTexturesFromCache()
{
	//printf("Generating textures and index for %zd sprites\n", cache.size());
	std::map<uint64_t, CacheItem>::const_reverse_iterator it;
	size_t c=0;
	for (it=cache.rbegin();it != cache.rend();++it) {
		addToTexture(it->second);
		c++;
	}
	printf("%zd Sprites stored in %zd textures and %zd indexes\n", c, TextureList.size(), Index.size());


	fflush(stdout);

}


void TextureFile::addToTexture(const CacheItem& item)
{
	// Haben wir dafür Platz in einer vorhandenen Textur?
	ppl7::grafix::Rect tgt;
	//bool found=false;
	std::list<Texture>::iterator it;
	it=TextureList.begin();
	Texture* bestmatch=NULL;
	Texture* tx=NULL;
	ppl7::grafix::Size size(item.width, item.height);
	size_t smallest_size=0;

	while (it != TextureList.end()) {
		tx=&(*it);
		size_t bytes=tx->findSmallestMatch(size);
		if ((smallest_size == 0) & (bytes > 0)) {
			smallest_size=bytes;
			bestmatch=tx;
		} else if (bytes > 0 && bytes < smallest_size) {
			smallest_size=bytes;
			bestmatch=tx;
		}

		/*
		if (tx->add(item.texture, tgt)) {
			found=true;
			break;
		}
		*/
		++it;
	}
	if (bestmatch) {
		if (!bestmatch->add(item.texture, tgt)) {
			printf("ups\n");
			return;
		}
		tx=bestmatch;
	} else {
		//printf("Beginne neue Textur\n");
		tx=new Texture(twidth, theight);
		if (!tx->add(item.texture, tgt)) {
			delete tx;
			//printf("Textur konnte nicht hinzugefuegt werden\n");
			return;
		}
		TextureList.push_back(*tx);
	}
	// Eintrag im Index machen
	IndexItem idx;
	idx.ItemId=item.id;
	idx.TextureId=tx->GetId();
	idx.pos=tgt;
	idx.pivot.x=item.pivot.x;
	idx.pivot.y=item.pivot.y;
	idx.offset.x=item.offset.x;
	idx.offset.y=item.offset.y;
	//printf("added sprite %4d to texture %2d in coordinates: %4d:%4d - %4d:%4d (%4d:%4d)\n", item.id, idx.TextureId, idx.pos.x1, idx.pos.y1, idx.pos.x2, idx.pos.y2, idx.pos.width(), idx.pos.height());
	Index.push_back(idx);
}
