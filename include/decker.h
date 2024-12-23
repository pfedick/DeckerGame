#ifndef INCLUDE_DECKER_H_
#define INCLUDE_DECKER_H_

#include <SDL.h>
#include <ppl7.h>
#include <ppl7-grafix.h>
#include <ppltk.h>
#include <map>
#include <list>
#include <vector>
#include <array>
#include "decker_sdl.h"
#include "ui.h"
#include "audio.h"
#include "audiopool.h"
#include "waynet.h"
#include "translate.h"
#include "metrics.h"
#include "particle.h"
#include "gamecontroller.h"
#include "light.h"

#define APP_COMPANY "Patrick F.-Productions"
#define APP_NAME "George Decker"

#ifndef MAX_TILESETS
#define MAX_TILESETS 30
#endif


#ifndef MAX_SPRITESETS
#define MAX_SPRITESETS 16
#endif

#define MAX_TILE_LAYER 4




#ifndef DECKER_EXCEPTION
#define DECKER_EXCEPTION
#define STR_VALUE(arg)      #arg
#define EXCEPTION(name,inherit)	class name : public inherit { public: \
	name() throw() {}; \
	name(const char *msg, ...) throw() {  \
		va_list args; va_start(args, msg); copyText(msg,args); \
		va_end(args); } \
		const char* what() const overrides throw() { return (STR_VALUE(name)); } \
	};
#endif

EXCEPTION(InitializationFailed, ppl7::Exception);
EXCEPTION(SDLException, ppl7::Exception);

#define TILE_WIDTH 32
#define TILE_HEIGHT 38

enum class PlaneId {
	Near=6,
	Front=1,
	Player=0,
	Back=3,
	Middle=4,
	Far=2,
	Horizon=5,
	// "MaxPlaneId" must always be the highest ID. It is used for defining
	// dimensions of arrays
	MaxPlaneId=7
};

// planeFactor Player, Front, Far, Back, Middle, Horizon, Near
const double planeFactor[static_cast<int>(PlaneId::MaxPlaneId)]={ 1.0f, 1.0f, 0.5f, 1.0f, 0.8f, 0.3f, 1.3f };


namespace Decker::Objects {
class ObjectSystem;
class Object;

} // EOF namespace Decker::Objects

class SettingsScreen;
class ParticleSystem;
class GameHUD;
class Glimmer;

class Config
{
private:
	ppl7::String		ConfigFile;
public:
	enum class DifficultyLevel
	{
		easy=1,
		normal=2,
		hard=3
	};
	typedef ppltk::Window::WindowMode WindowMode;
		// Video
	int					videoDevice;
	ppl7::grafix::Size	ScreenResolution;
	int 				ScreenRefreshRate;
	ppltk::Window::WindowMode windowMode;

	// Audio
	float				volumeTotal;
	float				volumeMusic;
	float				volumeEffects;
	float				volumeAmbience;
	float				volumeSpeech;

	// Misc
	ppl7::String		CustomLevelPath;
	ppl7::String		LastEditorLevel;
	ppl7::String		TextLanguage;
	ppl7::String		SpeechLanguage;
	bool				tutorialPlayed;
	bool				skipIntro;
	bool				noBlood;
	DifficultyLevel		difficulty;

	// Controller
	class Controller {
	public:
		int					deadzone;
		int					axis_walk;
		int					axis_jump;
		int					button_up;
		int					button_down;
		int					button_left;
		int					button_right;
		int					button_menu;
		int					button_back;
		int					button_action;
		int					button_jump;
		int					button_crouch;
		int					button_flashlight;
		bool				use_rumble;
		Controller();
	};
	Controller controller;




	Config();
	~Config();
	void load();
	void save();
};


class FPS
{
private:
	ppl7::ppl_time_t fps_start_time;
	int fps_frame_count;
	int fps;
public:
	FPS();
	int getFPS() const;
	void update();
};


class ColorPaletteItem
{
public:
	ppl7::grafix::Color color;
	ppl7::String name;
	int ldraw_material;

	ColorPaletteItem();
	ColorPaletteItem(const ppl7::grafix::Color& color, const ppl7::String& name, int ldraw_material);

	void set(const ppl7::grafix::Color& color, const ppl7::String& name, int ldraw_material);
};

class ColorPalette
{
private:
	std::array<ColorPaletteItem, 256> palette;
	ColorPaletteItem undefined;
public:
	ColorPalette();
	void setDefaults();
	void set(uint32_t index, const ppl7::grafix::Color& color, const ppl7::String& name=ppl7::String(), int ldraw_material=0);
	void set(uint32_t index, const ColorPaletteItem& item);
	void setColor(uint32_t index, const ppl7::grafix::Color& color);
	void setName(uint32_t index, const ppl7::String& name);

	const ColorPaletteItem& get(uint32_t index) const;
	const ppl7::grafix::Color& getColor(uint32_t index) const;
	const std::array<ColorPaletteItem, 256>& getPalette() const;
	const ppl7::String& getName(uint32_t index) const;

