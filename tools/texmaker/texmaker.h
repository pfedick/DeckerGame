#include <ppl7.h>
#include <ppl7-grafix.h>
#include <list>
#include <map>


enum PIVOT_DETECTION
{
	PIVOT_PARAMS=0,
	PIVOT_BRICKS=1,
	PIVOT_LOWER_MIDDLE=2
};


class IndexItem
{
public:
	int ItemId;
	int TextureId;
	ppl7::grafix::Rect pos;
	ppl7::grafix::Point pivot;
	ppl7::grafix::Point offset;

};

class CacheItem
{
public:
	int id;
	int width;
	int height;
	ppl7::grafix::Image texture;
	ppl7::grafix::Point pivot;
	ppl7::grafix::Point offset;
};

class FreeSpace
{
public:
	FreeSpace();
	FreeSpace(int x, int y, int width, int height);

	ppl7::grafix::Rect r;
	std::list<FreeSpace> mySpaces;

	ppl7::grafix::Rect occupy(const ppl7::grafix::Size& size);

	bool findMatch(const ppl7::grafix::Size& size, ppl7::grafix::Rect& found);

	FreeSpace* findSmallestMatch(const ppl7::grafix::Size& size);
};

class Texture
{
private:
	int width, height;
	ppl7::grafix::Image texture;
	int x, y;
	int maxy;
	int id;
	FreeSpace spaces;
public:
	Texture(int width, int height);
	~Texture();
	int add(const ppl7::grafix::Drawable& surface, const ppl7::grafix::Rect& r, ppl7::grafix::Rect& tgt);
	int add(const ppl7::grafix::Drawable& surface, ppl7::grafix::Rect& tgt);
	int GetId();
	ppl7::PFPChunk* MakeChunk() const;
	void SaveTexture(const char* filename) const;
	size_t findSmallestMatch(const ppl7::grafix::Size& size);

};

class TextureFile
{
private:
	ppl7::PFPFile			File;
	std::list<Texture>	 	TextureList;
	std::list<IndexItem>	Index;
	std::map<uint64_t, CacheItem> cache;
	bool debug;

	int twidth, theight, maxtnum;
	PIVOT_DETECTION pivot_detection;
	void detectPivotBricks(const ppl7::grafix::Drawable& surface, int& px, int& py);
	void detectPivotLowerMiddle(const ppl7::grafix::Drawable& surface, int& px, int& py);
	void addIndexChunk();
	void addTextureChunks();

	// cache
	void addToCache(int id, const ppl7::grafix::Drawable& surface, const ppl7::grafix::Rect& r, const ppl7::grafix::Point& pivot);
	void printCache() const;
	void generateTexturesFromCache();
	void addToTexture(const CacheItem& item);

public:
	TextureFile();
	~TextureFile();

	void SetTextureSize(int width, int height);
	void SetMaxTextureNum(int num);
	void SetAuthor(const char* name);
	void SetName(const char* name);
	void SetCopyright(const char* copyright);
	void SetDescription(const char* description);
	void SetPivotDetection(const PIVOT_DETECTION d);
	int AddFile(const ppl7::String& filename, int id, int pivotx, int pivoty);
	int AddSurface(ppl7::grafix::Drawable& surface, ppl7::grafix::Rect* r, int id, int pivotx, int pivoty);

	void Save(const char* filename);
	void SaveTextures(const char* prefix);

};
