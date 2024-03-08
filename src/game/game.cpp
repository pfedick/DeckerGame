#include "decker.h"
#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <ppl7-grafix.h>
#include "player.h"
#include "objects.h"
#include "particle.h"
#include "screens.h"

//#define EVENTTRACKING 1

static ppl7::tk::Window* GameWindow=NULL;
static Game* GameInstance=NULL;

double game_start=0.0f;

void FadeToBlack(SDL_Renderer* renderer, int fade_to_black)
{
	if (fade_to_black > 0) {
		//ppl7::PrintDebugTime("FadeToBlack triggered with value: %d\n", fade_to_black);
		SDL_BlendMode currentBlendMode;
		SDL_GetRenderDrawBlendMode(renderer, &currentBlendMode);
		//SDL_BlendMode newBlendMode=SDL_BLENDMODE_BLEND;
		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, fade_to_black);
		SDL_RenderFillRect(renderer, NULL);
		SDL_SetRenderDrawBlendMode(renderer, currentBlendMode);
	}
}



ppl7::tk::Window* GetGameWindow()
{
	return GameWindow;
}

ColorPalette& GetColorPalette()
{
	return GameInstance->getLevel().palette;
}

Game& GetGame()
{
	return *GameInstance;
}

ppl7::grafix::Point GetViewPos()
{
	return GameInstance->getViewPos();
}

Game::Game()
{
	game_start=ppl7::GetMicrotime();
	GameWindow=this;
	GameInstance=this;
	tex_level_grid=NULL;
	tex_render_target=NULL;
	tex_render_layer=NULL;
	tex_render_lightmap=NULL;
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
	lights_selection=NULL;
	waynet_edit=NULL;
	world_widget=NULL;
	sprite_mode=spriteModeDraw;
	selected_sprite_system=NULL;
	selected_sprite.id=-1;
	selected_light=NULL;

	selected_object=NULL;
	fade_to_black=0;
	death_state=0;
	showui=false;
	controlsEnabled=true;
	settings_screen=NULL;
	filedialog=NULL;
	game_stats_screen=NULL;
	gameState=GameState::None;
	last_frame_time=0.0f;
	frame_rate_compensation=1.0f;
	game_speed=GameSpeed::Normal;
	screenshot=NULL;
}

Game::~Game()
{
	if (settings_screen) delete  settings_screen;
	if (player) delete player;
	if (tex_level_grid) sdl.destroyTexture(tex_level_grid);
	if (tex_render_target) sdl.destroyTexture(tex_render_target);
	if (tex_render_lightmap) sdl.destroyTexture(tex_render_lightmap);
	if (tex_render_layer) sdl.destroyTexture(tex_render_layer);
	if (screenshot) delete screenshot;
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

	resources.Sprites_Tropical.enableOutlines(true);
	resources.Sprites_Tropical.enableMemoryBuffer(true);
	resources.Sprites_Tropical.load(sdl, "res/sprites_tropical.tex");

	resources.Sprites_White1D.enableOutlines(true);
	resources.Sprites_White1D.enableMemoryBuffer(true);
	resources.Sprites_White1D.load(sdl, "res/sprites_white_x1.tex");

	resources.Lightmaps.enableOutlines(true);
	resources.Lightmaps.enableMemoryBuffer(true);
	resources.Lightmaps.setTextureBlendMode(SDL_BLENDMODE_ADD);
	resources.Lightmaps.load(sdl, "res/lightmaps.tex");
	resources.Lightmaps.setPivot(8, 256, 27);
	resources.Lightmaps.setPivot(9, 256, 27);
	resources.Lightmaps.setPivot(10, 256, 27);
	resources.Lightmaps.setPivot(11, 256, 27);
	resources.Lightmaps.setPivot(12, 256, 27);
	resources.Lightmaps.setPivot(13, 256, 27);


	resources.LightObjects.enableOutlines(true);
	resources.LightObjects.enableMemoryBuffer(true);
	resources.LightObjects.load(sdl, "res/lightobjects.tex");


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

	resources.uiSprites_White1D.enableSDLBuffer(false);
	resources.uiSprites_White1D.enableMemoryBuffer(true);
	resources.uiSprites_White1D.load(sdl, "res/sprites_white_x1_ui.tex");

	resources.uiSpritesTropical.enableSDLBuffer(false);
	resources.uiSpritesTropical.enableMemoryBuffer(true);
	resources.uiSpritesTropical.load(sdl, "res/sprites_tropical_ui.tex");

	resources.uiLightmaps.enableSDLBuffer(false);
	resources.uiLightmaps.enableMemoryBuffer(true);
	resources.uiLightmaps.load(sdl, "res/lightmaps_ui.tex");

	resources.Waynet.load(sdl, "res/waynet.tex");

	level.objects->loadSpritesets(sdl);
	level.particles->loadSpritesets(sdl);
	level.lights.loadSpritesets(sdl);
	level.waynet.setSpriteset(&resources.Waynet);
	//level.setLightset(&resources.Lightmaps, &resources.LightObjects);

	message_overlay.loadSprites();

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
	wm->setGameControllerFocus(this);

	SDL_ShowCursor(SDL_DISABLE);
}

void Game::createRenderTarget()
{
	tex_render_target=sdl.createRenderTargetTexture(1920, 1080);
	if (tex_render_target) SDL_SetTextureScaleMode(tex_render_target, SDL_ScaleModeBest);
	tex_render_layer=sdl.createRenderTargetTexture(1920, 1080);
	tex_render_lightmap=sdl.createRenderTargetTexture(1920, 1080);

	SDL_SetTextureBlendMode(tex_render_layer, SDL_BLENDMODE_BLEND);
	SDL_SetTextureBlendMode(tex_render_lightmap, SDL_BLENDMODE_MUL);
	level.setRenderTargets(tex_render_target, tex_render_lightmap, tex_render_layer);

}

ppl7::tk::Window& Game::window()
{
	return *this;
}

Level& Game::getLevel()
{
	return level;
}

LightSystem& Game::getLightSystem()
{
	return level.lights;
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
		viewport.y1=32;
		viewport.y2=desktop.height - 32;
		world_widget->setViewport(viewport);
		mainmenue->setVisible(true);
		mainmenue->fitMetrics(viewport);
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
		mainmenue->fitMetrics(viewport);
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
	translator.load();
	translator.setLanguage(config.TextLanguage);
	createWindow();
	createRenderTarget();
	initUi();
	initAudio();
	initGameController();

	desktopSize=clientSize();
	viewport=clientRect();
	message_overlay.resize(viewport.size());

	gui_font.setName("Default");
	gui_font.setSize(12);
	gui_font.setBold(true);
	gui_font.setColor(ppl7::grafix::Color(255, 255, 255, 255));
	gui_font.setBorderColor(ppl7::grafix::Color(0, 0, 0, 0));
	gui_font.setShadowColor(ppl7::grafix::Color(0, 0, 0, 0));
	gui_font.setOrientation(ppl7::grafix::Font::TOP);
	gui_font.setAntialias(true);
}

void Game::initGameController()
{
	std::list<GameController::Device>device_list=GameController::enumerate();
	if (device_list.size() > 0) {
		controller.open(device_list.front());
	}
	updateGameControllerMapping();
}

void Game::updateGameControllerMapping()
{
	controller.setDeadzone(config.controller.deadzone);
	controller.mapping.setMappingAxis(GameControllerMapping::Axis::Walk, config.controller.axis_walk);
	controller.mapping.setMappingAxis(GameControllerMapping::Axis::Jump, config.controller.axis_jump);
	controller.mapping.setMappingButton(GameControllerMapping::Button::MenuUp, config.controller.button_up);
	controller.mapping.setMappingButton(GameControllerMapping::Button::MenuDown, config.controller.button_down);
	controller.mapping.setMappingButton(GameControllerMapping::Button::MenuLeft, config.controller.button_left);
	controller.mapping.setMappingButton(GameControllerMapping::Button::MenuRight, config.controller.button_right);
	controller.mapping.setMappingButton(GameControllerMapping::Button::Menu, config.controller.button_menu);
	controller.mapping.setMappingButton(GameControllerMapping::Button::Action, config.controller.button_action);
	controller.mapping.setMappingButton(GameControllerMapping::Button::Jump, config.controller.button_jump);
	controller.mapping.setMappingButton(GameControllerMapping::Button::Back, config.controller.button_back);
	controller.mapping.updateMapping();

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
	level.setSpriteset(9, &resources.Sprites_Tropical);
	level.setSpriteset(10, &resources.Sprites_White1D);

	level.TileTypeMatrix.setTileTypesSprites(&resources.TileTypes);

	//ppl7::PrintDebugTime("Size of audio pool: %zd MB\n", audiopool.size() / 1024 / 1024);

	ppl7::PrintDebugTime("Startup time: %0.3f s, size of Audio pool: %zd MB\n",
		ppl7::GetMicrotime() - game_start,
		audiopool.size() / 1024 / 1024
	);

}