	void save(ppl7::FileObject& file, unsigned char id) const;
	void load(const ppl7::ByteArrayPtr& ba);
};



class SpriteTexture
{
public:
	class SpriteIndexItem
	{
	public:
		int id;
		int textureId;
		SDL_Texture* tex;
		const ppl7::grafix::Drawable* drawable;
		SDL_Rect r;
		ppl7::grafix::Point Pivot;
		ppl7::grafix::Point Offset;

		SpriteIndexItem()
		{
			id=0;
			textureId=0;
			tex=NULL;
			drawable=NULL;
		}
		SpriteIndexItem(const SpriteIndexItem& other)
			:r(other.r), Pivot(other.Pivot), Offset(other.Offset)
		{
			id=other.id;
			textureId=other.textureId;
			tex=other.tex;
			drawable=other.drawable;
		}
	};
private:
	std::map<int, SDL_Texture*> TextureMap;
	std::map<int, ppl7::grafix::Image> InMemoryTextureMap;
	std::map<int, SpriteIndexItem> SpriteList;

	SDL_Texture* current_outline_texture;
	int current_outline_sprite_id;

	bool bSDLBufferd;
	bool bMemoryBufferd;
	bool bOutlinesEnabled;
	bool bCollisionDetectionEnabled;
	SDL_BlendMode defaultBlendMode;

	void loadTexture(SDL& sdl, ppl7::PFPChunk* chunk, const ppl7::grafix::Color& tint);
	void loadIndex(ppl7::PFPChunk* chunk);
	SDL_Texture* postGenerateOutlines(SDL_Renderer* renderer, int sprite_id);
	SDL_Texture* findTexture(int id) const;
	const ppl7::grafix::Drawable* findInMemoryTexture(int id) const;

public:
	SpriteTexture();
	~SpriteTexture();
	void load(SDL& sdl, const ppl7::String& filename, const ppl7::grafix::Color& tint=ppl7::grafix::Color());
	void load(SDL& sdl, ppl7::FileObject& ff, const ppl7::grafix::Color& tint=ppl7::grafix::Color());
	void clear();
	void draw(ppl7::grafix::Drawable& target, int x, int y, int id) const;
	void draw(ppl7::grafix::Drawable& target, int x, int y, int id, const ppl7::grafix::Color& color_modulation) const;
	void draw(SDL_Renderer* renderer, int x, int y, int id) const;
	void draw(SDL_Renderer* renderer, int x, int y, int id, const ppl7::grafix::Color& color_modulation) const;
	void draw(SDL_Renderer* renderer, int x, int y, int id, const SDL_Color& color_modulation) const;
	void drawBoundingBox(SDL_Renderer* renderer, int x, int y, int id) const;
	void drawBoundingBoxWithAngle(SDL_Renderer* renderer, int x, int y, int id, float scale_x, float scale_y, float angle) const;
	void draw(SDL_Renderer* renderer, int id, const SDL_Rect& source, const SDL_Rect& target) const;
	void drawScaled(SDL_Renderer* renderer, int x, int y, int id, float scale_factor) const;
	void drawScaled(SDL_Renderer* renderer, int x, int y, int id, float scale_factor, const ppl7::grafix::Color& color_modulation) const;
	void drawScaledWithAngle(SDL_Renderer* renderer, int x, int y, int id, float scale_x, float scale_y, float angle, const ppl7::grafix::Color& color_modulation) const;
	void drawOutlines(SDL_Renderer* renderer, int x, int y, int id, float scale_factor);
	void drawOutlinesWithAngle(SDL_Renderer* renderer, int x, int y, int id, float scale_x, float scale_y, float angle);


	ppl7::grafix::Size spriteSize(int id, float scale_factor) const;
	ppl7::grafix::Rect spriteBoundary(int id, float scale_factor, int x, int y) const;
	ppl7::grafix::Rect spriteBoundary(int id, float scale_factor_x, float scale_factor_y, float rotation, int x, int y) const;
	const ppl7::grafix::Drawable getDrawable(int id) const;
	void enableMemoryBuffer(bool enabled);
	void enableSDLBuffer(bool enabled);
	void enableCollisionDetection(bool enabled);
	void enableOutlines(bool enabled);
	int numTextures() const;
	int numSprites() const;
	void setTextureBlendMode(SDL_BlendMode blendMode);
	SDL_BlendMode getTextureBlendMode() const;
	void setPivot(int id, int x, int y);

	SDL_Rect getSpriteSource(int id) const;
	const SpriteIndexItem* getSpriteIndex(int id) const;
	ppl7::grafix::Point spriteOffset(int id) const;
};

