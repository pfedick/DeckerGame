#include "decker.h"
#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <ppl7-grafix.h>
#include "player.h"
#include "objects.h"
#include "screens.h"

static double planeFactor[]={ 1.0f, 1.0f, 0.5f, 1.0f, 0.8f, 0.3f, 1.3f };

static ppl7::tk::Window* GameWindow=NULL;
static Game* GameInstance=NULL;


ppl7::tk::Window* GetGameWindow()
{
	return GameWindow;
}

ppl7::grafix::Point GetViewPos()
{
	return GameInstance->getViewPos();
}

Game::Game()
{
	GameWindow=this;
	GameInstance=this;
	tex_level_grid=NULL;
	wm=ppl7::tk::GetWindowManager();
	ppl7::tk::WidgetStyle s(ppl7::tk::WidgetStyle::Dark);
	Style=s;
	player=NULL;
	mainmenue=NULL;
	statusbar=NULL;
	tiles_selection=NULL;
	quitGame=false;
	worldIsMoving=false;
	tiletype_selection=NULL;
	sprite_selection=NULL;
	object_selection=NULL;
	waynet_edit=NULL;
	world_widget=NULL;
	sprite_mode=spriteModeDraw;
	selected_sprite_system=NULL;
	selected_sprite.id=-1;
	selected_object=NULL;
	fade_to_black=0;
	death_state=0;
	showui=false;
	controlsEnabled=true;
	settings_screen=NULL;
	filedialog=NULL;
}

Game::~Game()
{
	if (settings_screen) delete  settings_screen;
	if (player) delete player;
	if (tex_level_grid) sdl.destroyTexture(tex_level_grid);
	wm->destroyWindow(*this);
}

void Game::loadGrafix()
{
	resources.Sprite_George.enableMemoryBuffer(true);
	resources.Sprite_George.load(sdl, "res/george.tex");

	resources.Sprite_George_Adventure.enableMemoryBuffer(true);
	resources.Sprite_George_Adventure.load(sdl, "res/george_adventure.tex");

	resources.Cursor.load(sdl, "res/cursor.tex");
	resources.TileTypes.enableMemoryBuffer(true);
	resources.TileTypes.load(sdl, "res/tiletypes.tex");
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	resources.Sprites_Nature.enableOutlines(true);
	resources.Sprites_Nature.enableMemoryBuffer(true);
	resources.Sprites_Nature.load(sdl, "res/sprites_nature.tex");

	resources.Sprites_Plants.enableOutlines(true);
	resources.Sprites_Plants.enableMemoryBuffer(true);
	resources.Sprites_Plants.load(sdl, "res/sprites_plants.tex");

	resources.Sprites_Objects.enableOutlines(true);
	resources.Sprites_Objects.enableMemoryBuffer(true);
	resources.Sprites_Objects.load(sdl, "res/sprites_objects.tex");

	resources.Sprites_Nature_Blury.enableOutlines(true);
	resources.Sprites_Nature_Blury.enableMemoryBuffer(true);
	resources.Sprites_Nature_Blury.load(sdl, "res/sprites_nature_blury.tex");

	resources.Sprites_Rocks.enableOutlines(true);
	resources.Sprites_Rocks.enableMemoryBuffer(true);
	resources.Sprites_Rocks.load(sdl, "res/sprites_rocks.tex");

	resources.Sprites_Flowers.enableOutlines(true);
	resources.Sprites_Flowers.enableMemoryBuffer(true);
	resources.Sprites_Flowers.load(sdl, "res/sprites_flowers.tex");

	resources.Sprites_Treasure.enableOutlines(true);
	resources.Sprites_Treasure.enableMemoryBuffer(true);
	resources.Sprites_Treasure.load(sdl, "res/sprites_treasure.tex");

	resources.Sprites_White.enableOutlines(true);
	resources.Sprites_White.enableMemoryBuffer(true);
	resources.Sprites_White.load(sdl, "res/sprites_white_x8.tex");


	resources.loadBricks(sdl);
	brick_occupation.createFromSpriteTexture(resources.bricks[2].world, TILE_WIDTH, TILE_HEIGHT);
	brick_occupation_solid.push_back(BrickOccupation::Item(0, 0, Tile::TileOccupation::OccupationBrick));
	brick_occupation_solid.push_back(BrickOccupation::Item(0, 1, Tile::TileOccupation::OccupationBrick));
	brick_occupation_solid.push_back(BrickOccupation::Item(1, 0, Tile::TileOccupation::OccupationBrick));
	brick_occupation_solid.push_back(BrickOccupation::Item(1, 1, Tile::TileOccupation::OccupationBrick));

	resources.uiSpritesNature.enableSDLBuffer(false);
	resources.uiSpritesNature.enableMemoryBuffer(true);
	resources.uiSpritesNature.load(sdl, "res/sprites_nature_ui.tex");

	resources.uiSpritesPlants.enableSDLBuffer(false);
	resources.uiSpritesPlants.enableMemoryBuffer(true);
	resources.uiSpritesPlants.load(sdl, "res/sprites_plants_ui.tex");

	resources.uiSpritesObjects.enableSDLBuffer(false);
	resources.uiSpritesObjects.enableMemoryBuffer(true);
	resources.uiSpritesObjects.load(sdl, "res/sprites_objects_ui.tex");

	resources.uiSpritesNatureBlury.enableSDLBuffer(false);
	resources.uiSpritesNatureBlury.enableMemoryBuffer(true);
	resources.uiSpritesNatureBlury.load(sdl, "res/sprites_nature_blury_ui.tex");

	resources.uiSpritesRocks.enableSDLBuffer(false);
	resources.uiSpritesRocks.enableMemoryBuffer(true);
	resources.uiSpritesRocks.load(sdl, "res/sprites_rocks_ui.tex");

	resources.uiSpritesFlowers.enableSDLBuffer(false);
	resources.uiSpritesFlowers.enableMemoryBuffer(true);
	resources.uiSpritesFlowers.load(sdl, "res/sprites_flowers_ui.tex");

	resources.uiSpritesTreasure.enableSDLBuffer(false);
	resources.uiSpritesTreasure.enableMemoryBuffer(true);
	resources.uiSpritesTreasure.load(sdl, "res/sprites_treasure_ui.tex");

	resources.uiObjects.enableSDLBuffer(false);
	resources.uiObjects.enableMemoryBuffer(true);
	resources.uiObjects.load(sdl, "res/objects_ui.tex");

	resources.uiSpritesWhite.enableSDLBuffer(false);
	resources.uiSpritesWhite.enableMemoryBuffer(true);
	resources.uiSpritesWhite.load(sdl, "res/sprites_white_ui.tex");

	level.objects->loadSpritesets(sdl);

}

