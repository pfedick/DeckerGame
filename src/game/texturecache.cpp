#include <stdio.h>
#include <stdlib.h>
#include <ppl7.h>
#include <ppl7-grafix.h>
#include "decker.h"


TextureCache::TextureCache(SDL& s)
    :sdl(s)
{

}

TextureCache::~TextureCache()
{
    clear();
}

void TextureCache::clear()
{
    std::map<ppl7::String, SDL_Texture*>::iterator it;
    for (it=texture_cache.begin();it != texture_cache.end();++it) {
        SDL_Texture* tex=(*it).second;
        if (tex) {
            sdl.destroyTexture(tex);
        }
    }
    texture_cache.clear();
}

SDL_Texture* TextureCache::get(const ppl7::String& filename)
{
    //ppl7::PrintDebugTime("TextureCache::get(%s)\n", (const char*)filename);
    if (filename.isEmpty()) return NULL;
    std::map<ppl7::String, SDL_Texture*>::const_iterator it;
    it=texture_cache.find(filename);
    if (it != texture_cache.end()) return it->second;
    if (ppl7::File::exists(filename)) {
        SDL_Texture* tex=sdl.createStreamingTexture(filename);
        if (tex) {
            texture_cache.insert(std::pair<ppl7::String, SDL_Texture*>(filename, tex));
            return tex;
        }
    }
    return NULL;
}