class SpriteSystem
{
public:
	class Item
	{
	public:
		Item();
		SpriteSystem* spritesystem;
		SpriteTexture* texture;
		int id;
		int x;			// 4 Byte
		int y;			// 4 Byte
		int z;			// 1 Byte
		uint32_t color_index; // 1 Byte
		int sprite_set;	// 2 Byte
		int sprite_no;	// 2 Byte
		float scale;	// 4 Byte	==> 18 Byte
		float rotation;	// 4 Byte   ==> 22 Byte
		ppl7::grafix::Rect boundary;
	};
private:
	const ColorPalette& palette;
	ppl7::Mutex mutex;
	int maxid;
	std::map<int, SpriteSystem::Item> sprite_list;
	std::map<uint64_t, const SpriteSystem::Item&> visible_sprite_map;
	SpriteTexture* spriteset[MAX_SPRITESETS + 1];
	bool bSpritesVisible;

public:
	SpriteSystem(const ColorPalette& palette);
	~SpriteSystem();
	void clear();
	int addSprite(int x, int y, int z, int spriteset, int sprite_no, float sprite_scale, float sprite_rotation, uint32_t color_index);
	int addSprite(const Item& sprite);
	bool getSprite(int id, SpriteSystem::Item& sprite);
	void deleteSprite(int id);
	void modifySprite(const SpriteSystem::Item& item);
	void setVisible(bool visible);
	bool isVisible() const;
	void setSpriteset(int no, SpriteTexture* spriteset);
	void updateVisibleSpriteList(const ppl7::grafix::Point& worldcoords, const ppl7::grafix::Rect& viewport);
	void draw(SDL_Renderer* renderer, const ppl7::grafix::Rect& viewport, const ppl7::grafix::Point& worldcoords) const;
	void save(ppl7::FileObject& file, unsigned char id) const;
	void load(const ppl7::ByteArrayPtr& ba);
	bool findMatchingSprite(const ppl7::grafix::Point& p, SpriteSystem::Item& sprite) const;

	void drawSelectedSpriteOutline(SDL_Renderer* renderer, const ppl7::grafix::Rect& viewport, const ppl7::grafix::Point& worldcoords, int id);

	size_t count() const;
	size_t countVisible() const;
};

class Tile
{
public:
	enum TileOccupation {
		OccupationNone = 0,
		OccupationPlate0 = 1,
		OccupationPlate1 = 2,
		OccupationPlate2 = 4,
		OccupationBrick = 7
	};
	class Layer
	{
	public:
		int tileset;
		int tileno;
		int origin_x;
		int origin_y;
		int color_index;
		TileOccupation occupation;
		bool showStuds;
	};
	Layer layer[MAX_TILE_LAYER];
	bool block_background;
	Tile();
	void setSprite(int z, int tileset, int tileno, int color_index, bool showStuds);
	bool hasSprite(int z) const;
	bool hasSprite() const;
	void setOccupation(int z, TileOccupation o, int origin_x=-1, int origin_y=-1);
};

class BrickOccupation
{
public:
	class Item
	{
	public:
		Item(int x, int y, Tile::TileOccupation o);
		int x, y;
		Tile::TileOccupation o;
	};
	typedef std::list<BrickOccupation::Item> Matrix;
private:
	std::map<int, BrickOccupation::Matrix> tiles;
	BrickOccupation::Matrix empty;
public:
	void createFromSpriteTexture(const SpriteTexture& tex, int brick_width, int brick_height);
	void createFromImage(int id, const ppl7::grafix::Drawable& img, int brick_width, int brick_height);
	void set(int id, const BrickOccupation::Matrix& matrix);
	const BrickOccupation::Matrix& get(int id);
};

class Plane
{
	//friend class Level;
private:
	Tile** tilematrix;
	int width, height;
	//SpriteSystem spritessystem[2];
	bool bTilesVisible;

	ppl7::grafix::Rect plane_dimension;
	size_t tile_count;

public:
	Plane();
	~Plane();
	void clear();
	void create(int width, int height);
	ppl7::grafix::Size getSize() const;
	void setTile(int x, int y, int z, int tileset, int tileno, int color_index, bool showStuds=true);
	void setBlockBackground(int x, int y, bool block);
	void setOccupation(int x, int y, int z, Tile::TileOccupation o, int origin_x=-1, int origin_y=-1);
	Tile::TileOccupation getOccupation(int x, int y, int z);
	ppl7::grafix::Point getOccupationOrigin(int x, int y, int z);
	void setOccupation(int x, int y, int z, const BrickOccupation::Matrix& matrix);
	void clearOccupation(int x, int y, int z, const BrickOccupation::Matrix& matrix);
	bool isOccupied(int x, int y, int z, const BrickOccupation::Matrix& matrix);
	void clearTile(int x, int y, int z);
	const Tile* get(int x, int y) const;
	void save(ppl7::FileObject& file, unsigned char id) const;
	void load(const ppl7::ByteArrayPtr& ba);
	void setVisible(bool visible);
	bool isVisible() const;
	int getTileNo(int x, int y, int z);
	int getTileSet(int x, int y, int z);
	int getColorIndex(int x, int y, int z);

	size_t tileCount() const;
	ppl7::grafix::Rect getOccupiedArea() const;
};

class TileType
{
public:
	enum Type {
		NonBlocking,
		Blocking,
		SteepRampLeft,
		SteepRampRight,
		ShallowRampLeftLower,
		ShallowRampLeftUpper,
		ShallowRampRightUpper,
		ShallowRampRightLower,
		Ladder,
		Water,
		Plate1h,
		Plate2h,
		Speer,
		Fire,
		AirStream,
		EnemyBlocker,
		BlockFromTop,
		MaxType			// used for arrays
	};
};