void Game::initAudio()
{
	audiosystem.init();
	audiosystem.setGlobalVolume(config.volumeTotal);
	audiosystem.setVolume(AudioClass::Effect, config.volumeEffects);
	audiosystem.setVolume(AudioClass::Music, config.volumeMusic);
	audiopool.load();
	audiopool.load_speech(config.SpeechLanguage);
	audiopool.setAudioSystem(&audiosystem);
}

void Game::drawGrid()
{
	if (!tex_level_grid) {
		tex_level_grid=sdl.createStreamingTexture(game_viewport.width(), game_viewport.height());
		ppl7::grafix::Drawable draw=sdl.lockTexture(tex_level_grid);
		ppl7::grafix::Color white(255, 255, 255, 128);
		ppl7::grafix::Color black(0, 0, 0, 128);

		draw.cls(0);
		for (int x=0;x < game_viewport.width();x+=TILE_WIDTH) {
			draw.line(x, 0, x, game_viewport.height(), black);
			draw.line(x + 1, 0, x + 1, game_viewport.height(), white);
		}
		for (int y=0;y < game_viewport.height();y+=TILE_HEIGHT) {
			draw.line(0, y, game_viewport.width(), y, black);
			draw.line(0, y + 1, game_viewport.width(), y + 1, white);
		}
		sdl.unlockTexture(tex_level_grid);
	}
	int currentPlane=mainmenue->currentPlane();
	ppl7::grafix::Point c=WorldCoords * planeFactor[currentPlane];

	SDL_Renderer* renderer=sdl.getRenderer();
	SDL_Rect target;
	target.x=game_viewport.x1 - (c.x % TILE_WIDTH);
	target.y=game_viewport.y1 - (c.y % TILE_HEIGHT);
	target.w=game_viewport.width();
	target.h=game_viewport.height();
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

void Game::updateUi(const ppl7::tk::MouseState& mouse, const Metrics& last_metrics)
{
	fps.update();

	statusbar->setFps(fps.getFPS());
	statusbar->setMouse(mouse);
	statusbar->setWorldCoords(WorldCoords);
	if (player)
		statusbar->setPlayerCoords(ppl7::grafix::Point(player->x, player->y));
	size_t total_sprites=level.countSprites();
	size_t visible_sprites=level.countVisibleSprites();
	size_t total_objects=level.objects->count();
	size_t visible_objects=level.objects->countVisible();
	size_t total_particles=level.particles->count();
	size_t visible_particles=level.particles->countVisible();
	size_t total_lights=level.countLights();
	size_t visible_lights=level.countVisibleLights();

	if (player) statusbar->setPlayerState(player->getState());
	world_widget->updatePlayerStats(player);
	if (selected_object) {
		statusbar->setSelectedObject(selected_object->id);
	} else {
		statusbar->setSelectedObject(-1);
	}

	metrics.fps+=fps.getFPS();
	metrics.total_sprites+=total_sprites;
	metrics.visible_sprites+=visible_sprites;
	metrics.total_objects+=total_objects;
	metrics.visible_objects+=visible_objects;
	metrics.total_particles+=total_particles;
	metrics.visible_particles+=visible_particles;
	metrics.total_lights+=total_lights;
	metrics.visible_lights+=visible_lights;
}

void Game::updateWorldCoords()
{
	if (!player) return;
	int mx=game_viewport.width() / 2;
	int my=game_viewport.height() / 2 + 192;	//256
	WorldCoords.x=player->x - mx;
	WorldCoords.y=player->y - my;
	if (WorldCoords.x < 0) WorldCoords.x=0;
	if (WorldCoords.y < 0) WorldCoords.y=0;
}

ppl7::grafix::Point Game::getViewPos() const
{
	ppl7::grafix::Point p=WorldCoords;
	p.x+=game_viewport.width() / 2;
	p.y+=game_viewport.height() / 2;
	return p;
}

void Game::drawWorld(SDL_Renderer* renderer)
{
	metrics.time_draw_world.start();
	double now=ppl7::GetMicrotime();
	frame_rate_compensation=1.0f;
	if (last_frame_time > 0.0f) {

		float frametime=now - last_frame_time;
		frame_rate_compensation=frametime / (1.0f / 60.0f);
		if (frame_rate_compensation > 2.0f) frame_rate_compensation=2.0f;
		metrics.frame_rate_compensation+=frame_rate_compensation;
		metrics.frametime+=frametime;
		//ppl7::PrintDebugTime("Updated Frametime to: %0.3f, fpscomp=%0.3f\n", frametime, frame_rate_compensation);
		// 30Hz, frametime=0.033333
		// 90Hz, frametime=0.011111
		// 60Hz, frametime=0.016667
	}
	last_frame_time=now;


	level.setEditmode(object_selection != NULL);
	metrics.time_update_lights.start();
	level.updateVisibleLightsLists(WorldCoords, game_viewport);
	level.lights.update(now, frame_rate_compensation);
	metrics.time_update_lights.stop();

	metrics.time_update_sprites.start();
	level.updateVisibleSpriteLists(WorldCoords, game_viewport);	// => TODO: own Thread
	metrics.time_update_sprites.stop();
	metrics.time_update_objects.start();
	player->setGodMode(mainmenue->godModeEnabled());
	player->WorldCoords=WorldCoords;
	player->Viewport=game_viewport;
	//printf("viewport: x1=%d, y1=%d, x2=%d, y2=%d\n", game_viewport.x1, game_viewport.y1, game_viewport.x2, game_viewport.y2);
	level.objects->updateVisibleObjectList(WorldCoords, game_viewport);
	if (this->controlsEnabled || player->isAutoWalk())
		player->update(now, level.TileTypeMatrix, level.objects, frame_rate_compensation);

	if (game_speed == GameSpeed::Normal || game_speed == GameSpeed::ManualStep) {
		level.objects->update(now, level.TileTypeMatrix, *player, frame_rate_compensation);
	}

	ppl7::tk::MouseState mouse=wm->getMouseState();
	if (mainmenue->worldFollowsPlayer())
		updateWorldCoords();
	metrics.time_update_objects.stop();

	// Particles
	metrics.time_update_particles.start();
	metrics.time_particle_thread.addDuration(level.particles->waitForUpdateThreadFinished());
	//level.particles->cleanupParticles(now);
	if (game_speed == GameSpeed::Normal || game_speed == GameSpeed::ManualStep) {
		level.particles->update(now, level.TileTypeMatrix, *player, WorldCoords, game_viewport, frame_rate_compensation);
	}
	metrics.time_update_particles.stop();


	metrics.time_misc.start();
	// TODO: Refactor into Events: Handle Mouse events inside World
	if (mouse.p.inside(game_viewport)) {
		moveWorldOnMouseClick(mouse);
	}

	level.setViewport(game_viewport);
	metrics.time_misc.stop();


	// Draw background
	metrics.time_draw_background.start();
	// For unknown reason, the SDL_RenderClear on the screen is extreme expensive on linux with intel grafix card, it takes up to 12 ms!
	//SDL_SetRenderTarget(renderer, NULL);
	//SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	//SDL_RenderFillRect(renderer, NULL);


	/*
	SDL_SetRenderTarget(renderer, tex_render_layer);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);
	SDL_SetRenderTarget(renderer, tex_render_target);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);

	SDL_SetRenderTarget(renderer, NULL);
	*/
	updateLevelModificator(now);
	SDL_SetRenderTarget(renderer, tex_render_target);
	background.setColor(level.runtimeParams.BackgroundColor);
	background.setImage(level.runtimeParams.BackgroundImage);
	background.setBackgroundType(level.runtimeParams.backgroundType);
	background.draw(renderer, game_viewport, WorldCoords);
	metrics.time_draw_background.stop();
	if (screenshot) screenshot->save(Screenshot::Layer::Background, Screenshot::Type::Final, tex_render_target);

	// Draw Planes and Sprites
	metrics.time_draw_tsop.start();
	level.FarPlane.setVisible(mainmenue->visibility_plane_far);
	level.PlayerPlane.setVisible(mainmenue->visibility_plane_player);
	level.FrontPlane.setVisible(mainmenue->visibility_plane_front);
	level.BackPlane.setVisible(mainmenue->visibility_plane_back);
	level.MiddlePlane.setVisible(mainmenue->visibility_plane_middle);
	level.HorizonPlane.setVisible(mainmenue->visibility_plane_horizon);
	level.NearPlane.setVisible(mainmenue->visibility_plane_near);

	level.lights.setVisible(LightPlaneId::Horizon, mainmenue->visibility_plane_horizon);
	level.lights.setVisible(LightPlaneId::Far, mainmenue->visibility_plane_far);
	level.lights.setVisible(LightPlaneId::Middle, mainmenue->visibility_plane_middle);
	level.lights.setVisible(LightPlaneId::Player, mainmenue->visibility_plane_player);
	level.lights.setVisible(LightPlaneId::Near, mainmenue->visibility_plane_near);

	level.setEnableLights(mainmenue->visibility_lighting);
	level.setShowSprites(mainmenue->visibility_sprites);
	level.setShowObjects(mainmenue->visibility_objects);
	level.setShowParticles(mainmenue->visibility_particles);
	level.draw(renderer, WorldCoords, player, metrics);
	metrics.time_draw_tsop.stop();
	if (screenshot) {
		delete(screenshot);
		screenshot=NULL;
	}

	metrics.time_misc.start();
	if (player->isDead() == true && death_state == 0) {
		death_state=1;
		fade_to_black=0;
		if (player->lifes <= 1)soundtrack.fadeout(1.0f);
	}
	if (gameState == GameState::Running) {
		if (death_state) handleDeath(renderer, frame_rate_compensation);
		else if (fade_to_black > 0) {
			fade_to_black-=(5.0f * frame_rate_compensation);
			if (fade_to_black < 0.0f) fade_to_black=0.0f;
		}
	} else if (gameState == GameState::LevelEndTriggerd || gameState == GameState::GameOver || gameState == GameState::BackToMenue || gameState == GameState::QuitGame) {
		if (fade_to_black < 255) {
			fade_to_black+=(5.0f * frame_rate_compensation);
			if (fade_to_black > 255.0f) fade_to_black=255.0f;
		} else {
			if (gameState == GameState::LevelEndTriggerd) {
				if (LevelFile != "level/start.lvl") {
					gameState=GameState::ShowStats;
					showStatsScreen(StatsScreenReason::LevelEnd);
				}
				gameState=GameState::StartNextLevel;
			} else if (gameState == GameState::GameOver) {
				if (LevelFile != "level/start.lvl") {
					gameState=GameState::ShowStats;
					showStatsScreen(StatsScreenReason::PlayerDied);
				}
				player->resetState();
				world_widget->resetPlayerStats(player);
				startLevel(LevelFile);
			} else if (gameState == GameState::BackToMenue || gameState == GameState::QuitGame) {
				quitGame=true;
			}
		}
	}

	/*
	SDL_SetRenderTarget(renderer, tex_render_lightmap);
	SDL_SetRenderDrawColor(renderer, 64, 64, 64, 255);
	SDL_RenderClear(renderer);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_Rect rr;
	rr.x=1920 / 2 - 100;rr.y=1080 / 2 - 0;
	rr.w=200;rr.h=200;
	SDL_RenderFillRect(renderer, &rr);

	SDL_SetRenderTarget(renderer, tex_render_layer);
	SDL_RenderCopy(renderer, tex_render_lightmap, NULL, NULL);


	SDL_SetRenderTarget(renderer, tex_render_target);
	SDL_RenderCopy(renderer, tex_render_layer, NULL, NULL);
	*/

	metrics.time_misc.stop();
	metrics.time_draw_world.stop();
	if (game_speed == GameSpeed::ManualStep) {
		game_speed = GameSpeed::Paused;
	}
}

void Game::run()
{
	resizeEvent(NULL);
	last_frame_time=0.0f;
	death_state=0;
	fade_to_black=255;
	world_widget->setVisible(true);
	world_widget->setEnabled(true);
	wm->setKeyboardFocus(world_widget);
	wm->setGameControllerFocus(this);
	SDL_Renderer* renderer=sdl.getRenderer();
	quitGame=false;
	Metrics last_metrics, total_metrics;
	metrics.clear();
	ppl7::ppl_time_t last_second=ppl7::GetTime();
	ppl7::ppl_time_t start_total_metrics=last_second + 5;
	ppl7::ppl_time_t end_total_metrics=start_total_metrics + 60;
	while (!quitGame) {
		ppl7::ppl_time_t current_second=ppl7::GetTime();
		if (current_second > last_second) {
			last_second=current_second;
			//metrics.print();
			if (current_second >= start_total_metrics) {
				total_metrics+=metrics;
				if (current_second >= end_total_metrics) {
					//total_metrics.print();
					total_metrics.clear();
					end_total_metrics=current_second + 60;
				}
			}
			last_metrics=metrics.getAverage();
			//last_metrics.print();
			mainmenue->updateMetrics(last_metrics);
			metrics.clear();
		}
		metrics.newFrame();
		metrics.time_frame.start();
		metrics.time_total.start();
		metrics.time_events.start();
		checkSoundtrack();
		wm->handleEvents();
		ppl7::tk::MouseState mouse=wm->getMouseState();
		if (filedialog) checkFileDialog();
		metrics.time_events.stop();



		drawWorld(renderer);

		metrics.time_draw_ui.start();
		updateUi(mouse, last_metrics);
		drawSelectedSprite(renderer, mouse.p);
		drawSelectedObject(renderer, mouse.p);
		drawSelectedTile(renderer, mouse.p);
		if (lights_selection != NULL) {
			int plane=mainmenue->currentPlane();
			if (plane == static_cast<int>(PlaneId::Back) || plane == static_cast<int>(PlaneId::Front)) plane=static_cast<int>(PlaneId::Player);
			level.lights.drawEditMode(renderer, game_viewport, WorldCoords * planeFactor[mainmenue->currentPlane()], static_cast<LightPlaneId>(plane));
			//LightLayer& ls=level.lightsystem(mainmenue->currentPlane());
			//ls.drawObjects(renderer, game_viewport, WorldCoords * planeFactor[mainmenue->currentPlane()]);
			drawSelectedLight(renderer, mouse.p);
		}


		if (mainmenue->visibility_plane_player) {
			if (mainmenue->visibility_tiletypes) level.TileTypeMatrix.draw(renderer, game_viewport, WorldCoords);
			if (mainmenue->visibility_collision) player->drawCollision(renderer, game_viewport, WorldCoords);
			if (waynet_edit) level.waynet.draw(renderer, game_viewport, WorldCoords);
		}
		// Grid
		if (mainmenue->visibility_grid) drawGrid();
		if (message_overlay.hasMessage()) message_overlay.draw(renderer, game_viewport);
		// Widgets
		SDL_SetRenderTarget(renderer, NULL);
		drawRenderTargetToScreen();

		drawWidgets();
		// Mouse
		resources.Cursor.draw(renderer, mouse.p.x, mouse.p.y, 1);
		if (fade_to_black) FadeToBlack(renderer, (int)fade_to_black);

		metrics.time_draw_ui.stop();
		AudioSystem::Metrics audio_metrics=audiosystem.getMetrics();
		metrics.time_audioengine.addDuration(audio_metrics.time);
		metrics.total_audiotracks+=audio_metrics.tracks_total;
		metrics.hearable_audiotracks+=audio_metrics.tracks_played;
		metrics.time_total.stop();


		presentScreen();
		metrics.time_frame.stop();

		if (gameState == GameState::StartNextLevel) {
			player->resetLevelObjects();
			if (nextLevelFile == "MENU") break;
			if (!ppl7::File::exists(nextLevelFile)) {
				break;
			}
			startLevel(nextLevelFile);
		}
	}
	soundtrack.fadeout(4.0f);
}

void Game::quitEvent(ppl7::tk::Event* e)
{
	quitGame=true;
}

void Game::closeEvent(ppl7::tk::Event* e)
{
	ppl7::tk::Widget* widget=e->widget();
	if (widget != NULL && widget == settings_screen) {
		delete settings_screen;
		settings_screen=NULL;
		enableControls(true);
		wm->setKeyboardFocus(world_widget);
		wm->setGameControllerFocus(this);
		if (world_widget) world_widget->retranslateUi();
		if (e->customId() == 1) {
			gameState = GameState::BackToMenue;
			fade_to_black=0.0f;
			//quitGame=true;
		} else if (e->customId() == 2) {
			gameState = GameState::QuitGame;
			fade_to_black=0.0f;
			//quitGame=true;
		}
		return;
	}
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
		game_viewport.setMenuOffset(0);
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
		game_viewport.setMenuOffset(0);
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
		game_viewport.setMenuOffset(0);
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
		game_viewport.setMenuOffset(0);
		world_widget->setViewport(viewport);
	}
}

