#ifndef INCLUDE_DECKER_H_
#define INCLUDE_DECKER_H_

#include <SDL.h>
#include <ppl7.h>
#include <ppl7-grafix.h>
#include <ppl7-tk.h>
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

#define APP_COMPANY "Patrick F.-Productions"
#define APP_NAME "George Decker"

#ifndef MAX_TILESETS
#define MAX_TILESETS 30
#endif


#ifndef MAX_SPRITESETS
#define MAX_SPRITESETS 15
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

namespace Decker::Objects {
class ObjectSystem;
class Object;

} // EOF namespace Decker::Objects

class SettingsScreen;
class ParticleSystem;

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
	typedef ppl7::tk::Window::WindowMode WindowMode;
		// Video
	int					videoDevice;
	ppl7::grafix::Size	ScreenResolution;
	int 				ScreenRefreshRate;
	ppl7::tk::Window::WindowMode windowMode;

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
private:
	class SpriteIndexItem
	{
	public:
		int id;
		int textureId;
		SDL_Texture* tex;
		SDL_Texture* outlines;
		const ppl7::grafix::Drawable* drawable;
		SDL_Rect r;
		ppl7::grafix::Point Pivot;
		ppl7::grafix::Point Offset;

		SpriteIndexItem()
		{
			id=0;
			textureId=0;
			tex=outlines=NULL;
			drawable=NULL;
		}
		SpriteIndexItem(const SpriteIndexItem& other)
			:r(other.r), Pivot(other.Pivot), Offset(other.Offset)
		{
			id=other.id;
			textureId=other.textureId;
			tex=other.tex;
			outlines=other.outlines;
			drawable=other.drawable;
		}
	};
	std::map<int, SDL_Texture*> TextureMap;
	std::map<int, SDL_Texture*> OutlinesTextureMap;
	std::map<int, ppl7::grafix::Image> InMemoryTextureMap;
	std::map<int, SpriteIndexItem> SpriteList;

	bool bSDLBufferd;
	bool bMemoryBufferd;
	bool bOutlinesEnabled;
	bool bCollisionDetectionEnabled;

	void loadTexture(SDL& sdl, ppl7::PFPChunk* chunk, const ppl7::grafix::Color& tint);
	void loadIndex(ppl7::PFPChunk* chunk);
	SDL_Texture* postGenerateOutlines(SDL_Renderer* renderer, int id);
	//void generateOutlines(SDL& sdl, int id, const ppl7::grafix::Image& src);
	SDL_Texture* generateOutlines(SDL_Renderer* renderer, int id, const ppl7::grafix::Image& src);
	SDL_Texture* findTexture(int id) const;
	SDL_Texture* findOutlines(int id) const;
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
	void draw(SDL_Renderer* renderer, int id, const SDL_Rect& source, const SDL_Rect& target) const;
	void drawScaled(SDL_Renderer* renderer, int x, int y, int id, float scale_factor) const;
	void drawScaled(SDL_Renderer* renderer, int x, int y, int id, float scale_factor, const ppl7::grafix::Color& color_modulation) const;
	void drawOutlines(SDL_Renderer* renderer, int x, int y, int id, float scale_factor);


	ppl7::grafix::Size spriteSize(int id, float scale_factor) const;
	ppl7::grafix::Rect spriteBoundary(int id, float scale_factor, int x, int y) const;
	const ppl7::grafix::Drawable getDrawable(int id) const;
	void enableMemoryBuffer(bool enabled);
	void enableSDLBuffer(bool enabled);
	void enableCollisionDetection(bool enabled);
	void enableOutlines(bool enabled);
	int numTextures() const;
	int numSprites() const;

	SDL_Rect getSpriteSource(int id) const;
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
		ppl7::grafix::Rect boundary;
	};
private:
	const ColorPalette& palette;
	ppl7::Mutex mutex;
	std::map<int, SpriteSystem::Item> sprite_list;
	std::map<uint64_t, const SpriteSystem::Item&> visible_sprite_map;
	SpriteTexture* spriteset[MAX_SPRITESETS + 1];
	bool bSpritesVisible;

public:
	SpriteSystem(const ColorPalette& palette);
	~SpriteSystem();
	void clear();
	void addSprite(int x, int y, int z, int spriteset, int sprite_no, float sprite_scale, uint32_t color_index);
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