class TileTypePlane
{
private:
	SpriteTexture* tiletypes;
	TileType::Type* tilematrix;
	int width, height;

public:
	TileTypePlane();
	~TileTypePlane();
	void clear();
	void create(int width, int height);
	void setType(int x, int y, TileType::Type type);
	TileType::Type getType(int x, int y) const;
	TileType::Type getType(const ppl7::grafix::Point& player) const;
	int getPlayerGround(const ppl7::grafix::Point& player) const;
	void setTileTypesSprites(SpriteTexture* sprites);
	void draw(SDL_Renderer* renderer, const ppl7::grafix::Rect& viewport, const ppl7::grafix::Point& worldcoords) const;
	void save(ppl7::FileObject& file, unsigned char id) const;
	void load(const ppl7::ByteArrayPtr& ba);
	ppl7::grafix::Rect getOccupiedArea() const;
	ppl7::grafix::Size size() const;
};

class Resources
{
private:
	int max_tileset_id;

public:
	SpriteTexture Sprite_George;
	SpriteTexture Sprite_George_Adventure;
	SpriteTexture Cursor;
	SpriteTexture TileTypes;
	SpriteTexture Sprites_Nature;
	SpriteTexture uiSpritesNature;
	SpriteTexture Sprites_Nature_Blury;
	SpriteTexture uiSpritesNatureBlury;
	SpriteTexture Sprites_Plants;
	SpriteTexture uiSpritesPlants;
	SpriteTexture Sprites_Objects;
	SpriteTexture uiSpritesObjects;
	SpriteTexture uiObjects;
	SpriteTexture Sprites_Rocks;
	SpriteTexture uiSpritesRocks;
	SpriteTexture Sprites_Flowers;
	SpriteTexture uiSpritesFlowers;
	SpriteTexture Sprites_Treasure;
	SpriteTexture uiSpritesTreasure;
	SpriteTexture Sprites_White;
	SpriteTexture uiSpritesWhite;
	SpriteTexture Sprites_Tropical;
	SpriteTexture uiSpritesTropical;
	SpriteTexture Lightmaps;
	SpriteTexture uiLightmaps;
	SpriteTexture LightObjects;
	SpriteTexture Sprites_White1D;
	SpriteTexture uiSprites_White1D;
	SpriteTexture Sprites_Decals;
	SpriteTexture uiSpritesDecals;
	SpriteTexture Sprites_Glimmer;


	SpriteTexture Waynet;

	std::list<ppl7::String> background_images;

	class BrickResource
	{
	public:
		ppl7::String name;
		int ldraw_material;
		SpriteTexture world;
		SpriteTexture ui;
		BrickResource();
	};
	Resources();
	BrickResource bricks[MAX_TILESETS];
	void loadBricks(SDL& sdl);
	int getMaxTilesetId() const;
};

Resources& getResources();

class Player;

class Background
{
public:
	enum class Type {
		Image,
		Color
	};
private:
	SDL& sdl;
	SDL_Texture* tex_sky;
	ppl7::grafix::Size tex_size;
	ppl7::grafix::Color color;
	Type t;
	ppl7::String last_image;
	ppl7::grafix::Rect level_dimension;

	Type fade_target_type;
	ppl7::grafix::Color fade_target_color;
	ppl7::String fade_target_image_filename;
	SDL_Texture* fade_target_tex;
	ppl7::grafix::Size fade_tex_size;
	float fade_progress;

	void drawFade(SDL_Renderer* renderer, const ppl7::grafix::Rect& viewport, const ppl7::grafix::Point& WorldCoords);


public:
	Background(SDL& s);
	void clear();
	void setImage(const ppl7::String& filename);
	void setColor(const ppl7::grafix::Color& color);
	void setBackgroundType(Type t);
	void setLevelDimension(const ppl7::grafix::Rect& tiles);
	void setFadeTargetColor(const ppl7::grafix::Color& color);
	void setFadeTargetImage(const ppl7::String& filename);
	void setFadeProgress(float progress);

	void draw(SDL_Renderer* renderer, const ppl7::grafix::Rect& viewport, const ppl7::grafix::Point& WorldCoords);

};


class ModifiableParameter
{
public:
	Background::Type backgroundType;
	ppl7::String BackgroundImage;
	ppl7::String CurrentSong;
	ppl7::grafix::Color BackgroundColor;
	ppl7::grafix::Color GlobalLighting;
	ModifiableParameter();
	void clear();
	bool operator==(const ModifiableParameter& other) const;
};

class LevelDescription
{
public:
	bool partOfStory;
	bool visibleInLevelSelection;
	int  levelSort;
	std::map<ppl7::String, ppl7::String> LevelName;
	std::map<ppl7::String, ppl7::String> Description;
	ppl7::String Author;
	ppl7::ByteArray Thumbnail;

	ppl7::String Filename;	// only for Level selection
	bool isCustomLevel;		// only for Level selection