void Game::closeLightsSelection()
{
	if (lights_selection) {
		this->removeChild(lights_selection);
		delete(lights_selection);
		lights_selection=NULL;
		viewport.x1=0;
		game_viewport.setMenuOffset(0);
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
		game_viewport.setMenuOffset(0);
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
	closeLightsSelection();
	if (tiles_selection) {
		closeTileSelection();
	} else {
		tiles_selection=new Decker::ui::TilesSelection(0, 32, 300, statusbar->y() - 32, this);
		for (int i=1;i <= resources.getMaxTilesetId();i++) {
			tiles_selection->setTileSet(i, resources.bricks[i].name, &resources.bricks[i].ui);
		}
		tiles_selection->setCurrentTileSet(remember.lastTileset);
		tiles_selection->setSelectedTile(remember.lastTile);
		tiles_selection->setColorIndex(remember.lastTileColor);
		tiles_selection->setLayer(remember.lastTileLayer);
		this->addChild(tiles_selection);
		viewport.x1=300;
		game_viewport.setMenuOffset(300);
		world_widget->setViewport(viewport);
	}
}

void Game::showTileTypeSelection()
{
	closeSpriteSelection();
	closeTileSelection();
	closeObjectSelection();
	closeWayNet();
	closeLightsSelection();
	if (tiletype_selection) {
		closeTileTypeSelection();
		mainmenue->setShowTileTypes(false);
	} else {
		tiletype_selection=new Decker::ui::TileTypeSelection(0, 32, 300, statusbar->y() - 32, this, &resources.TileTypes);
		this->addChild(tiletype_selection);
		viewport.x1=300;
		world_widget->setViewport(viewport);
		game_viewport.setMenuOffset(300);
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
	closeLightsSelection();
	if (sprite_selection) {
		closeSpriteSelection();
	} else {
		sprite_selection=new Decker::ui::SpriteSelection(0, 32, 300, statusbar->y() - 32, this);
		sprite_selection->setSpriteSet(1, "Nature", &resources.uiSpritesNature, 4);
		sprite_selection->setSpriteSet(2, "Plants", &resources.uiSpritesPlants);
		sprite_selection->setSpriteSet(3, "Objects", &resources.uiSpritesObjects, 1);
		sprite_selection->setSpriteSet(4, "Nature Blury", &resources.uiSpritesNature, 1);
		sprite_selection->setSpriteSet(5, "Rocks", &resources.uiSpritesRocks, 4);
		sprite_selection->setSpriteSet(6, "Flowers", &resources.uiSpritesFlowers, 1);
		sprite_selection->setSpriteSet(7, "Treasure", &resources.uiSpritesTreasure, 1);
		sprite_selection->setSpriteSet(8, "Recolorable 8d", &resources.uiSpritesWhite, 8);
		sprite_selection->setSpriteSet(10, "Recolorable 1d", &resources.uiSprites_White1D, 1);
		sprite_selection->setSpriteSet(9, "Tropical", &resources.uiSpritesTropical, 4);
		this->addChild(sprite_selection);
		viewport.x1=300;
		game_viewport.setMenuOffset(300);
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
	closeLightsSelection();
	if (object_selection) {
		closeObjectSelection();
	} else {
		object_selection=new Decker::ui::ObjectSelection(0, 32, 300, statusbar->y() - 32, this);
		object_selection->setSpriteSet(&resources.uiObjects);
		this->addChild(object_selection);
		viewport.x1=300;
		game_viewport.setMenuOffset(300);
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
	closeLightsSelection();
	if (waynet_edit) {
		closeWayNet();
		mainmenue->setShowTileTypes(false);
	} else {
		waynet_edit=new Decker::ui::WayNetEdit(0, 32, 300, statusbar->y() - 2 - 32, this);
		//waynet_edit->setSpriteSet(&resources.uiObjects);
		this->addChild(waynet_edit);
		viewport.x1=300;
		game_viewport.setMenuOffset(300);
		world_widget->setViewport(viewport);
		mainmenue->setShowTileTypes(true);
	}
}

void Game::showLightsSelection()
{
	closeTileTypeSelection();
	closeTileSelection();
	closeSpriteSelection();
	closeObjectSelection();
	closeWayNet();
	if (lights_selection) {
		closeLightsSelection();
	} else {
		lights_selection=new Decker::ui::LightSelection(0, 32, 300, statusbar->y() - 2 - 32, this);
		lights_selection->setSpriteSet(&resources.uiLightmaps);
		this->addChild(lights_selection);
		viewport.x1=300;
		game_viewport.setMenuOffset(300);
		world_widget->setViewport(viewport);
		sprite_mode=spriteModeDraw;
		selected_light=NULL;
		lights_selection->setLightId(0);
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
		selected_sprite_system->drawSelectedSpriteOutline(renderer, game_viewport,
			WorldCoords * planeFactor[currentPlane], selected_sprite.id);
	} else if (sprite_mode == spriteModeDraw) {
		if (!mouse.inside(game_viewport)) return;
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
		ppl7::grafix::Point tmouse=game_viewport.translate(mouse);
		float scale=sprite_selection->spriteScale();
		float rotation=sprite_selection->spriteRotation();
		if (!level.spriteset[spriteset]) return;
		level.spriteset[spriteset]->drawScaledWithAngle(renderer,
			tmouse.x, tmouse.y, nr, scale, scale, rotation, level.palette.getColor(sprite_selection->colorIndex()));
		level.spriteset[spriteset]->drawOutlinesWithAngle(renderer,
			tmouse.x, tmouse.y, nr, scale, scale, rotation);
	}
}

void Game::drawSelectedLight(SDL_Renderer* renderer, const ppl7::grafix::Point& mouse)
{
	if (!lights_selection) return;
	if (sprite_mode == SpriteModeEdit && selected_light != NULL) {
		updateLightFromUi();
		int currentPlane=mainmenue->currentPlane();
		level.lights.drawSelectedLight(renderer, game_viewport,
			WorldCoords * planeFactor[currentPlane], selected_light->id);
		return;
	} else if (sprite_mode == spriteModeDraw) {
		if (!mouse.inside(game_viewport)) return;
		int nr=lights_selection->selectedLight();
		if (nr < 0) return;
		ppl7::grafix::Point tmouse=game_viewport.translate(mouse);
		float scale_x=lights_selection->lightScaleX();
		float scale_y=lights_selection->lightScaleY();
		float angle=lights_selection->lightAngle();
		ppl7::grafix::Color c=lights_selection->color();
		c.setAlpha(lights_selection->colorIntensity());
		resources.Lightmaps.drawScaledWithAngle(renderer,
			tmouse.x, tmouse.y, nr, scale_x, scale_y, angle, c);
		resources.Lightmaps.drawOutlinesWithAngle(renderer,
			tmouse.x, tmouse.y, nr, scale_x, scale_y, angle);
		return;
	}
	if (lights_selection->selectedLight() >= 0 && sprite_mode != spriteModeDraw) {
		selected_light=NULL;
		lights_selection->setLightId(0);
		sprite_mode=spriteModeDraw;
	}
}


void Game::drawSelectedTile(SDL_Renderer* renderer, const ppl7::grafix::Point& mouse)
{
	if (!tiles_selection) return;
	if (!mouse.inside(game_viewport)) return;
	int currentPlane=mainmenue->currentPlane();
	int currentLayer=tiles_selection->currentLayer();
	int nr=tiles_selection->selectedTile();
	int tileset=tiles_selection->currentTileSet();
	int color_index=tiles_selection->colorIndex();
	if (nr < 0 || tileset<0 || tileset>MAX_TILESETS) return;
	if (!level.tileset[tileset]) return;
	ppl7::grafix::Point tmouse=game_viewport.translate(mouse);
	//ppl7::grafix::Point wp=tmouse - game_viewport.topLeft() + WorldCoords * planeFactor[currentPlane];
	ppl7::grafix::Point wp=tmouse + WorldCoords * planeFactor[currentPlane];
	int tx=wp.x / TILE_WIDTH;
	int ty=wp.y / TILE_HEIGHT;
	BrickOccupation::Matrix occupation=brick_occupation.get(nr);
	if (tileset == 1) occupation=brick_occupation_solid;
	if (!level.plane(currentPlane).isOccupied(tx, ty, currentLayer, occupation)) {
		int x=tx * TILE_WIDTH + game_viewport.x1 - WorldCoords.x * planeFactor[currentPlane];;
		int y=ty * TILE_HEIGHT + game_viewport.y1 - WorldCoords.y * planeFactor[currentPlane];;
		level.tileset[tileset]->draw(renderer,
			x, y + TILE_HEIGHT, nr, level.palette.getColor(color_index));
	}

}

void Game::drawSelectedObject(SDL_Renderer* renderer, const ppl7::grafix::Point& mouse)
{
	if (!object_selection) return;
	if (sprite_mode == SpriteModeEdit && selected_object != NULL) {
		level.objects->drawSelectedSpriteOutline(renderer, game_viewport,
			WorldCoords, selected_object->id);
	} else if (sprite_mode == spriteModeDraw) {
		if (!mouse.inside(game_viewport)) return;
		int object_type=object_selection->selectedObjectType();
		if (object_type < 0) return;
		ppl7::grafix::Point tmouse=game_viewport.translate(mouse);
		level.objects->drawPlaceSelection(renderer, tmouse, object_type);
	}
}

void Game::unloadLevel()
{
	soundtrack.fadeout(2.0f);
	closeTileTypeSelection();
	closeTileSelection();
	closeSpriteSelection();
	closeLightsSelection();
	closeObjectSelection();
	closeWayNet();
	remember.clear();
	LevelFile.clear();
	level.clear();
	background.clear();
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
	background.clear();
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
	player->resetLevelObjects();
	player->stand();
	soundtrack.playInitialSong();
	background.setBackgroundType(level.params.backgroundType);
	background.setColor(level.params.BackgroundColor);
	background.setImage(level.params.BackgroundImage);
	background.setLevelDimension(level.getOccupiedAreaFromTileTypePlane());
	gameState=GameState::Running;
	last_frame_time=0.0f;
	if (filename == "level/start.lvl") {
		player->setAutoWalk(true);
		enableControls(false);

	} else {
		player->setAutoWalk(false);
	}

}

void Game::checkSoundtrack()
{
	if (player->isDead()) return;
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
	try {
		level.backup(filename);
		level.save(filename);
		LevelFile=filename;
	} catch (...) {
		ppl7::String levelname=ppl7::File::getFilename(filename);
		LevelFile=config.CustomLevelPath + "/" + levelname;
#ifdef WIN32
		LevelFile.replace("/", "\\");
		openSaveAsDialog();
		return;
#endif
	}
	if (mainmenue) mainmenue->update();
}

void Game::load()
{
	last_frame_time=0.0f;
	closeTileTypeSelection();
	closeTileSelection();
	closeSpriteSelection();
	closeObjectSelection();
	closeWayNet();
	message_overlay.clear();
	selected_object=NULL;
	remember.clear();
	level.load(LevelFile);
	enableControls(true);
	if (mainmenue) mainmenue->update();
}

void Game::createNewLevel(const LevelParameter& params)
{
	last_frame_time=0.0f;
	closeTileTypeSelection();
	closeTileSelection();
	closeSpriteSelection();
	closeObjectSelection();
	closeWayNet();
	selected_object=NULL;
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
#ifdef EVENTTRACKING
	ppl7::PrintDebugTime("Game::mouseDownEvent\n");
#endif
	if (event->widget() == world_widget) {
		wm->setKeyboardFocus(world_widget);
		game_viewport.translateMouseEvent(event);
		if (sprite_selection != NULL) {
			mouseDownEventOnSprite(event);
		} else if (object_selection != NULL) {
			mouseDownEventOnObject(event);
		} else if ((tiles_selection != NULL || tiletype_selection != NULL)) {
			handleMouseDrawInWorld(*event);
		} else if (waynet_edit != NULL) {
			mouseDownEventOnWayNet(event);
		} else if (lights_selection != NULL) {
			mouseDownEventOnLight(event);

		}
	}
}


void Game::mouseDownEventOnSprite(ppl7::tk::MouseEvent* event)
{
#ifdef EVENTTRACKING
	ppl7::PrintDebugTime("Game::mouseDownEventOnSprite\n");
#endif

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
		float rotation=sprite_selection->spriteRotation();
		int layer=sprite_selection->currentLayer();
		if (layer < 0 || layer>1 || spriteset > MAX_SPRITESETS) return;
		if (!level.spriteset[spriteset]) return;
		int currentPlane=mainmenue->currentPlane();
		SpriteSystem& ss=level.spritesystem(currentPlane, layer);
		ppl7::grafix::Point coords=WorldCoords * planeFactor[currentPlane];
		ss.addSprite(event->p.x + coords.x,
			event->p.y + coords.y,
			0,
			spriteset, nr, scale, rotation, sprite_selection->colorIndex());
	} else if (event->widget() == world_widget && event->buttonMask == ppl7::tk::MouseState::Right) {
		sprite_selection->setSelectedSprite(-1);
		sprite_mode=spriteModeDraw;
		selected_sprite.id=-1;
		selected_sprite_system=NULL;
	}
}

void Game::mouseDownEventOnLight(ppl7::tk::MouseEvent* event)
{
#ifdef EVENTTRACKING
	ppl7::PrintDebugTime("Game::mouseDownEventOnSprite\n");
#endif

	if (event->widget() == world_widget && event->buttonMask == ppl7::tk::MouseState::Left) {
		int nr=lights_selection->selectedLight();
		if (nr < 0) {
			selectLight(event->p);
			return;
		}
		if (sprite_mode != spriteModeDraw) return;
		int currentPlane=mainmenue->currentPlane();
		if (currentPlane == static_cast<int>(PlaneId::Back) || currentPlane == static_cast<int>(PlaneId::Front)) currentPlane=static_cast<int>(PlaneId::Player);
		ppl7::grafix::Point coords=WorldCoords * planeFactor[currentPlane];
		LightObject* light=new LightObject();
		light->plane=currentPlane;
		light->scale_x=lights_selection->lightScaleX();
		light->scale_y=lights_selection->lightScaleY();
		light->angle=lights_selection->lightAngle();
		light->x=event->p.x + coords.x;
		light->y=event->p.y + coords.y;
		light->sprite_no=nr;
		light->color=lights_selection->color();
		light->flarePlane=lights_selection->getLensFlarePlane();
		light->intensity=lights_selection->colorIntensity();
		light->playerPlane=static_cast<uint8_t>(lights_selection->getPlayerPlaneMatrix());
		light->enabled=lights_selection->getCurrentState();
		light->initial_state=lights_selection->getInitialState();
		light->has_lensflare=lights_selection->getLensflare();
		light->flare_useLightColor=lights_selection->getFlareUseLightColor();
		light->flare_intensity=lights_selection->getLensflareIntensity();
		light->myType=lights_selection->getLightType();
		light->typeParameter=lights_selection->lightTypeParameter();

		level.lights.addLight(light);
	} else if (event->widget() == world_widget && event->buttonMask == ppl7::tk::MouseState::Right) {
		lights_selection->setSelectedLight(-1);
		sprite_mode=spriteModeDraw;
		selected_light=NULL;
		lights_selection->setLightId(0);
	}
}


void Game::mouseDownEventOnObject(ppl7::tk::MouseEvent* event)
{
#ifdef EVENTTRACKING
	ppl7::PrintDebugTime("Game::mouseDownEventOnObject\n");
#endif

	if (event->widget() == world_widget && (event->buttonMask == ppl7::tk::MouseState::Middle
		|| (event->buttonMask == ppl7::tk::MouseState::Left && SDL_GetKeyboardState(NULL)[SDL_SCANCODE_LSHIFT]))) {
		Decker::Objects::Object* object=level.objects->findMatchingObject(event->p + WorldCoords);
		if (object) {
			wm->setKeyboardFocus(world_widget);
			object_selection->setObjectType(object->type());
			object_selection->setObjectDifficulty(object->difficulty_matrix);
			object_selection->setLayer(static_cast<int>(object->myLayer));
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
				object_selection->setObjectType(object->type());
				object_selection->setObjectDifficulty(object->difficulty_matrix);
				object_selection->setLayer(static_cast<int>(object->myLayer));
			}
			return;
		}
		if (sprite_mode != spriteModeDraw) return;
		selected_object=level.objects->getInstance(object_type);
		if (selected_object) {
			ppl7::grafix::Point coords=WorldCoords;
			selected_object->difficulty_matrix=object_selection->getDifficulty();
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
#ifdef EVENTTRACKING
	ppl7::PrintDebugTime("Game::mouseDownEventOnWayNet\n");
#endif
	ppl7::grafix::Point coords=WorldCoords;
	int x=(event->p.x + coords.x);
	int y=(event->p.y + coords.y);
	WayPoint wp(x, y);
	if (waynet_edit->debugMode()) {
		if (event->buttonMask == ppl7::tk::MouseState::Left) {
			waynet_edit->setDebugStart(wp);
			level.waynet.setDebugStart(wp);
		} if (event->buttonMask == ppl7::tk::MouseState::Right) {
			waynet_edit->setDebugEnd(wp);
			level.waynet.setDebugEnd(wp);
		}
	} else {
		if (event->buttonMask == ppl7::tk::MouseState::Left) {
			const WayPoint& found_wp=level.waynet.findPoint(wp);
			if (found_wp != level.waynet.invalidPoint()) {
				//printf ("Point selected\n");
				if (level.waynet.hasSelection()) {
					const WayPoint p1=level.waynet.getSelection();
					level.waynet.deleteConnection(p1, found_wp);
					Connection::ConnectionType type=(Connection::ConnectionType)waynet_edit->getSelectedWayType();
					if (type != Connection::Invalid) {
						level.waynet.addConnection(p1,
							Connection(p1, found_wp, type,
								waynet_edit->getCost()));
					}
					level.waynet.setSelection(found_wp);
				} else {
					level.waynet.setSelection(found_wp);
				}
			} else {
				const WayPoint& nearest=level.waynet.findNearestWaypoint(wp);
				if (Distance(wp, nearest) >= 2 * TILE_WIDTH) {
					level.waynet.addPoint(wp);
					level.waynet.clearSelection();
				}
			}
		} else if (event->buttonMask == ppl7::tk::MouseState::Right) {
			const WayPoint& found_wp=level.waynet.findPoint(wp);
			if (found_wp != level.waynet.invalidPoint()) {
				level.waynet.deletePoint(found_wp);
			}
			level.waynet.clearSelection();
		}
	}
}

void Game::mouseWheelEvent(ppl7::tk::MouseEvent* event)
{
	if (sprite_selection != NULL && event->widget() == world_widget) {
		const Uint8* state = SDL_GetKeyboardState(NULL);
		if (state[SDL_SCANCODE_LSHIFT]) {
			if (sprite_mode == spriteModeDraw) {
				float angle=sprite_selection->spriteRotation();
				if (event->wheel.y < 0) angle-=5;
				if (event->wheel.y > 0) angle+=5;
				if (angle <= 0) angle+=360;
				if (angle >= 360) angle-=360;
				sprite_selection->setSpriteRotation(angle);
			} else if (sprite_mode == SpriteModeEdit && selected_sprite.id >= 0 && selected_sprite_system != NULL) {
				if (event->wheel.y < 0) selected_sprite.rotation-=5;
				if (event->wheel.y > 0) selected_sprite.rotation+=5;
				if (selected_sprite.rotation <= 0) selected_sprite.rotation+=360;
				if (selected_sprite.rotation >= 360) selected_sprite.rotation-=360;
				//selected_light_system->modifyLight(selected_light);
				selected_sprite_system->modifySprite(selected_sprite);
			}

		} else {
			if (sprite_mode == spriteModeDraw) {
				float scale=sprite_selection->spriteScale();
				if (event->wheel.y < 0 && scale>0.1) scale-=0.05;
				else if (event->wheel.y > 0 && scale < 2.0) scale+=0.05;
				sprite_selection->setSpriteScale(scale);
			} else if (sprite_mode == SpriteModeEdit && selected_sprite.id >= 0 && selected_sprite_system != NULL) {
				//printf ("wheel\n");
				if (event->wheel.y < 0 && selected_sprite.scale>0.1) selected_sprite.scale-=0.05;
				else if (event->wheel.y > 0 && selected_sprite.scale < 2.0) selected_sprite.scale+=0.05;
				selected_sprite_system->modifySprite(selected_sprite);
			}
		}
	} else if (lights_selection != NULL && event->widget() == world_widget) {
		const Uint8* state = SDL_GetKeyboardState(NULL);
		if (state[SDL_SCANCODE_LSHIFT]) {
			if (sprite_mode == spriteModeDraw) {
				float angle=lights_selection->lightAngle();
				if (event->wheel.y < 0) angle-=5;
				if (event->wheel.y > 0) angle+=5;
				if (angle <= 0) angle+=360;
				if (angle >= 360) angle-=360;
				lights_selection->setLightAngle(angle);
			} else if (sprite_mode == SpriteModeEdit && selected_light != NULL) {
				if (event->wheel.y < 0) selected_light->angle-=5;
				if (event->wheel.y > 0) selected_light->angle+=5;
				if (selected_light->angle <= 0) selected_light->angle+=360;
				if (selected_light->angle >= 360) selected_light->angle-=360;
				//selected_light_system->modifyLight(selected_light);
				lights_selection->setLightAngle(selected_light->angle);
			}

		} else {
			if (sprite_mode == spriteModeDraw) {
				float scale_x=lights_selection->lightScaleX();
				float scale_y=lights_selection->lightScaleY();
				if (event->wheel.y < 0 && scale_x>0.01) scale_x-=0.05;
				if (event->wheel.y < 0 && scale_y>0.01) scale_y-=0.05;
				if (event->wheel.y > 0 && scale_x < 5.0) scale_x+=0.05;
				if (event->wheel.y > 0 && scale_y < 5.0) scale_y+=0.05;
				lights_selection->setLightScaleX(scale_x);
				lights_selection->setLightScaleY(scale_y);
			} else if (sprite_mode == SpriteModeEdit && selected_light != NULL) {
				//printf ("wheel\n");
				if (event->wheel.y < 0 && selected_light->scale_x>0.1) selected_light->scale_x-=0.05;
				if (event->wheel.y < 0 && selected_light->scale_y>0.1) selected_light->scale_y-=0.05;
				if (event->wheel.y > 0 && selected_light->scale_x < 5.0) selected_light->scale_x+=0.05;
				if (event->wheel.y > 0 && selected_light->scale_y < 5.0) selected_light->scale_y+=0.05;
				//selected_light_system->modifyLight(selected_light);
				lights_selection->setLightScaleX(selected_light->scale_x);
				lights_selection->setLightScaleY(selected_light->scale_y);
			}
		}
	}
}

void Game::selectSprite(const ppl7::grafix::Point& mouse)
{
#ifdef EVENTTRACKING
	ppl7::PrintDebugTime("Game::selectSprite\n");
#endif
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

void Game::selectLight(const ppl7::grafix::Point& mouse)
{
#ifdef EVENTTRACKING
	ppl7::PrintDebugTime("Game::selectSprite\n");
#endif
	int plane=mainmenue->currentPlane();
	if (plane == static_cast<int>(PlaneId::Back) || plane == static_cast<int>(PlaneId::Front)) plane=static_cast<int>(PlaneId::Player);
	selected_light=level.lights.findMatchingLight(mouse + WorldCoords * planeFactor[plane], static_cast<LightPlaneId>(plane));


	if (selected_light) {
		lights_selection->setLightId(selected_light->id);
		lights_selection->setSelectedLight(selected_light->sprite_no);
		lights_selection->setLightAngle(selected_light->angle);
		lights_selection->setLightScaleX(selected_light->scale_x);
		lights_selection->setLightScaleY(selected_light->scale_y);
		lights_selection->setColorIntensity(selected_light->intensity);
		lights_selection->setColor(selected_light->color);
		lights_selection->setLensFlarePlane(selected_light->flarePlane);
		lights_selection->setCurrentState(selected_light->enabled);
		lights_selection->setInitialState(selected_light->initial_state);
		lights_selection->setPlayerPlaneMatrix(static_cast<LightPlayerPlaneMatrix>(selected_light->playerPlane));
		lights_selection->setLensflare(selected_light->has_lensflare);
		lights_selection->setLensflareIntensity(selected_light->flare_intensity);
		lights_selection->setFlareUseLightColor(selected_light->flare_useLightColor);
		lights_selection->setLightType(selected_light->myType);
		lights_selection->setLightTypeParameter(selected_light->typeParameter);

		wm->setKeyboardFocus(world_widget);
		sprite_mode=SpriteModeEdit;
		sprite_move_start=mouse;
	} else {
		selected_light=NULL;
		lights_selection->setLightId(0);
	}
}


void Game::keyDownEvent(ppl7::tk::KeyEvent* event)
{
	if (gameState == GameState::ShowStats) {
		if (game_stats_screen) {
			if (event->key == ppl7::tk::KeyEvent::KEY_ESCAPE || event->key == ppl7::tk::KeyEvent::KEY_SPACE) {
				game_stats_screen->signalContinue();
			}
		}
		return;
	}
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
		} else if (sprite_mode == SpriteModeEdit && lights_selection != NULL && selected_light != NULL) {
			if (event->key == ppl7::tk::KeyEvent::KEY_DELETE
				&& (event->modifier & ppl7::tk::KeyEvent::KEYMOD_MODIFIER) == 0) {
				level.lights.deleteLight(selected_light->id);
				selected_light=NULL;
				lights_selection->setLightId(0);
				lights_selection->setSelectedLight(-1);
			}
		}
	}
	//printf("keyDownEvent: %d, modifier: %04x\n", event->key, event->modifier);
	if (event->key == ppl7::tk::KeyEvent::KEY_F4 && showui == true) {
		ppl7::grafix::Point pos=level.objects->nextPlayerStart();
		player->move(pos.x, pos.y);
		player->stand();
		player->setSavePoint(pos);
	} else if (event->key == ppl7::tk::KeyEvent::KEY_F3 && showui == true) {
		level.load(LevelFile);
		background.clear();
		translator.load();
	} else if (event->key == ppl7::tk::KeyEvent::KEY_F5 && showui == true) {
		player->resetState();
	} else if (event->key == ppl7::tk::KeyEvent::KEY_F9) {
		showUi(!showui);
	} else if (event->key == ppl7::tk::KeyEvent::KEY_F10) {
		mainmenue->showMetrics();
		mainmenue->fitMetrics(viewport);

	} else if (event->key == ppl7::tk::KeyEvent::KEY_RETURN && (event->modifier & ppl7::tk::KeyEvent::KEYMOD_ALT) > 0) {
		//printf("toggle fullscreen or back\n");
		ppl7::tk::WindowManager_SDL2* sdl2wm=(ppl7::tk::WindowManager_SDL2*)wm;
		Window::WindowMode mode=sdl2wm->getWindowMode(*this);
		if (mode == Window::WindowMode::Window) {
			windowedSize.setSize(width(), height());
			//printf("Aktueller mode ist Window mit %d x %d\n", windowedSize.width, windowedSize.height);
			ppl7::grafix::Size s=sdl.getDisplaySize(config.videoDevice);
			//printf("switche zu FullscreenDesktop %d x %d\n", s.width, s.height);
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
			//printf("Aktueller mode ist FullscreenDesktop, switche zu Fenster %d x %d\n", windowedSize.width, windowedSize.height);
			sdl2wm->changeWindowMode(*this, Window::WindowMode::Window);
			ppl7::tk::Window::DisplayMode dmode;
			dmode.format=rgbFormat();
			dmode.width=windowedSize.width;
			dmode.height=windowedSize.height;
			dmode.refresh_rate=config.ScreenRefreshRate;
			setWindowDisplayMode(dmode);
			resizeEvent(NULL);
		} else {
			//printf("Aktueller mode ist Fullscreen\n");
		}
	} else if (event->key == ppl7::tk::KeyEvent::KEY_ESCAPE) {
		if (settings_screen) {
			delete settings_screen;
			settings_screen=NULL;
			enableControls(true);
			wm->setKeyboardFocus(world_widget);
		} else openSettingsScreen();
	} else if (event->key == ppl7::tk::KeyEvent::KEY_F8) {
		ppl7::PrintDebugTime("TakeScreenshot\n");
		TakeScreenshot();
	}
}

void Game::mouseMoveEvent(ppl7::tk::MouseEvent* event)
{
	if (sprite_selection != NULL) {
		if (event->widget() == world_widget && event->buttonMask == ppl7::tk::MouseState::Left
			&& sprite_mode == SpriteModeEdit && selected_sprite.id >= 0
			&& selected_sprite_system != NULL) {
			game_viewport.translateMouseEvent(event);
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
			game_viewport.translateMouseEvent(event);
			ppl7::grafix::Point diff=event->p - sprite_move_start;
			selected_object->initial_p.x+=diff.x;
			selected_object->initial_p.y+=diff.y;
			selected_object->p=selected_object->initial_p;
			selected_object->updateBoundary();
			sprite_move_start=event->p;
		}
	} else if (lights_selection != NULL) {
		if (event->widget() == world_widget && event->buttonMask == ppl7::tk::MouseState::Left
			&& sprite_mode == SpriteModeEdit && selected_light != NULL) {
			game_viewport.translateMouseEvent(event);
			ppl7::grafix::Point diff=event->p - sprite_move_start;
			selected_light->x+=diff.x;
			selected_light->y+=diff.y;
			//selected_light_system->modifyLight(selected_light);
			//printf("Move: %d, %d\n", diff.x, diff.y);
			sprite_move_start=event->p;

		}
	} else if ((tiles_selection != NULL || tiletype_selection != NULL) && event->widget() == world_widget) {
		game_viewport.translateMouseEvent(event);
		handleMouseDrawInWorld(*event);
	}

}

void Game::resizeEvent(ppl7::tk::ResizeEvent* event)
{
	/*
	if (tex_level_grid) {
		sdl.destroyTexture(tex_level_grid);
		tex_level_grid=NULL;
	}
	*/
	desktopSize=clientSize();
	viewport=clientRect();
	game_viewport.setRealViewport(clientSize());
	resizeMenueAndStatusbar();
	message_overlay.resize(desktopSize);
	showUi(showui);
	//printf("Game::resizeEvent, Window sagt: %d x %d\n", this->width(), this->height());
	//if (start_screen) start_screen->resizeEvent(event);
	if (settings_screen) settings_screen->resizeEvent(event);

}

void Game::handleDeath(SDL_Renderer* renderer, float frame_rate_compensation)
{
	if (death_state == 1) {
		if (fade_to_black < 255) {
			fade_to_black+=(5.0f * frame_rate_compensation);
			if (fade_to_black > 255) fade_to_black=255;
		} else death_state=2;
	} else if (death_state == 2) {
		player->dropLifeAndResetToLastSavePoint();
		death_state=3;
		if (player->lifes <= 0) {
			death_state = 0;
			gameState = GameState::GameOver;
		}
	} else if (death_state == 3) {
		if (fade_to_black > 0) {
			fade_to_black-=(5.0f * frame_rate_compensation);
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
	if (!ppl7::File::exists("Makefile")) {
		// we check if the save file is inside the installation path. We won't be able to write here
		if (LevelFile.startsWith(ppl7::Dir::currentPath()) || LevelFile.startsWith("level/")) {
			filedialog->setFilename(config.CustomLevelPath + "/" + ppl7::File::getFilename(LevelFile));
		}
	}
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
	filedialog->setFilter("*.lvl");
	if (LevelFile.notEmpty())	filedialog->setFilename(LevelFile);
	else {
		ppl7::String pwd=ppl7::Dir::currentPath() + "/level";
		if (ppl7::Dir::exists(pwd)) filedialog->setDirectory(pwd);
		else filedialog->setDirectory(config.CustomLevelPath);
	}

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
		LevelFile=filename;
		// Are we in dev environment?
		if (!ppl7::File::exists("Makefile")) {
			config.CustomLevelPath=ppl7::File::getPath(filename);
		}
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

void Game::openSettingsScreen()
{
	if (settings_screen) delete settings_screen;
	settings_screen=NULL;
	settings_screen=new SettingsScreen(*this,
		100, 100, 1720, 880, true);
	this->addChild(settings_screen);
	this->needsRedraw();
	enableControls(false);
}

bool Game::nextLevel(const ppl7::String& filename)
/*!Jump to next level
 * @param filename Filename of level to load
 * @return Returns true, if level exists and will be loaded, returns false, when level is not loadable
 */
{
	nextLevelFile.clear();
	if (filename == "MENU") {
		nextLevelFile="MENU";
		gameState=GameState::LevelEndTriggerd;
		fade_to_black=0;
		enableControls(false);
		return true;
	}
	if (ppl7::File::exists(filename)) {
		nextLevelFile=filename;
	} else if (ppl7::File::exists("level/" + filename)) {
		nextLevelFile="level/" + filename;
	}
	if (nextLevelFile.isEmpty()) return false;
	enableControls(false);
	gameState=GameState::LevelEndTriggerd;
	fade_to_black=0;
	//printf("wir sollten hierhin gehen: %s\n", (const char*)nextLevelFile);

	return true;
}


void Game::pauseGame(bool flag)
{
	if (flag) game_speed=GameSpeed::Paused;
	else game_speed=GameSpeed::Normal;
}

void Game::stepFrame()
{
	game_speed=GameSpeed::ManualStep;
}

Player* Game::getPlayer()
{
	return player;
}


void Game::gameControllerAxisMotionEvent(ppl7::tk::GameControllerAxisEvent* event)
{


}

void Game::gameControllerButtonDownEvent(ppl7::tk::GameControllerButtonEvent* event)
{
	GameControllerMapping::Button b=controller.mapping.getButton(event);
	//ppl7::PrintDebugTime("gameControllerButtonDownEvent b=%d\n", (int)b);
	if (b == GameControllerMapping::Button::Menu) {
		if (settings_screen) {
			delete settings_screen;
			settings_screen=NULL;
			enableControls(true);
			wm->setKeyboardFocus(world_widget);
			wm->setGameControllerFocus(this);
		} else openSettingsScreen();
	}

}

void Game::gameControllerButtonUpEvent(ppl7::tk::GameControllerButtonEvent* event)
{

}


void Game::gameControllerDeviceAdded(ppl7::tk::GameControllerEvent* event)
{
	//ppl7::PrintDebugTime("gameControllerDeviceAdded: %d\n", event->which);
	controller.open(event->which);
}

void Game::gameControllerDeviceRemoved(ppl7::tk::GameControllerEvent* event)
{
	//ppl7::PrintDebugTime("gameControllerDeviceRemoved: %d\n", event->which);
	controller.close();
}

void Game::updateDifficultyForSelectedObject(uint8_t dificulty)
{
	if (selected_object) selected_object->difficulty_matrix=dificulty;
}

void Game::updateLayerForSelectedObject(int layer)
{
	if (selected_object) {
		selected_object->myLayer=static_cast<Decker::Objects::Object::Layer>(layer);
		//ppl7::PrintDebugTime("Update Layer to: %d\n", layer);
	}
}


void Game::drawRenderTargetToScreen()
{
	SDL_Renderer* renderer=sdl.getRenderer();

	//ppl7::grafix::Rect vp=clientRect();

	game_viewport.setRealViewport(this->clientSize());
	// TODO: clear screen outside the viewport
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_Rect rr;
	rr.x=1920 / 2 - 100;rr.y=1080 / 2 - 0;
	rr.w=200;rr.h=200;
	SDL_RenderFillRect(renderer, &rr);




	SDL_RenderCopy(renderer, tex_render_target, NULL, &game_viewport.getRenderRect());

}

SDL_Texture* Game::getLightRenderTarget()
{
	return tex_render_lightmap;
}


void Game::updateLightFromUi()
{
	if (!lights_selection) return;
	if (selected_light != NULL) {
		//int currentPlane=mainmenue->currentPlane();
		selected_light->sprite_no=lights_selection->selectedLight();
		selected_light->angle=lights_selection->lightAngle();
		selected_light->scale_x=lights_selection->lightScaleX();
		selected_light->scale_y=lights_selection->lightScaleY();
		selected_light->color=lights_selection->color();
		selected_light->flarePlane=lights_selection->getLensFlarePlane();
		selected_light->intensity=lights_selection->colorIntensity();
		selected_light->enabled=lights_selection->getCurrentState();
		selected_light->initial_state=lights_selection->getInitialState();
		selected_light->has_lensflare=lights_selection->getLensflare();
		selected_light->flare_intensity=lights_selection->getLensflareIntensity();
		selected_light->flare_useLightColor=lights_selection->getFlareUseLightColor();
		selected_light->playerPlane=static_cast<uint8_t>(lights_selection->getPlayerPlaneMatrix());
		selected_light->myType=lights_selection->getLightType();
		selected_light->typeParameter=lights_selection->lightTypeParameter();

		//selected_light_system->modifyLight(selected_light);
	}
}


void Game::startLevelModification(double time, void* object)
{
	Decker::Objects::LevelModificator* mod=static_cast<Decker::Objects::LevelModificator*>(object);
	if (mod == NULL || mod == levelModificator.triggerobject) return;
	levelModificator.start=level.runtimeParams;
	levelModificator.end=levelModificator.start;
	if (mod->loadLevelDefault) {
		levelModificator.end=level.params;
		levelModificator.end.CurrentSong=level.params.InitialSong;
	}
	if (mod->changeGlobalLighting) levelModificator.end.GlobalLighting=mod->GlobalLighting;
	if (mod->changeBackground) {
		levelModificator.end.BackgroundColor=mod->BackgroundColor;
		levelModificator.end.BackgroundImage=mod->BackgroundImage;
		levelModificator.end.backgroundType=Background::Type::Color;
		if (mod->backgroundType == Decker::Objects::LevelModificator::BackgroundType::Image) levelModificator.end.backgroundType=Background::Type::Image;
	}
	if (levelModificator.start == levelModificator.end) return;
	if (levelModificator.end.backgroundType == Background::Type::Image) {
		background.setFadeTargetImage(levelModificator.end.BackgroundImage);
	} else {
		background.setFadeTargetColor(levelModificator.end.BackgroundColor);
	}

	levelModificator.starttime=time;
	levelModificator.triggerobject=object;
	levelModificator.duration=mod->transitionTime;
	//ppl7::PrintDebugTime("Game::startLevelModification\n");
}

void Game::updateLevelModificator(double time)
{
	if (levelModificator.triggerobject == NULL) return;
	float progress=(time - levelModificator.starttime) / levelModificator.duration;
	if (levelModificator.duration <= 0.0f) progress=1.0f;
	//ppl7::PrintDebugTime("Game::updateLevelModificator: %0.3f\n", progress);

	if (time >= levelModificator.starttime + levelModificator.duration || progress >= 1.0f) {
		levelModificator.triggerobject=NULL;
		level.runtimeParams=levelModificator.end;
		background.setFadeProgress(1.0f);
		return;
	}
	background.setFadeProgress(progress);
	float revprog=1.0f - progress;

	if (levelModificator.start.GlobalLighting != levelModificator.end.GlobalLighting) {
		level.runtimeParams.GlobalLighting=levelModificator.start.GlobalLighting * revprog + levelModificator.end.GlobalLighting * progress;
	}

	/*
	if (levelModificator.start.BackgroundColor != levelModificator.end.BackgroundColor) {
		level.runtimeParams.BackgroundColor=levelModificator.start.BackgroundColor * revprog + levelModificator.end.BackgroundColor * progress;
	}
	*/

}

void* Game::getLevelModificationObject() const
{
	return levelModificator.triggerobject;
}


Screenshot::Screenshot()
{
	ppl7::DateTime now=ppl7::DateTime::currentTime();
	Filename.setf("decker_%s", (const char*)now.getISO8601withMsec());
	Filename.replace(":", "-");
}

void Screenshot::setPath(const ppl7::String& path)
{
	this->Path=path;
}

void Screenshot::save(LightPlaneId lplane, LightPlayerPlaneMatrix pplane, Type type, SDL_Texture* texture)
{
	Layer layer=Layer::Complete;
	switch (lplane) {
	case LightPlaneId::Horizon: layer=Layer::Horizon; break;
	case LightPlaneId::Far: layer=Layer::Far; break;
	case LightPlaneId::Middle: layer=Layer::Middle; break;
	case LightPlaneId::Player:
		switch (pplane) {
		case LightPlayerPlaneMatrix::Back: layer=Layer::Back; break;
		case LightPlayerPlaneMatrix::Player: layer=Layer::Player; break;
		case LightPlayerPlaneMatrix::Front: layer=Layer::Front; break;
		default: break;
		}
		break;
	case LightPlaneId::Near: layer=Layer::Near; break;
	default: break;
	}
	save(layer, type, texture);
}

void Screenshot::save(Layer layer, Type type, SDL_Texture* texture)
{
	ppl7::String filename=Path;
	if (Path.notEmpty()) filename+="/";
	filename+=this->Filename;
	filename.appendf("_layer%02d_", static_cast<int>(layer));
	switch (layer) {
	case Layer::Background: filename+="background"; break;
	case Layer::Horizon: filename+="horizon"; break;
	case Layer::Far: filename+="far"; break;
	case Layer::Middle: filename+="middle"; break;
	case Layer::Back: filename+="back"; break;
	case Layer::Player: filename+="player"; break;
	case Layer::Front: filename+="front"; break;
	case Layer::Near: filename+="near"; break;
	case Layer::Complete: filename+="complete"; break;
	}
	filename+="_";
	switch (type) {
	case Type::Color: filename+="0_color"; break;
	case Type::Lightmap: filename+="1_lightmap"; break;
	case Type::Final: filename+="2_final"; break;
	}
	filename+=".png";
	ppl7::PrintDebugTime("screenshot: %s\n", (const char*)filename);

	SDL_Renderer* renderer=GetGame().getSDLRenderer();
	if (!renderer) return;
	int width, height;
	if (SDL_QueryTexture(texture, NULL, NULL, &width, &height) != 0) return;
	void* pixels=malloc(width * 4 * height);
	if (!pixels) return;

	try {
		//ppl7::PrintDebug("try read pixel: %d x %d\n", width, height);
		if (SDL_RenderReadPixels(renderer, NULL, 0, pixels, width * 4) != 0) {
			ppl7::String err(SDL_GetError());
			throw SDLException("Couldn't read pixel from render target: " + err);
		}
		//ppl7::PrintDebug("create drawable\n");
		ppl7::grafix::Drawable draw(pixels, 4 * width, width, height, ppl7::grafix::RGBFormat::A8R8G8B8);
		ppl7::grafix::ImageFilter_PNG png;
		//ppl7::PrintDebug("save png\n");
		png.saveFile(filename, draw);
		//ppl7::PrintDebug("done\n");
	} catch (const ppl7::Exception& exp) {
		exp.print();
		ppl7::PrintDebug("\n");
	}
	if (pixels) free(pixels);
}


void Game::TakeScreenshot()
{
	if (screenshot) level.TakeScreenshot(NULL);
	delete screenshot;
	screenshot=new Screenshot();
	level.TakeScreenshot(screenshot);
}