void Game::createWindow()
{
	if (config.windowMode == Config::WindowMode::Window) {
		setFlags(ppl7::tk::Window::WaitVsync | ppl7::tk::Window::Resizeable);
	} else if (config.windowMode == Config::WindowMode::Fullscreen) {
		setFlags(ppl7::tk::Window::WaitVsync | ppl7::tk::Window::Fullscreen | ppl7::tk::Window::Resizeable);
	} else {
		setFlags(ppl7::tk::Window::WaitVsync | ppl7::tk::Window::FullscreenDesktop | ppl7::tk::Window::Resizeable);
	}
	//setFlags(ppl7::tk::Window::DefaultFullscreen);
	setWindowTitle("George Decker");
	ppl7::grafix::Image icon;
	icon.load("res/icon_128.png");
	setWindowIcon(icon);
	setRGBFormat(ppl7::grafix::RGBFormat::A8R8G8B8);
	setBackgroundColor(ppl7::grafix::Color(0, 0, 0, 0));
	setSize(config.ScreenResolution);
	wm->createWindow(*this);
	//setPos(0,0);
	SDL_Renderer* renderer=(SDL_Renderer*)getRenderer();
	sdl.setRenderer(renderer);
	//SDL_RenderSetLogicalSize(renderer, 1920, 1080);

	SDL_ShowCursor(SDL_DISABLE);
}

ppl7::tk::Window& Game::window()
{
	return *this;
}

Level& Game::getLevel()
{
	return level;
}

SDL_Renderer* Game::getSDLRenderer()
{
	return sdl.getRenderer();
}

SDL& Game::getSDL()
{
	return sdl;
}

void Game::showUi(bool enable)
{
	const ppl7::grafix::Size& desktop=clientSize();
	showui=enable;
	if (showui) {
		viewport.y1=33;
		viewport.y2=desktop.height - 33;
		world_widget->setViewport(viewport);
		mainmenue->setVisible(true);
		statusbar->setVisible(true);
	} else {
		closeTileTypeSelection();
		closeTileSelection();
		closeSpriteSelection();
		closeObjectSelection();
		closeWayNet();
		mainmenue->setShowTileTypes(false);
		mainmenue->setWorldFollowsPlayer(true);
		mainmenue->setVisible(false);
		statusbar->setVisible(false);
		viewport.y1=0;
		viewport.x1=0;
		viewport.y2=desktop.height;
		world_widget->setViewport(viewport);
	}
}

void Game::initUi()
{
	ppl7::grafix::Grafix* gfx=ppl7::grafix::GetGrafix();
	gfx->loadFont("res/notosans.fnt6", "NotoSans");
	gfx->loadFont("res/notosans-black.fnt6", "NotoSansBlack");
	Style.labelFont.setName("NotoSans");
	Style.buttonFont.setName("NotoSans");
	Style.buttonFont.setBold(true);
	Style.inputFont.setName("NotoSans");
	wm->setWidgetStyle(Style);

	const ppl7::grafix::Size& desktop=clientSize();
	//ppl7::tk::Label *label;

	resizeMenueAndStatusbar();
	viewport.y1=33;
	viewport.y2=desktop.height - 33;

	world_widget=new Decker::ui::WorldWidget();
	world_widget->create(0, 32, desktop.width, desktop.height - 64);
	world_widget->setEventHandler(this);
	world_widget->setViewport(viewport);
	this->addChild(world_widget);
	wm->setKeyboardFocus(world_widget);
}

void Game::resizeMenueAndStatusbar()
{
	const ppl7::grafix::Size& desktop=clientSize();
	if (!statusbar) {
		statusbar=new Decker::ui::StatusBar(0, desktop.height - 32, desktop.width, 32);
		this->addChild(statusbar);
	} else {
		statusbar->resize(0, desktop.height - 32, desktop.width, 32);
	}

	if (!mainmenue) {
		mainmenue=new Decker::ui::MainMenue(0, 0, desktop.width, 32, this);
		this->addChild(mainmenue);
	} else {
		mainmenue->resize(0, 0, desktop.width, 32);
	}
}

void Game::deleteUi()
{
	if (world_widget) {
		this->removeChild(world_widget);
		delete world_widget;
		world_widget=NULL;
	}
	if (statusbar) {
		this->removeChild(statusbar);
		delete statusbar;
		statusbar=NULL;
	}
	if (mainmenue) {
		this->removeChild(mainmenue);
		delete mainmenue;
		mainmenue=NULL;
	}
}

void Game::presentStartupScreen()
{
	SDL_Texture* tex=sdl.createStreamingTexture("res/loading.png");
	ppl7::grafix::Size imgsize=sdl.getTextureSize(tex);
	wm->handleEvents();
	//ppl7::grafix::Color white(255, 255, 255, 255);
	//sdl.startFrame(white);
	sdl.startFrame(Style.windowBackgroundColor);

	viewport=clientRect();
	SDL_Rect target;
	target.x=(viewport.width() - imgsize.width) / 2;
	target.y=(viewport.height() - imgsize.height) / 2;
	target.w=imgsize.width;
	target.h=imgsize.height;
	//SDL_SetRenderDrawColor(sdl.getRenderer(), 0, 0, 0, 0);
	//SDL_RenderFillRect(sdl.getRenderer(), &target);
	SDL_RenderCopy(sdl.getRenderer(), tex, NULL, &target);
	sdl.present();

	sdl.destroyTexture(tex);
}

void Game::init()
{
	createWindow();
	initUi();
	initAudio();
	desktopSize=clientSize();
	viewport=clientRect();


	gui_font.setName("Default");
	gui_font.setSize(12);
	gui_font.setBold(true);
	gui_font.setColor(ppl7::grafix::Color(255, 255, 255, 255));
	gui_font.setBorderColor(ppl7::grafix::Color(0, 0, 0, 0));
	gui_font.setShadowColor(ppl7::grafix::Color(0, 0, 0, 0));
	gui_font.setOrientation(ppl7::grafix::Font::TOP);
	gui_font.setAntialias(true);
}

void Game::init_grafix()
{
	presentStartupScreen();
	loadGrafix();

	if (player) delete player;
	player=new Player(this);
	player->setSavePoint(ppl7::grafix::Point(3300, 1800));
	updateWorldCoords();

	player->setSpriteResource(resources.Sprite_George_Adventure);
	player->setTileTypeResource(resources.TileTypes);

	//level.create(255,255);
	for (int i=1;i <= resources.getMaxTilesetId();i++) {
		//printf("setTileset %d\n",i);
		level.setTileset(i, &resources.bricks[i].world);
	}
	level.setSpriteset(1, &resources.Sprites_Nature);
	level.setSpriteset(2, &resources.Sprites_Plants);
	level.setSpriteset(3, &resources.Sprites_Objects);
	level.setSpriteset(4, &resources.Sprites_Nature_Blury);
	level.setSpriteset(5, &resources.Sprites_Rocks);
	level.setSpriteset(6, &resources.Sprites_Flowers);
	level.setSpriteset(7, &resources.Sprites_Treasure);
	level.setSpriteset(8, &resources.Sprites_White);

	level.TileTypeMatrix.setTileTypesSprites(&resources.TileTypes);

}