	LevelDescription();
	void clear();
	bool loadFromFile(const ppl7::String& filename);
	void loadFromAssocArray(const ppl7::AssocArray& a);
};

void getLevelList(std::list<LevelDescription>& level_list);

class LevelParameter : public ModifiableParameter, public LevelDescription
{
private:
public:
	int width;
	int height;
	//ppl7::String Name;
	ppl7::String InitialSong;
	std::vector<ppl7::String> SongPlaylist;
	bool randomSong;

	bool drainBattery;
	float batteryDrainRate; // EnergyPointsPerSecond
	bool flashlightOnOnLevelStart;
	std::set<int>InitialItems;

	LevelParameter();
	void clear();
	size_t size() const;
	void save(ppl7::File& ff, int chunk_id) const;
	void load(const ppl7::ByteArrayPtr& ba);

};

class LevelStats
{
	Player* player;
public:
	std::map<int, size_t>object_counter;
	LevelStats();
	void clear();
	void setPlayer(Player* player);
	size_t getObjectCount(int type) const;
	void print() const;
};

//class LightPlane;

class TextureCache
{
private:
	SDL& sdl;
	std::map<ppl7::String, SDL_Texture*> texture_cache;

public:
	TextureCache(SDL& s);
	~TextureCache();
	void clear();
	SDL_Texture* get(const ppl7::String& filename);
};

class Screenshot
{
public:
	enum class Layer {
		Background=0,
		Horizon,
		Far,
		Middle,
		Back,
		Player,
		Front,
		Near,
		Complete
	};
	enum class Type {
		Color=0,
		Lightmap,
		Final
	};

	enum class Mode {
		File=0,
		Memory
	};
private:
	ppl7::String Filename;
	ppl7::String Path;
	Mode myMode;
	ppl7::grafix::Image img;
	bool done_flag;
public:


	Screenshot(Mode m=Mode::File);
	void setPath(const ppl7::String& path);
	void save(Layer layer, Type type, SDL_Texture* texture);
	void save(LightPlaneId lplane, LightPlayerPlaneMatrix pplane, Type type, SDL_Texture* texture);

	const ppl7::grafix::Drawable& image() const;
	bool isDone() const;
	Mode mode() const;
};

class Level
{
	friend class Game;
public:
	LevelParameter params;
	ModifiableParameter runtimeParams;
	ColorPalette palette;

private:
	Plane FarPlane;
	Plane PlayerPlane;
	Plane FrontPlane;
	Plane BackPlane;
	Plane MiddlePlane;
	Plane HorizonPlane;
	Plane NearPlane;
	TileTypePlane TileTypeMatrix;
	SpriteSystem HorizonSprites[2]={ SpriteSystem(palette),SpriteSystem(palette) };
	SpriteSystem FarSprites[2]={ SpriteSystem(palette),SpriteSystem(palette) };
	SpriteSystem MiddleSprites[2]={ SpriteSystem(palette),SpriteSystem(palette) };
	SpriteSystem BackSprites[2]={ SpriteSystem(palette),SpriteSystem(palette) };
	SpriteSystem PlayerSprites[3]={ SpriteSystem(palette),SpriteSystem(palette),SpriteSystem(palette) };
	SpriteSystem FrontSprites[2]={ SpriteSystem(palette),SpriteSystem(palette) };
	SpriteSystem NearSprites[2]={ SpriteSystem(palette),SpriteSystem(palette) };

	/*
	LightLayer HorizonLights=LightLayer(palette);
	LightLayer FarLights=LightLayer(palette);
	LightLayer MiddleLights=LightLayer(palette);
	//LightLayer BackLights=LightLayer(palette);
	LightLayer PlayerLights=LightLayer(palette);
	LightLayer FrontLights=LightLayer(palette);
	LightLayer NearLights=LightLayer(palette);
	*/
	LightSystem lights;

	Decker::Objects::ObjectSystem* objects;
	ParticleSystem* particles;
	Waynet waynet;

	ppl7::grafix::Rect viewport;
	SpriteTexture* tileset[MAX_TILESETS + 1];
	SpriteTexture* spriteset[MAX_SPRITESETS + 1];
	SDL_Texture* tex_render_target;
	SDL_Texture* tex_render_lightmap;
	SDL_Texture* tex_render_layer;

	bool editMode;
	bool showSprites;
	bool showObjects;
	bool showParticles;
	bool lightsEnabled;

	Screenshot* screenshot;