public:
	Background(SDL& s);
	~Background();
	void setImage(const ppl7::String& filename);
	void setColor(const ppl7::grafix::Color& color);
	void setBackgroundType(Type t);
	void setLevelDimension(const ppl7::grafix::Rect& tiles);

	void draw(SDL_Renderer* renderer, const ppl7::grafix::Rect& viewport, const ppl7::grafix::Point& WorldCoords);

};


class LevelParameter
{
private:
public:
	int width;
	int height;
	ppl7::String Name;
	ppl7::String InitialSong;
	std::vector<ppl7::String> SongPlaylist;
	bool randomSong;
	Background::Type backgroundType;
	ppl7::String BackgroundImage;
	ppl7::grafix::Color BackgroundColor;

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


class Level
{
	friend class Game;
public:
	LevelParameter params;
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
	SpriteSystem PlayerSprites[2]={ SpriteSystem(palette),SpriteSystem(palette) };
	SpriteSystem FrontSprites[2]={ SpriteSystem(palette),SpriteSystem(palette) };
	SpriteSystem NearSprites[2]={ SpriteSystem(palette),SpriteSystem(palette) };
	Decker::Objects::ObjectSystem* objects;
	ParticleSystem* particles;
	Waynet waynet;

	ppl7::grafix::Rect viewport;
	SpriteTexture* tileset[MAX_TILESETS + 1];
	SpriteTexture* spriteset[MAX_SPRITESETS + 1];
	bool editMode;
	bool showSprites;
	bool showObjects;
	bool showParticles;

	void clear();

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
		chunkObjects=30,
		chunkWayNet=31,
		chunkLevelParameter=32,
		chunkColorPalette=33
	};

	void drawNonePlayerPlane(SDL_Renderer* renderer, const Plane& plane, const SpriteSystem& sprites1, const SpriteSystem& sprites2, const ppl7::grafix::Point& worldcoords, Metrics& metrics);
	void drawPlane(SDL_Renderer* renderer, const Plane& plane, const ppl7::grafix::Point& worldcoords) const;
	void drawParticles(SDL_Renderer* renderer, Particle::Layer layer, const ppl7::grafix::Point& worldcoords, Metrics& metrics);

public:


	Level();
	~Level();
	void setEditmode(bool enabled);
	void setShowSprites(bool enabled);
	void setShowObjects(bool enabled);
	void setShowParticles(bool enabled);
	void setTileset(int no, SpriteTexture* tileset);
	void setSpriteset(int no, SpriteTexture* spriteset);
	void create(int width, int height);
	void load(const ppl7::String& Filename);
	void save(const ppl7::String& Filename);
	void draw(SDL_Renderer* renderer, const ppl7::grafix::Point& worldcoords, Player* player, Metrics& metrics);
	void setViewport(const ppl7::grafix::Rect& r);
	Plane& plane(int id);
	SpriteSystem& spritesystem(int plane, int layer);
	void updateVisibleSpriteLists(const ppl7::grafix::Point& worldcoords, const ppl7::grafix::Rect& viewport);
	bool findSprite(const ppl7::grafix::Point& p, const ppl7::grafix::Point& worldcoords, SpriteSystem::Item& item, int& plane, int& layer) const;
	size_t countSprites() const;
	size_t countVisibleSprites() const;
	void getLevelStats(LevelStats& stats) const;

	size_t tileCount() const;
	ppl7::grafix::Rect getOccupiedArea() const;
	ppl7::grafix::Rect getOccupiedAreaFromTileTypePlane() const;

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
	BackToMenue

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

class Game : private ppl7::tk::Window
{
private:
	SDL sdl;
	ppl7::tk::WindowManager* wm;
	ppl7::tk::WidgetStyle Style;
	//ppl7::tk::Window window;

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
	ppl7::grafix::Size desktopSize;
	ppl7::grafix::Font gui_font;
	ppl7::grafix::Rect viewport;
	Decker::ui::MainMenue* mainmenue;
	Decker::ui::StatusBar* statusbar;
	Decker::ui::TilesSelection* tiles_selection;
	Decker::ui::TileTypeSelection* tiletype_selection;
	Decker::ui::SpriteSelection* sprite_selection;
	Decker::ui::ObjectSelection* object_selection;
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