void Game::initAudio()
{
	audiosystem.init();
	audiosystem.setGlobalVolume(config.volumeTotal);
	audiosystem.setVolume(AudioClass::Effect, config.volumeEffects);
	audiosystem.setVolume(AudioClass::Music, config.volumeMusic);
	audiopool.load();
	audiopool.setAudioSystem(&audiosystem);
}

void Game::drawGrid()
{
	if (!tex_level_grid) {
		tex_level_grid=sdl.createStreamingTexture(desktopSize.width, desktopSize.height);
		ppl7::grafix::Drawable draw=sdl.lockTexture(tex_level_grid);
		ppl7::grafix::Color white(255, 255, 255, 128);
		ppl7::grafix::Color black(0, 0, 0, 128);

		draw.cls(0);
		for (int x=0;x < desktopSize.width;x+=TILE_WIDTH) {
			draw.line(x, 0, x, desktopSize.height, black);
			draw.line(x + 1, 0, x + 1, desktopSize.height, white);
		}
		for (int y=0;y < desktopSize.height;y+=TILE_HEIGHT) {
			draw.line(0, y, desktopSize.width, y, black);
			draw.line(0, y + 1, desktopSize.width, y + 1, white);
		}
		sdl.unlockTexture(tex_level_grid);
	}
	int currentPlane=mainmenue->currentPlane();
	ppl7::grafix::Point c=WorldCoords * planeFactor[currentPlane];

	SDL_Renderer* renderer=sdl.getRenderer();
	SDL_Rect target;
	target.x=viewport.x1 - (c.x % TILE_WIDTH);
	target.y=viewport.y1 - (c.y % TILE_HEIGHT);
	target.w=desktopSize.width;
	target.h=desktopSize.height;
	SDL_RenderCopy(renderer, tex_level_grid, NULL, &target);
}

void Game::moveWorld(int offset_x, int offset_y)
{
	if (offset_x == 0 && offset_y == 0) return;
	WorldCoords.x+=offset_x;
	WorldCoords.y+=offset_y;
	if (WorldCoords.x < 0) WorldCoords.x=0;
	if (WorldCoords.x > 62000) WorldCoords.x=62000;
	if (WorldCoords.y < 0) WorldCoords.y=0;
	if (WorldCoords.y > 62000) WorldCoords.y=62000;
}

void Game::moveWorldOnMouseClick(const ppl7::tk::MouseState& mouse)
{
	const Uint8* state = SDL_GetKeyboardState(NULL);
	if (worldIsMoving) {
		if (mouse.buttonMask == ppl7::tk::MouseState::Middle || ((mouse.buttonMask == ppl7::tk::MouseState::Left) && state[SDL_SCANCODE_LSHIFT])) {
			//printf("Move\n");
			moveWorld(WorldMoveStart.x - mouse.p.x, WorldMoveStart.y - mouse.p.y);
			WorldMoveStart=mouse.p;
		} else {
			worldIsMoving=false;
			//printf("End\n");
		}
	} else {
		//printf("mouse.buttonMask=%d\n", mouse.button);
		if (mouse.buttonMask == ppl7::tk::MouseState::Middle || ((mouse.buttonMask == ppl7::tk::MouseState::Left) && state[SDL_SCANCODE_LSHIFT])) {
			//printf("Start\n");
			if (showui) {
				worldIsMoving=true;
				WorldMoveStart=mouse.p;
				mainmenue->setWorldFollowsPlayer(false);
			}
		} else {
			worldIsMoving=false;;
		}

	}
}

void Game::updateUi(const ppl7::tk::MouseState& mouse)
{
	fps.update();
	statusbar->setFps(fps.getFPS());
	statusbar->setMouse(mouse);
	statusbar->setWorldCoords(WorldCoords);
	if (player)
		statusbar->setPlayerCoords(ppl7::grafix::Point(player->x, player->y));
	statusbar->setSpriteCount(level.countSprites(), level.countVisibleSprites());
	statusbar->setObjectCount(level.objects->count(), level.objects->countVisible());
	if (player) statusbar->setPlayerState(player->getState());
	world_widget->updatePlayerStats(player);
	if (selected_object) {
		statusbar->setSelectedObject(selected_object->id);
	} else {
		statusbar->setSelectedObject(-1);
	}
}

void Game::updateWorldCoords()
{
	if (!player) return;
	int mx=viewport.width() / 2;
	int my=viewport.height() / 2 + 192;	//256
	WorldCoords.x=player->x - mx;
	WorldCoords.y=player->y - my;
	if (WorldCoords.x < 0) WorldCoords.x=0;
	if (WorldCoords.y < 0) WorldCoords.y=0;
}

ppl7::grafix::Point Game::getViewPos() const
{
	ppl7::grafix::Point p=WorldCoords;
	p.x+=viewport.width() / 2;
	p.y+=viewport.height() / 2;
	return p;
}

void Game::drawWorld(SDL_Renderer* renderer)
{
	double now=ppl7::GetMicrotime();
	level.setEditmode(object_selection != NULL);
	level.updateVisibleSpriteLists(WorldCoords, viewport);	// => TODO: own Thread
	player->setGodMode(mainmenue->godModeEnabled());
	if (this->controlsEnabled)
		player->update(now, level.TileTypeMatrix, level.objects);
	level.objects->update(now, level.TileTypeMatrix, *player);
	ppl7::tk::MouseState mouse=wm->getMouseState();
	if (mainmenue->worldFollowsPlayer())
		updateWorldCoords();

	updateUi(mouse);



	// TODO: Refactor into Events: Handle Mouse events inside World
	if (mouse.p.inside(viewport)) {
		moveWorldOnMouseClick(mouse);
	}

	level.setViewport(viewport);

	// Draw background
	background.setColor(level.params.BackgroundColor);
	background.setImage(level.params.BackgroundImage);
	background.setBackgroundType(level.params.backgroundType);
	background.draw(renderer, viewport, WorldCoords);

	// Draw Planes and Sprites
	level.FarPlane.setVisible(mainmenue->visibility_plane_far);
	level.PlayerPlane.setVisible(mainmenue->visibility_plane_player);
	level.FrontPlane.setVisible(mainmenue->visibility_plane_front);
	level.BackPlane.setVisible(mainmenue->visibility_plane_back);
	level.MiddlePlane.setVisible(mainmenue->visibility_plane_middle);
	level.HorizonPlane.setVisible(mainmenue->visibility_plane_horizon);
	level.NearPlane.setVisible(mainmenue->visibility_plane_near);
	level.setShowSprites(mainmenue->visibility_sprites);
	level.setShowObjects(mainmenue->visibility_objects);
	level.draw(renderer, WorldCoords, player);

	if (player->isDead() == true && death_state == 0) {
		death_state=1;
		fade_to_black=0;
	}
	if (death_state) handleDeath(renderer);
	else if (fade_to_black > 0) {
		fade_to_black-=5;
		if (fade_to_black < 0) fade_to_black=0;
	}
	if (fade_to_black > 0) {
		SDL_BlendMode currentBlendMode;
		SDL_GetRenderDrawBlendMode(renderer, &currentBlendMode);
		//SDL_BlendMode newBlendMode=SDL_BLENDMODE_BLEND;
		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, fade_to_black);
		SDL_RenderFillRect(renderer, NULL);
		SDL_SetRenderDrawBlendMode(renderer, currentBlendMode);
	}
}