	void clear();

public:
	enum LevelChunkId {
		chunkPlayerPlane=1,
		chunkFrontPlane=2,
		chunkFarPlane=3,
		chunkBackPlane=4,
		chunkMiddlePlane=5,
		chunkNearPlane=6,
		chunkHorizonPlane=7,
		chunkPlayerSpritesLayer0=10,
		chunkPlayerSpritesLayer1=11,
		chunkFrontSpritesLayer0=12,
		chunkFrontSpritesLayer1=13,
		chunkFarSpritesLayer0=14,
		chunkFarSpritesLayer1=15,
		chunkBackSpritesLayer0=16,
		chunkBackSpritesLayer1=17,
		chunkMiddleSpritesLayer0=18,
		chunkMiddleSpritesLayer1=19,
		chunkTileTypes=20,
		chunkHorizonSpritesLayer0=21,
		chunkHorizonSpritesLayer1=22,
		chunkNearSpritesLayer0=23,
		chunkNearSpritesLayer1=24,
		chunkPlayerSpritesLayer2=25,
		chunkObjects=30,
		chunkWayNet=31,
		chunkLevelParameter=32,
		chunkColorPalette=33,
		chunkLightsHorizon=40,
		chunkLightsFar=41,
		chunkLightsMiddle=42,
		chunkLightsBack=43,
		chunkLightsPlayer=44,
		chunkLightsFront=45,
		chunkLightsNear=46,
		chunkLights=47,
	};

private:
	void drawNonePlayerPlane(SDL_Renderer* renderer, PlaneId planeid, const Plane& plane, const SpriteSystem& sprites1, const SpriteSystem& sprites2, const ppl7::grafix::Point& worldcoords, Metrics& metrics, Particle::Layer particle_back, Particle::Layer particle_front);
	void drawPlane(SDL_Renderer* renderer, const Plane& plane, const ppl7::grafix::Point& worldcoords) const;
	void drawParticles(SDL_Renderer* renderer, Particle::Layer layer, const ppl7::grafix::Point& worldcoords, Metrics& metrics);
	void addLightmap(SDL_Renderer* renderer, LightPlaneId plane, LightPlayerPlaneMatrix pplane, const ppl7::grafix::Point& worldcoords, Metrics& metrics);
	void prepareLayer(SDL_Renderer* renderer);
public:


	Level();
	~Level();
	void setEditmode(bool enabled);
	void setShowSprites(bool enabled);
	void setShowObjects(bool enabled);
	void setShowParticles(bool enabled);
	void setEnableLights(bool enabled);
	void setTileset(int no, SpriteTexture* tileset);
	void setSpriteset(int no, SpriteTexture* spriteset);
	void create(int width, int height);
	void load(const ppl7::String& Filename);
	void save(const ppl7::String& Filename);
	void backup(const ppl7::String& Filename);
	void draw(SDL_Renderer* renderer, const ppl7::grafix::Point& worldcoords, Player* player, Metrics& metrics, Glimmer* glimmer);
	void setViewport(const ppl7::grafix::Rect& r);
	void setRenderTargets(SDL_Texture* tex_render_target, SDL_Texture* tex_render_lightmap, SDL_Texture* tex_render_layer);
	Plane& plane(int id);
	SpriteSystem& spritesystem(int plane, int layer);
	//LightLayer& lightsystem(int plane);
	void updateVisibleSpriteLists(const ppl7::grafix::Point& worldcoords, const ppl7::grafix::Rect& viewport);
	void updateVisibleLightsLists(const ppl7::grafix::Point& worldcoords, const ppl7::grafix::Rect& viewport);
	void updateDynamicLightsLists(const ppl7::grafix::Point& worldcoords, const ppl7::grafix::Rect& viewport);
	bool findSprite(const ppl7::grafix::Point& p, const ppl7::grafix::Point& worldcoords, SpriteSystem::Item& item, int& plane, int& layer) const;
	size_t countSprites() const;
	size_t countVisibleSprites() const;
	size_t countLights() const;
	size_t countVisibleLights() const;

	void getLevelStats(LevelStats& stats) const;

	size_t tileCount() const;
	ppl7::grafix::Rect getOccupiedArea() const;
	ppl7::grafix::Rect getOccupiedAreaFromTileTypePlane() const;
	void TakeScreenshot(Screenshot* screenshot);

};

enum class GameState {
	None=0,
	QuitGame,
	StartGame,
	ShowSettings,
	StartEditor,
	Running,
	GameOver,
	LevelEndTriggerd,
	ShowStats,
	TransferPlayer,
	BlendOut,
	StartTutorial,
	StartNextLevel,
	BackToMenue,
	SelectLevel,
	StartLevel

};

enum class StatsScreenReason {
	PlayerDied,
	LevelEnd
};

class StartScreen;
class SettingsScreen;
class GameStatsScreen;



class Soundtrack
{
private:
	AudioStream* playing_song;
	size_t song_index;
	AudioSystem& audiosystem;
	const LevelParameter& params;
	ppl7::String currentSong;

public:
	Soundtrack(AudioSystem& audio, const LevelParameter& level_params);
	~Soundtrack();
	void update();
	void playInitialSong();
	void playSong(const ppl7::String& filename);
	void fadeout(float seconds);
};

class MessageOverlay
{
public:
	enum class Character {
		George=0
	};
private:
	SDL& sdl;
	SDL_Texture* overlay;
	SpriteTexture spriteset;
	ppl7::grafix::Font font;
	double timeout;
	ppl7::String text, phonetics;
	Character character;
	double nextPhonetic;
	double nextBlink;
	int mouth, eyes;
	std::map<int, int>phonetics_map;
	ppl7::grafix::Size lastSize;