	ppl7::grafix::Size windowedSize;
	RememberMe remember;

	// Soundtrack
	Soundtrack soundtrack=Soundtrack(audiosystem, level.params);

	Metrics metrics;

	ppl7::String nextLevelFile;
	float frame_rate_compensation;

	void createWindow();
	void presentStartupScreen();
	void loadGrafix();
	void initUi();
	void deleteUi();
	void resizeMenueAndStatusbar();
	void initAudio();
	void initGameController();
	void updateUi(const ppl7::tk::MouseState& mouse, const Metrics& metrics);
	void drawWorld(SDL_Renderer* renderer);
	void drawGrid();
	void drawSelectedSprite(SDL_Renderer* renderer, const ppl7::grafix::Point& mouse);
	void drawSelectedTile(SDL_Renderer* renderer, const ppl7::grafix::Point& mouse);
	void drawSelectedObject(SDL_Renderer* renderer, const ppl7::grafix::Point& mouse);
	void moveWorld(int offset_x, int offset_y);
	void moveWorldOnMouseClick(const ppl7::tk::MouseState& mouse);
	void handleMouseDrawInWorld(const ppl7::tk::MouseState& mouse);
	void closeTileTypeSelection();
	void closeTileSelection();
	void closeSpriteSelection();
	void closeObjectSelection();
	void closeWayNet();
	void selectSprite(const ppl7::grafix::Point& mouse);
	void updateWorldCoords();
	void handleDeath(SDL_Renderer* renderer, float frame_rate_compensation);

	void mouseDownEventOnSprite(ppl7::tk::MouseEvent* event);
	void mouseDownEventOnObject(ppl7::tk::MouseEvent* event);
	void mouseDownEventOnWayNet(ppl7::tk::MouseEvent* event);

	void checkFileDialog();
	void checkSoundtrack();

	Player* player;
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

	GameSpeed game_speed;

public:
	Config config;
	GameState gameState;
	MessageOverlay message_overlay=MessageOverlay(sdl);

	Game();
	~Game();
	void init();
	void init_grafix();
	void run();

	// EventHandler
	void quitEvent(ppl7::tk::Event* event);
	void closeEvent(ppl7::tk::Event* event);
	//void mouseClickEvent(ppl7::tk::MouseEvent *event);
	void mouseDownEvent(ppl7::tk::MouseEvent* event);
	void mouseWheelEvent(ppl7::tk::MouseEvent* event);
	void keyDownEvent(ppl7::tk::KeyEvent* event);
	void mouseMoveEvent(ppl7::tk::MouseEvent* event);
	void resizeEvent(ppl7::tk::ResizeEvent* event);
	void gameControllerAxisMotionEvent(ppl7::tk::GameControllerAxisEvent* event);
	void gameControllerButtonDownEvent(ppl7::tk::GameControllerButtonEvent* event);
	void gameControllerButtonUpEvent(ppl7::tk::GameControllerButtonEvent* event);
	void gameControllerDeviceAdded(ppl7::tk::GameControllerEvent* event);
	void gameControllerDeviceRemoved(ppl7::tk::GameControllerEvent* event);


	void showTilesSelection();
	void showTileTypeSelection();
	void showSpriteSelection();
	void showObjectsSelection();
	void showWayNetEdit();
	void setSpriteModeToDraw();
	void showUi(bool enable);
	void enableControls(bool enable);
	bool getControlsEnabled() const;

	SDL_Renderer* getSDLRenderer();
	SDL& getSDL();
	ppl7::grafix::Point getViewPos() const;

	ppl7::tk::Window& window();
	Level& getLevel();

	void startLevel(const ppl7::String& filename);
	void unloadLevel();
	bool nextLevel(const ppl7::String& filename);
	void save(const ppl7::String& filename);
	void load();
	void createNewLevel(const LevelParameter& params);
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

};

ppl7::tk::Window* GetGameWindow();
ColorPalette& GetColorPalette();
Game& GetGame();

void FadeToBlack(SDL_Renderer* renderer, int fade_to_black);

#endif /* INCLUDE_DECKER_H_ */