void Game::run()
{
	resizeEvent(NULL);

	death_state=0;
	fade_to_black=255;
	world_widget->setVisible(true);
	world_widget->setEnabled(true);
	wm->setKeyboardFocus(world_widget);
	SDL_Renderer* renderer=sdl.getRenderer();
	quitGame=false;
	while (!quitGame) {
		checkSoundtrack();
		wm->handleEvents();
		if (filedialog) checkFileDialog();
		drawWorld(renderer);

		ppl7::tk::MouseState mouse=wm->getMouseState();
		drawSelectedSprite(renderer, mouse.p);
		drawSelectedObject(renderer, mouse.p);
		drawSelectedTile(renderer, mouse.p);
		if (mainmenue->visibility_plane_player) {
			if (mainmenue->visibility_tiletypes) level.TileTypeMatrix.draw(renderer, viewport, WorldCoords);
			if (mainmenue->visibility_collision) player->drawCollision(renderer, viewport, WorldCoords);
			if (waynet_edit) level.waynet.draw(renderer, viewport, WorldCoords);
		}
		// Grid
		if (mainmenue->visibility_grid) drawGrid();

		// Widgets
		drawWidgets();
		// Mouse
		resources.Cursor.draw(renderer, mouse.p.x, mouse.p.y, 1);
		presentScreen();
	}
	soundtrack.fadeout(4.0f);
}

void Game::quitEvent(ppl7::tk::Event* e)
{
	quitGame=true;
}

void Game::closeEvent(ppl7::tk::Event* e)
{
	quitGame=true;
}

void Game::closeTileTypeSelection()
{
	if (tiletype_selection) {
		this->removeChild(tiletype_selection);
		delete(tiletype_selection);
		tiletype_selection=NULL;
		viewport.x1=0;
		world_widget->setViewport(viewport);
		mainmenue->setShowTileTypes(false);
	}
}

void Game::closeTileSelection()
{
	if (tiles_selection) {
		remember.lastTile=tiles_selection->selectedTile();
		remember.lastTileColor=tiles_selection->colorIndex();
		remember.lastTileset=tiles_selection->currentTileSet();
		remember.lastTileLayer=tiles_selection->currentLayer();
		this->removeChild(tiles_selection);
		delete(tiles_selection);
		tiles_selection=NULL;
		viewport.x1=0;
		world_widget->setViewport(viewport);
	}
}

void Game::closeSpriteSelection()
{
	if (sprite_selection) {
		this->removeChild(sprite_selection);
		delete(sprite_selection);
		sprite_selection=NULL;
		viewport.x1=0;
		world_widget->setViewport(viewport);
	}
}

void Game::closeObjectSelection()
{
	if (object_selection) {
		this->removeChild(object_selection);
		delete(object_selection);
		object_selection=NULL;
		viewport.x1=0;
		world_widget->setViewport(viewport);
	}
}

void Game::closeWayNet()
{
	if (waynet_edit) {
		this->removeChild(waynet_edit);
		delete(waynet_edit);
		waynet_edit=NULL;
		viewport.x1=0;
		world_widget->setViewport(viewport);
		mainmenue->setShowTileTypes(false);
	}
}



void Game::showTilesSelection()
{
	closeTileTypeSelection();
	closeSpriteSelection();
	closeObjectSelection();
	closeWayNet();
	if (tiles_selection) {
		closeTileSelection();
	} else {
		tiles_selection=new Decker::ui::TilesSelection(0, 33, 300, statusbar->y() - 2 - 33, this);
		for (int i=1;i <= resources.getMaxTilesetId();i++) {
			tiles_selection->setTileSet(i, resources.bricks[i].name, &resources.bricks[i].ui);
		}
		tiles_selection->setCurrentTileSet(remember.lastTileset);
		tiles_selection->setSelectedTile(remember.lastTile);
		tiles_selection->setColorIndex(remember.lastTileColor);
		tiles_selection->setLayer(remember.lastTileLayer);
		this->addChild(tiles_selection);
		viewport.x1=300;
		world_widget->setViewport(viewport);
	}
}

void Game::showTileTypeSelection()
{
	closeSpriteSelection();
	closeTileSelection();
	closeObjectSelection();
	closeWayNet();
	if (tiletype_selection) {
		closeTileTypeSelection();
		mainmenue->setShowTileTypes(false);
	} else {
		tiletype_selection=new Decker::ui::TileTypeSelection(0, 33, 300, statusbar->y() - 2 - 33, this, &resources.TileTypes);
		this->addChild(tiletype_selection);
		viewport.x1=300;
		world_widget->setViewport(viewport);
		mainmenue->setShowTileTypes(true);
		mainmenue->setCurrentPlane(0);
	}
}

void Game::showSpriteSelection()
{
	closeTileTypeSelection();
	closeTileSelection();
	closeObjectSelection();
	closeWayNet();
	if (sprite_selection) {
		closeSpriteSelection();
	} else {
		sprite_selection=new Decker::ui::SpriteSelection(0, 33, 300, statusbar->y() - 2 - 33, this);
		sprite_selection->setSpriteSet(1, "Nature", &resources.uiSpritesNature, 4);
		sprite_selection->setSpriteSet(2, "Plants", &resources.uiSpritesPlants);
		sprite_selection->setSpriteSet(3, "Objects", &resources.uiSpritesObjects, 1);
		sprite_selection->setSpriteSet(4, "Nature Blury", &resources.uiSpritesNature, 1);
		sprite_selection->setSpriteSet(5, "Rocks", &resources.uiSpritesRocks, 4);
		sprite_selection->setSpriteSet(6, "Flowers", &resources.uiSpritesFlowers, 1);
		sprite_selection->setSpriteSet(7, "Treasure", &resources.uiSpritesTreasure, 1);
		sprite_selection->setSpriteSet(8, "Recolorable Sprites", &resources.uiSpritesWhite, 8);
		this->addChild(sprite_selection);
		viewport.x1=300;
		sprite_mode=spriteModeDraw;
		selected_sprite.id=-1;
		selected_sprite_system=NULL;
		world_widget->setViewport(viewport);
	}
}