	void updatePhonetics();
	void render();

public:

	MessageOverlay(SDL& sdl);
	~MessageOverlay();
	void loadSprites();
	void resize(const ppl7::grafix::Size& size);
	void draw(SDL_Renderer* renderer, const ppl7::grafix::Rect& viewport);
	void setText(Character c, const ppl7::String& text, const ppl7::String& phonetics, float timeout=0.0f);
	bool hasMessage() const;
	void clear();

};

class GameViewport : public ppl7::grafix::Rect
{
private:
	int menu_offset_x;
	ppl7::grafix::Size real_viewport;
	ppl7::grafix::Size render_size;
	bool scaling_enabled;
	bool allow_upscale;
	SDL_Rect render_rect;
	void update();

public:
	GameViewport();
	void setRealViewport(const ppl7::grafix::Size& size);
	void setRenderSize(const ppl7::grafix::Size& size);
	void setMenuOffset(int x);
	void setScalingEnabled(bool enable);
	void setAllowUpscale(bool allow);
	ppl7::grafix::Point translate(const ppl7::grafix::Point& coords) const;

	void translateMouseEvent(ppltk::MouseEvent* event);
	void getRenderRect(SDL_Rect& rect) const;
	const SDL_Rect& getRenderRect() const;
};

class LevelModificator
{
public:
	ModifiableParameter start;
	ModifiableParameter end;
	float duration;
	double starttime;
	void* triggerobject;

	LevelModificator() {
		duration=0.0f;
		starttime=0.0f;
		triggerobject=NULL;
	}

};

class Game : private ppltk::Window
{
private:
	SDL sdl;
	ppltk::WindowManager* wm;
	ppltk::WidgetStyle Style;
	//ppltk::Window window;

	class RememberMe {
	public:
		int lastTileset;
		int lastTile;
		int lastTileColor;
		int lastTileLayer;

		RememberMe() { clear(); };
		void clear() {
			lastTileset=2;
			lastTile=0;
			lastTileColor=2;
			lastTileLayer=0;
		};

	};
	enum class GameSpeed
	{
		Normal,
		Paused,
		SlowMo,
		ExtremeSlowMo,
		ManualStep
	};
public:
	AudioSystem audiosystem;
	GameController controller;
private:
	AudioPool audiopool;
	Resources resources;
	Level level;
	Background background=Background(sdl);
	SDL_Texture* tex_level_grid;
	SDL_Texture* tex_render_target;
	SDL_Texture* tex_render_lightmap;
	SDL_Texture* tex_render_layer;
	ppl7::grafix::Size desktopSize;
	ppl7::grafix::Font gui_font;
	ppl7::grafix::Rect viewport;
	GameViewport game_viewport;
	GameHUD* hud;
	Decker::ui::MainMenue* mainmenue;
	Decker::ui::StatusBar* statusbar;
	Decker::ui::TilesSelection* tiles_selection;
	Decker::ui::TileTypeSelection* tiletype_selection;
	Decker::ui::SpriteSelection* sprite_selection;
	Decker::ui::ObjectSelection* object_selection;
	Decker::ui::LightSelection* lights_selection;
	Decker::ui::WorldWidget* world_widget;
	Decker::ui::WayNetEdit* waynet_edit;
	//StartScreen* start_screen;
	SettingsScreen* settings_screen;
	GameStatsScreen* game_stats_screen;
	Decker::ui::FileDialog* filedialog;

	BrickOccupation brick_occupation;
	BrickOccupation::Matrix brick_occupation_solid;
	FPS fps;
	Translator translator;

	ppl7::grafix::Point WorldCoords;
	ppl7::grafix::Point WorldMoveStart;
	ppl7::String LevelFile;
	double last_frame_time;
	bool worldIsMoving;
	bool showui;
	bool quitGame;
	bool controlsEnabled;
	bool bGameWindowCreated;

	ppl7::grafix::Size windowedSize;
	RememberMe remember;

	// Soundtrack
	Soundtrack soundtrack=Soundtrack(audiosystem, level.params);

	Metrics metrics;

	ppl7::String nextLevelFile;
	ppl7::String selectedLevelFilename;
	float frame_rate_compensation;

	void createWindow();
	void createRenderTarget();
	void presentStartupScreen();
	void drawRenderTargetToScreen();
	void loadGrafix();
	void initUi();
	void deleteUi();
	void resizeMenueAndStatusbar();
	void initAudio();
	void initGameController();
	void updateUi(const ppltk::MouseState& mouse, const Metrics& metrics);
	void drawWorld(SDL_Renderer* renderer);
	void drawGrid();
	void drawSelectedSprite(SDL_Renderer* renderer, const ppl7::grafix::Point& mouse);
	void drawSelectedTile(SDL_Renderer* renderer, const ppl7::grafix::Point& mouse);
	void drawSelectedObject(SDL_Renderer* renderer, const ppl7::grafix::Point& mouse);
	void drawSelectedLight(SDL_Renderer* renderer, const ppl7::grafix::Point& mouse);
	void moveWorld(int offset_x, int offset_y);
	void moveWorldOnMouseClick(const ppltk::MouseState& mouse);
	void handleMouseDrawInWorld(const ppltk::MouseState& mouse);
	void closeTileTypeSelection();
	void closeTileSelection();
	void closeSpriteSelection();
	void closeObjectSelection();
	void closeLightsSelection();
	void closeWayNet();
	void selectSprite(const ppl7::grafix::Point& mouse);
	void selectLight(const ppl7::grafix::Point& mouse);
	void updateWorldCoords();
	void handleDeath(SDL_Renderer* renderer, float frame_rate_compensation);

