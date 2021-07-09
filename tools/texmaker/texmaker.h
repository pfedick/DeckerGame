#include <ppl7.h>
#include <ppl7-grafix.h>
#include <list>


enum PIVOT_DETECTION
{
	PIVOT_PARAMS=0,
	PIVOT_BRICKS=1,
	PIVOT_LOWER_MIDDLE=2
};


class Texture
{
private:
	int width, height;
	ppl7::grafix::Image texture;
	int x,y;
	int maxy;
	int id;
public:
	Texture(int width, int height);
	~Texture();
	int Add(const ppl7::grafix::Drawable &surface, const ppl7::grafix::Rect &r, ppl7::grafix::Rect &tgt);
	int GetId();
	ppl7::PFPChunk *MakeChunk() const;
	void SaveTexture(const char *filename) const;
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

class TextureFile
{
private:
	ppl7::PFPFile			File;
	std::list<Texture>	 	TextureList;
	std::list<IndexItem>	Index;

	int twidth, theight, maxtnum;
	PIVOT_DETECTION pivot_detection;
	void detectPivotBricks(const ppl7::grafix::Drawable &surface, int &px, int &py);
	void detectPivotLowerMiddle(const ppl7::grafix::Drawable &surface, int &px, int &py);
	void addIndexChunk();
	void addTextureChunks();


public:
	TextureFile();
	~TextureFile();

	void SetTextureSize(int width, int height);
	void SetMaxTextureNum(int num);
	void SetAuthor(const char *name);
	void SetName(const char *name);
	void SetCopyright(const char *copyright);
	void SetDescription(const char *description);
	void SetPivotDetection(const PIVOT_DETECTION d);
	int AddFile(const ppl7::String &filename, int id, int pivotx, int pivoty);
	int AddSurface(ppl7::grafix::Drawable &surface, ppl7::grafix::Rect *r, int id, int pivotx, int pivoty);

	void Save(const char *filename);
	void SaveTextures(const char *prefix);

};