void Game::showObjectsSelection()
{
	closeTileTypeSelection();
	closeTileSelection();
	closeSpriteSelection();
	closeWayNet();
	if (object_selection) {
		closeObjectSelection();
	} else {
		object_selection=new Decker::ui::ObjectSelection(0, 33, 300, statusbar->y() - 2 - 33, this);
		object_selection->setSpriteSet(&resources.uiObjects);
		this->addChild(object_selection);
		viewport.x1=300;
		world_widget->setViewport(viewport);
		sprite_mode=SpriteModeSelect;
		selected_object=NULL;
	}
}

void Game::showWayNetEdit()
{
	closeTileTypeSelection();
	closeTileSelection();
	closeSpriteSelection();
	closeObjectSelection();
	if (waynet_edit) {
		closeWayNet();
		mainmenue->setShowTileTypes(false);
	} else {
		waynet_edit=new Decker::ui::WayNetEdit(0, 33, 300, statusbar->y() - 2 - 33, this);
		//waynet_edit->setSpriteSet(&resources.uiObjects);
		this->addChild(waynet_edit);
		viewport.x1=300;
		world_widget->setViewport(viewport);
		mainmenue->setShowTileTypes(true);
	}
}





void Game::handleMouseDrawInWorld(const ppl7::tk::MouseState& mouse)
{
	const Uint8* state = SDL_GetKeyboardState(NULL);
	//if (state[SDL_SCANCODE_LSHIFT]) return;

	if (tiletype_selection) {
		ppl7::grafix::Point coords=WorldCoords * planeFactor[0];
		int x=(mouse.p.x + coords.x) / TILE_WIDTH;
		int y=(mouse.p.y + coords.y) / TILE_HEIGHT;
		TileType::Type type=(TileType::Type)tiletype_selection->tileType();
		if (mouse.buttonMask == ppl7::tk::MouseState::Left) {
			level.TileTypeMatrix.setType(x, y, type);
		} else if (mouse.buttonMask == ppl7::tk::MouseState::Right) {
			level.TileTypeMatrix.setType(x, y, TileType::Type::NonBlocking);
		}
	} else if (tiles_selection) {
		int currentPlane=mainmenue->currentPlane();

		ppl7::grafix::Point coords=WorldCoords * planeFactor[currentPlane];
		int x=(mouse.p.x + coords.x) / TILE_WIDTH;
		int y=(mouse.p.y + coords.y) / TILE_HEIGHT;

		int selectedTile=tiles_selection->selectedTile();
		int selectedTileSet=tiles_selection->currentTileSet();
		int currentLayer=tiles_selection->currentLayer();
		int color_index=tiles_selection->colorIndex();
		Plane& plane=level.plane(currentPlane);

		if ((mouse.buttonMask == ppl7::tk::MouseState::Right || mouse.buttonMask == ppl7::tk::MouseState::Middle)
			&& state[SDL_SCANCODE_LSHIFT]) {
				// Pick Tile
			ppl7::grafix::Point p=plane.getOccupationOrigin(x, y, currentLayer);
			if (p.x >= 0 && p.y >= 0) {
				tiles_selection->setCurrentTileSet(plane.getTileSet(p.x, p.y, currentLayer));
				tiles_selection->setSelectedTile(plane.getTileNo(p.x, p.y, currentLayer));
				tiles_selection->setColorIndex(plane.getColorIndex(p.x, p.y, currentLayer));
			}
		} else if (mouse.buttonMask == ppl7::tk::MouseState::Left && selectedTile >= 0 && state[SDL_SCANCODE_LSHIFT] == 0) {
			BrickOccupation::Matrix occupation=brick_occupation.get(selectedTile);
			if (selectedTileSet == 1) occupation=brick_occupation_solid;
			if (!plane.isOccupied(x, y, currentLayer, occupation)) {
				plane.setTile(x, y,
					currentLayer,
					selectedTileSet,
					selectedTile, color_index, true);
				plane.setOccupation(x, y, currentLayer, occupation);
			}
		} else if (mouse.buttonMask == ppl7::tk::MouseState::Right && state[SDL_SCANCODE_LSHIFT] == 0) {
			ppl7::grafix::Point origin=plane.getOccupationOrigin(x, y, currentLayer);
			if (origin.x >= 0 && origin.y >= 0) {
				int origin_tile=plane.getTileNo(origin.x, origin.y, currentLayer);
				int origin_tileset=plane.getTileSet(origin.x, origin.y, currentLayer);
				if (origin_tile >= 0) {
					BrickOccupation::Matrix occupation=brick_occupation.get(origin_tile);
					if (origin_tileset == 1) occupation=brick_occupation_solid;
					plane.clearOccupation(origin.x, origin.y, currentLayer, occupation);
				}
				plane.clearTile(origin.x, origin.y, currentLayer);
			} else {
				plane.clearTile(x, y, currentLayer);
			}
		}
	}
}

void Game::setSpriteModeToDraw()
{
	sprite_mode=spriteModeDraw;
	selected_object=NULL;
}

void Game::drawSelectedSprite(SDL_Renderer* renderer, const ppl7::grafix::Point& mouse)
{
	if (!sprite_selection) return;
	if (sprite_selection->selectedSprite() >= 0 && sprite_mode != spriteModeDraw) {
		selected_sprite_system=NULL;
		sprite_mode=spriteModeDraw;
	}
	if (sprite_mode == SpriteModeEdit && selected_sprite.id >= 0 && selected_sprite_system != NULL) {
		int currentPlane=mainmenue->currentPlane();
		selected_sprite_system->drawSelectedSpriteOutline(renderer, viewport,
			WorldCoords * planeFactor[currentPlane], selected_sprite.id);
	} else if (sprite_mode == spriteModeDraw) {
		if (!mouse.inside(viewport)) return;
		int nr=sprite_selection->selectedSprite();
		if (nr < 0) return;
		int spriteset=sprite_selection->currentSpriteSet();
		int sprite_dimensions=sprite_selection->spriteSetDimensions();
		if (spriteset == 7) {
			if (nr == 0) nr=ppl7::rand(0, 47);
			else nr=(nr - 1) * 6;
		} else if (sprite_dimensions > 1) {
			//nr=nr * sprite_dimensions + ppl7::rand(0, sprite_dimensions - 1);
			nr=nr * sprite_dimensions;
		}
		float scale=sprite_selection->spriteScale();
		if (!level.spriteset[spriteset]) return;
		level.spriteset[spriteset]->drawScaled(renderer,
			mouse.x, mouse.y, nr, scale, level.palette.getColor(sprite_selection->colorIndex()));
		level.spriteset[spriteset]->drawOutlines(renderer,
			mouse.x, mouse.y, nr, scale);
	}
}