	void mouseDownEventOnSprite(ppltk::MouseEvent* event);
	void mouseDownEventOnObject(ppltk::MouseEvent* event);
	void mouseDownEventOnWayNet(ppltk::MouseEvent* event);
	void mouseDownEventOnLight(ppltk::MouseEvent* event);

	void checkFileDialog();
	void checkSoundtrack();
	void updateLevelModificator(double time);

	Player* player;
	Glimmer* glimmer;
	float fade_to_black;
	int death_state;


	enum spriteMode {
		spriteModeDraw,
		SpriteModeEdit,
		SpriteModeSelect
	};
	spriteMode sprite_mode;
	SpriteSystem::Item selected_sprite;
	SpriteSystem* selected_sprite_system;
	ppl7::grafix::Point sprite_move_start;
	Decker::Objects::Object* selected_object;
	LightObject* selected_light;

	GameSpeed game_speed;
	LevelModificator levelModificator;
	Screenshot* screenshot;

public:
	Config config;
	GameState gameState;
	MessageOverlay message_overlay=MessageOverlay(sdl);
	TextureCache texture_cache=TextureCache(sdl);

	Game();
	~Game();
	void init();
	void init_grafix();
	void run();

	// EventHandler
	void quitEvent(ppltk::Event* event);
	void closeEvent(ppltk::Event* event);
	//void mouseClickEvent(ppltk::MouseEvent *event);
	void mouseDownEvent(ppltk::MouseEvent* event);
	void mouseWheelEvent(ppltk::MouseEvent* event);
	void keyDownEvent(ppltk::KeyEvent* event);
	void mouseMoveEvent(ppltk::MouseEvent* event);
	//void resizeEvent(ppltk::ResizeEvent* event);
	void gameControllerAxisMotionEvent(ppltk::GameControllerAxisEvent* event);
	void gameControllerButtonDownEvent(ppltk::GameControllerButtonEvent* event);
	void gameControllerButtonUpEvent(ppltk::GameControllerButtonEvent* event);
	void gameControllerDeviceAdded(ppltk::GameControllerEvent* event);
	void gameControllerDeviceRemoved(ppltk::GameControllerEvent* event);


	void showTilesSelection();
	void showTileTypeSelection();
	void showSpriteSelection();
	void showObjectsSelection();
	void showLightsSelection();
	void showWayNetEdit();
	void setSpriteModeToDraw();
	void showUi(bool enable);
	void enableControls(bool enable);
	bool getControlsEnabled() const;

	void changePlane(int plane);

	ppl7::String selectedLevel() const;

	SDL_Renderer* getSDLRenderer();
	SDL& getSDL();
	ppl7::grafix::Point getViewPos() const;

	ppltk::Window& window();
	Level& getLevel();
	LightSystem& getLightSystem();

	void startLevel(const ppl7::String& filename);
	void unloadLevel();
	bool nextLevel(const ppl7::String& filename);
	void save(const ppl7::String& filename);
	void load();
	void createNewLevel(const LevelParameter& params);
	void updateFromLevelParameters();
	//void clearLevel(int width, int height);
	void playerDied();
	void resetPlayer();
	Player* getPlayer();

	const ppl7::String& getLevelFilename() const;

	void playIntroVideo();
	GameState showStartScreen(AudioStream& GeorgeDeckerTheme);
	void showStatsScreen(StatsScreenReason reason);

	void updateGameControllerMapping();

	void openSaveAsDialog();
	void openLoadDialog();
	void openNewLevelDialog();
	void openSettingsScreen();
	void pauseGame(bool flag);
	void stepFrame();

	void updateDifficultyForSelectedObject(uint8_t dificulty);
	void updateLayerForSelectedObject(int layer);
	void updatePlaneForSelectedObject(int layer);
	void updateSpriteFromUi();

	void updateLightFromUi();
	SDL_Texture* getLightRenderTarget();

	void startLevelModification(double time, void* object);
	void* getLevelModificationObject() const;
	void TakeScreenshot();
	void TakeScreenshot(Screenshot* screenshot);

	Glimmer* getGlimmer();
	Soundtrack& getSoundtrack();

	const ppl7::grafix::Rect& getViewport() const;
	const ppl7::grafix::Point& getWorldCoords() const;

};

ppltk::Window* GetGameWindow();
ColorPalette& GetColorPalette();
Game& GetGame();
uint64_t GetFrameNo();

void FadeToBlack(SDL_Renderer* renderer, int fade_to_black);

#endif /* INCLUDE_DECKER_H_ */