void Game::drawSelectedTile(SDL_Renderer* renderer, const ppl7::grafix::Point& mouse)
{
	if (!tiles_selection) return;
	if (!mouse.inside(viewport)) return;
	int currentPlane=mainmenue->currentPlane();
	int currentLayer=tiles_selection->currentLayer();
	int nr=tiles_selection->selectedTile();
	int tileset=tiles_selection->currentTileSet();
	int color_index=tiles_selection->colorIndex();
	if (nr < 0 || tileset<0 || tileset>MAX_TILESETS) return;
	if (!level.tileset[tileset]) return;
	ppl7::grafix::Point wp=mouse - viewport.topLeft() + WorldCoords * planeFactor[currentPlane];
	int tx=wp.x / TILE_WIDTH;
	int ty=wp.y / TILE_HEIGHT;
	BrickOccupation::Matrix occupation=brick_occupation.get(nr);
	if (tileset == 1) occupation=brick_occupation_solid;
	if (!level.plane(currentPlane).isOccupied(tx, ty, currentLayer, occupation)) {
		int x=tx * TILE_WIDTH + viewport.x1 - WorldCoords.x * planeFactor[currentPlane];;
		int y=ty * TILE_HEIGHT + viewport.y1 - WorldCoords.y * planeFactor[currentPlane];;
		level.tileset[tileset]->draw(renderer,
			x, y + TILE_HEIGHT, nr, level.palette.getColor(color_index));
	}

}

void Game::drawSelectedObject(SDL_Renderer* renderer, const ppl7::grafix::Point& mouse)
{
	if (!object_selection) return;
	if (sprite_mode == SpriteModeEdit && selected_object != NULL) {
		level.objects->drawSelectedSpriteOutline(renderer, viewport,
			WorldCoords, selected_object->id);
	} else if (sprite_mode == spriteModeDraw) {
		if (!mouse.inside(viewport)) return;
		int object_type=object_selection->selectedObjectType();
		if (object_type < 0) return;
		level.objects->drawPlaceSelection(renderer, mouse, object_type);
	}
}


void Game::startLevel(const ppl7::String& filename)
{
	if (!ppl7::File::exists(filename)) {
		return;
	}
	closeTileTypeSelection();
	closeTileSelection();
	closeSpriteSelection();
	closeObjectSelection();
	closeWayNet();
	remember.clear();
	level.load(filename);
	LevelFile=filename;
	mainmenue->update();
	ppl7::grafix::Point startpoint=level.objects->findPlayerStart();
	mainmenue->setWorldFollowsPlayer(true);
	if (startpoint.x > 0) {
		player->move(startpoint.x, startpoint.y);
		player->setSavePoint(startpoint);
		player->setVisible(true);
		enableControls(true);

	} else {
		player->setVisible(false);
		enableControls(false);
	}
	soundtrack.playInitialSong();
	background.setBackgroundType(level.params.backgroundType);
	background.setColor(level.params.BackgroundColor);
	background.setImage(level.params.BackgroundImage);
	background.setLevelDimension(level.getOccupiedAreaFromTileTypePlane());

}

void Game::checkSoundtrack()
{
	soundtrack.update();
	if (mainmenue->soundTrackEnabled()) {
		audiosystem.setVolume(AudioClass::Music, config.volumeMusic);
	} else {
		audiosystem.setVolume(AudioClass::Music, 0.0f);
	}

}

const ppl7::String& Game::getLevelFilename() const
{
	return LevelFile;
}

void Game::save(const ppl7::String& filename)
{
	level.save(filename);
	LevelFile=filename;
	if (mainmenue) mainmenue->update();
}

void Game::load()
{
	closeTileTypeSelection();
	closeTileSelection();
	closeSpriteSelection();
	closeObjectSelection();
	closeWayNet();
	remember.clear();
	level.load(LevelFile);
	enableControls(true);
	if (mainmenue) mainmenue->update();
}

void Game::createNewLevel(const LevelParameter& params)
{
	closeTileTypeSelection();
	closeTileSelection();
	closeSpriteSelection();
	closeObjectSelection();
	closeWayNet();
	remember.clear();
	enableControls(false);
	if (mainmenue) mainmenue->setWorldFollowsPlayer(false);
	WorldCoords.setPoint(0, 0);
	level.create(params.width, params.height);
	level.params=params;
	if (player) {
		player->move(500, 500);
		player->setVisible(false);
	}
	LevelFile.clear();
	enableControls(false);
	if (mainmenue) mainmenue->update();
}

void Game::mouseDownEvent(ppl7::tk::MouseEvent* event)
{
	if (event->widget() == world_widget) wm->setKeyboardFocus(world_widget);
	if (sprite_selection != NULL && event->widget() == world_widget) {
		mouseDownEventOnSprite(event);
	} else if (object_selection != NULL && event->widget() == world_widget) {
		mouseDownEventOnObject(event);
	} else if ((tiles_selection != NULL || tiletype_selection != NULL) && event->widget() == world_widget) {
		handleMouseDrawInWorld(*event);
	} else if (waynet_edit != NULL && event->widget() == world_widget) {
		mouseDownEventOnWayNet(event);
	}
}


void Game::mouseDownEventOnSprite(ppl7::tk::MouseEvent* event)
{
	if (event->widget() == world_widget && event->buttonMask == ppl7::tk::MouseState::Left) {
		int nr=sprite_selection->selectedSprite();
		if (nr < 0) {
			selectSprite(event->p);
			return;
		}
		if (sprite_mode != spriteModeDraw) return;
		int spriteset=sprite_selection->currentSpriteSet();
		int sprite_dimensions=sprite_selection->spriteSetDimensions();
		if (spriteset == 7) {
			if (nr == 0) nr=ppl7::rand(0, 47);
			else nr=(nr - 1) * 6 + ppl7::rand(0, 5);
		} else if (sprite_dimensions > 1) {
			nr=nr * sprite_dimensions + ppl7::rand(0, sprite_dimensions - 1);
		}
		float scale=sprite_selection->spriteScale();
		int layer=sprite_selection->currentLayer();
		if (layer < 0 || layer>1 || spriteset > MAX_SPRITESETS) return;
		if (!level.spriteset[spriteset]) return;
		int currentPlane=mainmenue->currentPlane();
		SpriteSystem& ss=level.spritesystem(currentPlane, layer);
		ppl7::grafix::Point coords=WorldCoords * planeFactor[currentPlane];
		ss.addSprite(event->p.x + coords.x,
			event->p.y + coords.y,
			0,
			spriteset, nr, scale, sprite_selection->colorIndex());
	} else if (event->widget() == world_widget && event->buttonMask == ppl7::tk::MouseState::Right) {
		sprite_selection->setSelectedSprite(-1);
		sprite_mode=spriteModeDraw;
		selected_sprite.id=-1;
		selected_sprite_system=NULL;
	}
}

void Game::mouseDownEventOnObject(ppl7::tk::MouseEvent* event)
{
	if (event->widget() == world_widget && (event->buttonMask == ppl7::tk::MouseState::Middle
		|| (event->buttonMask == ppl7::tk::MouseState::Left && SDL_GetKeyboardState(NULL)[SDL_SCANCODE_LSHIFT]))) {
		Decker::Objects::Object* object=level.objects->findMatchingObject(event->p + WorldCoords);
		if (object) {
			wm->setKeyboardFocus(world_widget);
			sprite_mode=SpriteModeEdit;
			selected_object=object;
			sprite_move_start=event->p;
			object->openUi();
		}
	} else if (event->widget() == world_widget && event->buttonMask == ppl7::tk::MouseState::Left) {
		int object_type=object_selection->selectedObjectType();
		if (object_type < 0 || sprite_mode == SpriteModeSelect || sprite_mode == SpriteModeEdit) {
			sprite_mode=SpriteModeSelect;
			Decker::Objects::Object* object=level.objects->findMatchingObject(event->p + WorldCoords);
			if (object) {
				//printf ("found Object with id %d\n", object->id);
				wm->setKeyboardFocus(world_widget);
				sprite_mode=SpriteModeEdit;
				//if (selected_object==object) object->openUi();
				selected_object=object;
				sprite_move_start=event->p;

			}
			return;
		}
		if (sprite_mode != spriteModeDraw) return;
		selected_object=level.objects->getInstance(object_type);
		if (selected_object) {
			ppl7::grafix::Point coords=WorldCoords;
			selected_object->myLayer=static_cast<Decker::Objects::Object::Layer>(object_selection->currentLayer());
			selected_object->initial_p.setPoint(event->p.x + coords.x, event->p.y + coords.y);
			selected_object->p=selected_object->initial_p;
			level.objects->addObject(selected_object);
			sprite_mode=spriteModeDraw;
		}
	} else if (event->widget() == world_widget && event->buttonMask == ppl7::tk::MouseState::Right) {
		sprite_mode=SpriteModeSelect;
		selected_object=NULL;
	}
}

void Game::mouseDownEventOnWayNet(ppl7::tk::MouseEvent* event)
{
	ppl7::grafix::Point coords=WorldCoords;
	int x=(event->p.x + coords.x) / TILE_WIDTH;
	int y=(event->p.y + coords.y) / TILE_HEIGHT;
	WayPoint wp(x, y);
	if (event->buttonMask == ppl7::tk::MouseState::Left) {
		if (level.waynet.hasPoint(wp)) {
			//printf ("Point selected\n");
			if (level.waynet.hasSelection()) {
				const WayPoint p1=level.waynet.getSelection();
				level.waynet.deleteConnection(p1, wp);
				Connection::ConnectionType type=(Connection::ConnectionType)waynet_edit->getSelectedWayType();
				if (type != Connection::Invalid) {
					level.waynet.addConnection(p1,
						Connection(p1, wp, type,
							waynet_edit->getCost()));
				}
				level.waynet.setSelection(wp);
			} else {
				level.waynet.setSelection(wp);
			}
		} else {
			level.waynet.addPoint(wp);
			level.waynet.clearSelection();
		}
	} else if (event->buttonMask == ppl7::tk::MouseState::Right) {
		level.waynet.deletePoint(wp);
		level.waynet.clearSelection();
	}
}

void Game::mouseWheelEvent(ppl7::tk::MouseEvent* event)
{
	if (sprite_selection != NULL && event->widget() == world_widget) {
		if (sprite_mode == spriteModeDraw) {
			float scale=sprite_selection->spriteScale();
			if (event->wheel.y < 0 && scale>0.1) scale-=0.1;
			else if (event->wheel.y > 0 && scale < 1.0) scale+=0.1;
			sprite_selection->setSpriteScale(scale);
		} else if (sprite_mode == SpriteModeEdit && selected_sprite.id >= 0 && selected_sprite_system != NULL) {
			//printf ("wheel\n");
			if (event->wheel.y < 0 && selected_sprite.scale>0.1) selected_sprite.scale-=0.1;
			else if (event->wheel.y > 0 && selected_sprite.scale < 1.0) selected_sprite.scale+=0.1;
			selected_sprite_system->modifySprite(selected_sprite);
		}
	}
}

void Game::selectSprite(const ppl7::grafix::Point& mouse)
{
	int plane=0;
	int layer=0;
	if (level.findSprite(mouse, WorldCoords, selected_sprite, plane, layer)) {
		//printf ("found Sprite on plane %d, layer %d\n",plane,layer);
		mainmenue->setCurrentPlane(plane);
		sprite_selection->setCurrentLayer(layer);
		sprite_selection->setCurrentSpriteSet(selected_sprite.sprite_set);
		wm->setKeyboardFocus(world_widget);
		sprite_mode=SpriteModeEdit;
		selected_sprite_system=selected_sprite.spritesystem;
		sprite_move_start=mouse;
	} else {
		selected_sprite.id=-1;
		selected_sprite_system=NULL;
	}
}


void Game::keyDownEvent(ppl7::tk::KeyEvent* event)
{
	if (event->widget() == world_widget) {
		if (sprite_mode == SpriteModeEdit && sprite_selection != NULL && selected_sprite.id >= 0
			&& selected_sprite_system != NULL) {
			if (event->key == ppl7::tk::KeyEvent::KEY_DELETE
				&& (event->modifier & ppl7::tk::KeyEvent::KEYMOD_MODIFIER) == 0) {
			//printf ("KeyEvent\n");
				selected_sprite_system->deleteSprite(selected_sprite.id);
				selected_sprite.id=-1;
				selected_sprite_system=NULL;
			}
		} else if (sprite_mode == SpriteModeEdit && object_selection != NULL && selected_object != NULL) {
			if (event->key == ppl7::tk::KeyEvent::KEY_DELETE
				&& (event->modifier & ppl7::tk::KeyEvent::KEYMOD_MODIFIER) == 0) {
			//printf ("KeyEvent\n");
				level.objects->deleteObject(selected_object->id);
				selected_object=NULL;
			}
		}
	}
	//printf("keyDownEvent: %d, modifier: %04x\n", event->key, event->modifier);
	if (event->key == ppl7::tk::KeyEvent::KEY_F4) {
		// TODO: for developing only
		ppl7::grafix::Point pos=level.objects->nextPlayerStart();
		player->move(pos.x, pos.y);
		player->setSavePoint(pos);
	} else if (event->key == ppl7::tk::KeyEvent::KEY_F3) {
		level.load("level/test.lvl");
	} else if (event->key == ppl7::tk::KeyEvent::KEY_F9) {
		showUi(!showui);
	} else if (event->key == ppl7::tk::KeyEvent::KEY_ESCAPE) {
		// TODO

	} else if (event->key == ppl7::tk::KeyEvent::KEY_RETURN && (event->modifier & ppl7::tk::KeyEvent::KEYMOD_ALT) > 0) {
		printf("toggle fullscreen or back\n");
		ppl7::tk::WindowManager_SDL2* sdl2wm=(ppl7::tk::WindowManager_SDL2*)wm;
		Window::WindowMode mode=sdl2wm->getWindowMode(*this);
		if (mode == Window::WindowMode::Window) {
			windowedSize.setSize(width(), height());
			printf("Aktueller mode ist Window mit %d x %d\n", windowedSize.width, windowedSize.height);
			ppl7::grafix::Size s=sdl.getDisplaySize(config.videoDevice);
			printf("switche zu FullscreenDesktop %d x %d\n", s.width, s.height);
			sdl2wm->changeWindowMode(*this, Window::WindowMode::FullscreenDesktop);
			ppl7::tk::Window::DisplayMode dmode;
			dmode.format=rgbFormat();
			dmode.width=s.width;
			dmode.height=s.height;
			dmode.refresh_rate=config.ScreenRefreshRate;
			setWindowDisplayMode(dmode);
			resizeEvent(NULL);
		} else if (mode == Window::WindowMode::FullscreenDesktop) {
			if (windowedSize.width == 0 || windowedSize.height == 0) windowedSize=config.ScreenResolution;
			printf("Aktueller mode ist FullscreenDesktop, switche zu Fenster %d x %d\n", windowedSize.width, windowedSize.height);
			sdl2wm->changeWindowMode(*this, Window::WindowMode::Window);
			ppl7::tk::Window::DisplayMode dmode;
			dmode.format=rgbFormat();
			dmode.width=windowedSize.width;
			dmode.height=windowedSize.height;
			dmode.refresh_rate=config.ScreenRefreshRate;
			setWindowDisplayMode(dmode);
			resizeEvent(NULL);
		} else {
			printf("Aktueller mode ist Fullscreen\n");
		}
	}
}

void Game::mouseMoveEvent(ppl7::tk::MouseEvent* event)
{
	if (sprite_selection != NULL) {
		if (event->widget() == world_widget && event->buttonMask == ppl7::tk::MouseState::Left
			&& sprite_mode == SpriteModeEdit && selected_sprite.id >= 0
			&& selected_sprite_system != NULL) {
			ppl7::grafix::Point diff=event->p - sprite_move_start;
			selected_sprite.x+=diff.x;
			selected_sprite.y+=diff.y;
			selected_sprite_system->modifySprite(selected_sprite);
			//printf("Move: %d, %d\n", diff.x, diff.y);
			sprite_move_start=event->p;

		}
	} else if (object_selection != NULL) {
		if (event->widget() == world_widget && event->buttonMask == ppl7::tk::MouseState::Left
			&& sprite_mode == SpriteModeEdit && selected_object != NULL) {
			ppl7::grafix::Point diff=event->p - sprite_move_start;
			selected_object->initial_p.x+=diff.x;
			selected_object->initial_p.y+=diff.y;
			selected_object->p=selected_object->initial_p;
			selected_object->updateBoundary();
			sprite_move_start=event->p;
		}
	} else if ((tiles_selection != NULL || tiletype_selection != NULL) && event->widget() == world_widget) {
		handleMouseDrawInWorld(*event);
	}

}

void Game::resizeEvent(ppl7::tk::ResizeEvent* event)
{
	printf("Game::resizeEvent\n");
	fflush(stdout);
	if (tex_level_grid) {
		sdl.destroyTexture(tex_level_grid);
		tex_level_grid=NULL;
	}
	desktopSize=clientSize();
	viewport=clientRect();
	resizeMenueAndStatusbar();
	showUi(showui);
	//printf("Game::resizeEvent, Window sagt: %d x %d\n", this->width(), this->height());
	//if (start_screen) start_screen->resizeEvent(event);
	if (settings_screen) settings_screen->resizeEvent(event);

}

void Game::handleDeath(SDL_Renderer* renderer)
{
	if (death_state == 1) {
		if (fade_to_black < 255) {
			fade_to_black+=5;
			if (fade_to_black > 255) fade_to_black=255;
		} else death_state=2;
	} else if (death_state == 2) {
		player->dropLifeAndResetToLastSavePoint();
		death_state=3;
	} else if (death_state == 3) {
		if (fade_to_black > 0) {
			fade_to_black-=5;
			if (fade_to_black < 0) fade_to_black=0;
		} else death_state=0;
	}
}

void Game::enableControls(bool enable)
{
	controlsEnabled=enable;
}


bool Game::getControlsEnabled() const
{
	return controlsEnabled;
}

void Game::resetPlayer()
{
	if (player) {
		player->resetState();
		if (world_widget) world_widget->resetPlayerStats(player);
	}
}


void Game::openSaveAsDialog()
{
	if (filedialog) delete filedialog;
	filedialog=NULL;
	int w=800;
	int h=600;
	if (w >= width() - 100) w=width() - 100;
	if (h >= height() - 100) h=height() - 100;

	filedialog=new Decker::ui::FileDialog(w, h);
	filedialog->setFilename(LevelFile);
	filedialog->setWindowTitle("save level");
	filedialog->custom_id=1;
	this->addChild(filedialog);
}

void Game::openLoadDialog()
{
	if (filedialog) delete filedialog;
	filedialog=NULL;

	int w=800;
	int h=600;
	if (w >= width() - 100) w=width() - 100;
	if (h >= height() - 100) h=height() - 100;
	filedialog=new Decker::ui::FileDialog(w, h, Decker::ui::FileDialog::FileMode::ExistingFile);
	filedialog->setFilename(LevelFile);
	filedialog->setWindowTitle("load existing level");
	filedialog->custom_id=2;
	this->addChild(filedialog);
}

void Game::checkFileDialog()
{
	if (!filedialog) return;
	if (filedialog->state() == Decker::ui::FileDialog::DialogState::Open) return;
	if (filedialog->state() == Decker::ui::FileDialog::DialogState::Aborted) {
		delete filedialog;
		filedialog=NULL;
		return;
	}
	if (filedialog->custom_id == 1) {	// save level
		ppl7::String filename=filedialog->filename();
		save(filename);
		config.LastEditorLevel=filename;
		config.save();

	} else if (filedialog->custom_id == 2) {	// load level
		ppl7::String filename=filedialog->filename();
		startLevel(filename);
		config.LastEditorLevel=filename;
		config.save();
	}

	delete filedialog;
	filedialog=NULL;
}

void Game::openNewLevelDialog()
{
	showUi(true);
	mainmenue->openLevelDialog(true);
	player->setVisible(false);

}
