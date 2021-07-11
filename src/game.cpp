#include "decker.h"
#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <ppl7-grafix.h>
#include "player.h"

static double planeFactor[]={1.0f, 1.0f, 0.5f};

Game::Game()
{
	tex_level_grid=NULL;
	wm=ppl7::tk::GetWindowManager();
	ppl7::tk::WidgetStyle s(ppl7::tk::WidgetStyle::Dark);
	wm->setWidgetStyle(s);
	Style=s;
	player=NULL;
	mainmenue=NULL;
	statusbar=NULL;
	tiles_selection=NULL;
	quitGame=false;
	worldIsMoving=false;
	tex_sky=NULL;
	tiletype_selection=NULL;
	sprite_selection=NULL;
	world_widget=NULL;
	sprite_mode=spriteModeDraw;
	selected_sprite_system=NULL;
	selected_sprite_id=-1;
}

Game::~Game()
{
	if (player) delete player;
	if (tex_level_grid) sdl.destroyTexture(tex_level_grid);
	wm->destroyWindow(*this);
}

void Game::loadGrafix()
{
	resources.Sprite_George.load(sdl, "res/george.tex");
	//printf ("Sprites loaded: %d\n",Sprite_Charlie.numSprites());
	//resources.Tiles.load(sdl, "res/bricks.tex", ppl7::grafix::Color(230,220,0,255));
	resources.Tiles.enableMemoryBuffer(true);
	resources.Tiles.load(sdl, "res/tiles.tex");
	resources.Tiles_Nature.enableMemoryBuffer(true);
	resources.Tiles_Nature.load(sdl, "res/tiles_nature.tex");
	resources.Cursor.load(sdl, "res/cursor.tex");
	resources.Nature.load(sdl, "res/nature.tex");
	resources.Trees.load(sdl, "res/trees.tex");
	resources.TileTypes.enableMemoryBuffer(true);
	resources.TileTypes.load(sdl, "res/tiletypes.tex");
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY,"linear");
	resources.Sprites_Nature.enableOutlines(true);
	resources.Sprites_Nature.enableMemoryBuffer(true);
	resources.Sprites_Nature.load(sdl, "res/sprites_nature.tex");

	//resources.uiTiles.load("res/tiles.tex");
	//resources.uiTileTypes.load("res/tiletypes.tex");
	//resources.uiTilesNature.load("res/tiles_nature.tex");
	resources.uiSpritesNature.enableSDLBuffer(false);
	resources.uiSpritesNature.enableMemoryBuffer(true);
	resources.uiSpritesNature.load(sdl, "res/sprites_nature_ui.tex");


	ppl7::grafix::Image img;
	img.load("res/sky2.png");
	tex_sky=sdl.createStreamingTexture(img.width(), img.height());
	ppl7::grafix::Drawable draw=sdl.lockTexture(tex_sky);
	draw.blt(img);
	sdl.unlockTexture(tex_sky);
}

void Game::createWindow()
{
	setFlags(ppl7::tk::Window::DefaultWindow);
	setWindowTitle("DeckerGame");
	setRGBFormat(ppl7::grafix::RGBFormat::A8R8G8B8);
	setBackgroundColor(ppl7::grafix::Color(0,0,0,0));
	ppl7::grafix::Size desktop=wm->desktopResolution();
	desktop.height-=80;
	setSize(desktop);
	wm->createWindow(*this);
	sdl.setRenderer((SDL_Renderer *)getRenderer());
	SDL_ShowCursor(SDL_DISABLE);

}

SDL_Renderer *Game::getSDLRenderer()
{
	return sdl.getRenderer();
}


void Game::initUi()
{
	//ppl7::grafix::Grafix *gfx=ppl7::grafix::GetGrafix();
	const ppl7::grafix::Size &desktop=clientSize();
	//ppl7::tk::Label *label;

	mainmenue=new Decker::ui::MainMenue(0,0,desktop.width,32, this);
	this->addChild(mainmenue);


	// Bottom Frame
	statusbar=new Decker::ui::StatusBar(0,desktop.height-32,desktop.width,32);
	this->addChild(statusbar);
	viewport.y1=33;
	viewport.y2=desktop.height-33;

	world_widget=new Decker::ui::WorldWidget();
	world_widget->create(0,33,desktop.width, desktop.height-32-33);
	world_widget->setEventHandler(this);
	this->addChild(world_widget);

}

void Game::init()
{
	createWindow();
	loadGrafix();

	desktopSize=sdl.getWindowSize();
	viewport=sdl.getClientWindow();
	initUi();
	PlayerCoords.x=desktopSize.width/2;
	PlayerCoords.y=desktopSize.height/2;

	gui_font.setName("Default");
	gui_font.setSize(12);
	gui_font.setBold(true);
	gui_font.setColor(ppl7::grafix::Color(255,255,255,255));
	gui_font.setBorderColor(ppl7::grafix::Color(0,0,0,0));
	gui_font.setShadowColor(ppl7::grafix::Color(0,0,0,0));
	gui_font.setOrientation(ppl7::grafix::Font::TOP);
	gui_font.setAntialias(true);
	if (player) delete player;
	player=new Player();
	//player->setGameWindow(sdl.getClientWindow());

	player->setSpriteResource(resources.Sprite_George);

	//level.create(255,255);
	level.setTileset(1, &resources.Tiles);
	level.setTileset(2, &resources.Tiles_Nature);
	level.setSpriteset(1, &resources.Sprites_Nature);
	level.setTileTypesSprites(&resources.TileTypes);
	//level.setRenderer
	level.load("level/test.lvl");

	showTilesSelection();


}

void Game::drawGrid()
{
	if (!tex_level_grid) {
		tex_level_grid=sdl.createStreamingTexture(desktopSize.width, desktopSize.height);
		ppl7::grafix::Drawable draw=sdl.lockTexture(tex_level_grid);
		ppl7::grafix::Color white(255,255,255,128);
		ppl7::grafix::Color black(0,0,0,128);

		draw.cls(0);
		for (int x=0;x<desktopSize.width;x+=64) {
			draw.line(x,0,x,desktopSize.height,black);
			draw.line(x+1,0,x+1,desktopSize.height,white);
		}
		for (int y=0;y<desktopSize.height;y+=64) {
			draw.line(0,y,desktopSize.width,y ,black);
			draw.line(0,y+1,desktopSize.width,y+1 ,white);
		}
		sdl.unlockTexture(tex_level_grid);
	}
	int currentPlane=mainmenue->currentPlane();
	ppl7::grafix::Point c=WorldCoords*planeFactor[currentPlane];

	SDL_Renderer *renderer=sdl.getRenderer();
	SDL_Rect target;
	target.x=viewport.x1-(c.x%64);
	target.y=viewport.y1-(c.y%64);
	target.w=desktopSize.width;
	target.h=desktopSize.height;
	SDL_RenderCopy(renderer, tex_level_grid, NULL, &target);
}

void Game::moveWorld(int offset_x, int offset_y)
{
	if (offset_x==0 && offset_y==0) return;
	WorldCoords.x+=offset_x;
	WorldCoords.y+=offset_y;
	if (WorldCoords.x<0) WorldCoords.x=0;
	if (WorldCoords.x>62000) WorldCoords.x=62000;
	if (WorldCoords.y<0) WorldCoords.y=0;
	if (WorldCoords.y>62000) WorldCoords.y=62000;
}

void Game::moveWorldOnMouseClick(const ppl7::tk::MouseState &mouse)
{
	const Uint8 *state = SDL_GetKeyboardState(NULL);
	if (worldIsMoving) {
		if (mouse.buttonMask == ppl7::tk::MouseState::Middle || ((mouse.buttonMask == ppl7::tk::MouseState::Left) && state[SDL_SCANCODE_LSHIFT])) {
			//printf("Move\n");
			moveWorld(WorldMoveStart.x-mouse.p.x, WorldMoveStart.y-mouse.p.y);
			WorldMoveStart=mouse.p;
		} else {
			worldIsMoving=false;
			//printf("End\n");
		}
	} else {
		//printf("mouse.buttonMask=%d\n", mouse.button);
		if (mouse.buttonMask == ppl7::tk::MouseState::Middle || ((mouse.buttonMask == ppl7::tk::MouseState::Left) && state[SDL_SCANCODE_LSHIFT])) {
			//printf("Start\n");
			worldIsMoving=true;
			WorldMoveStart=mouse.p;
		} else {
			worldIsMoving=false;;
		}

	}
}

void Game::updateUi(const ppl7::tk::MouseState &mouse)
{
	fps.update();
	statusbar->setFps(fps.getFPS());
	statusbar->setMouse(mouse);
	statusbar->setWorldCoords(WorldCoords);
	statusbar->setPlayerCoords(PlayerCoords);
	statusbar->setSpriteCount(level.countSprites(), level.countVisibleSprites());

}

void Game::run()
{
	SDL_Renderer *renderer=sdl.getRenderer();
	quitGame=false;
	while (!quitGame) {
		double now=ppl7::GetMicrotime();
		level.updateVisibleSpriteLists(WorldCoords,viewport);	// => TODO: own Thread
		player->update(now);
		wm->handleEvents();
		ppl7::tk::MouseState mouse=wm->getMouseState();
		updateUi(mouse);
		// Handle Mouse events inside World
		if (mouse.p.inside(viewport)) {
			moveWorldOnMouseClick(mouse);
			handleMouseDrawInWorld(mouse);
		}
		sdl.startFrame(Style.windowBackgroundColor);
		level.setViewport(viewport);
		player->setGameWindow(viewport);
		SDL_Rect target;
		target.x=viewport.x1;
		target.y=viewport.y1;
		target.w=desktopSize.width;
		target.h=desktopSize.height;
		SDL_Rect source;
		ppl7::grafix::Point c=WorldCoords*0.1f;
		source.x=c.x;
		source.y=c.y;
		source.w=desktopSize.width;
		source.h=desktopSize.height;
		SDL_RenderCopy(renderer, tex_sky, &source, &target);

		// Draw Planes and Sprites
		level.FarPlane.setVisible(mainmenue->farPlaneVisible());
		level.PlayerPlane.setVisible(mainmenue->playerPlaneVisible());
		level.FrontPlane.setVisible(mainmenue->frontPlaneVisible());
		level.draw(renderer, WorldCoords,player);


		drawSelectedSprite(renderer, mouse.p);
		if (mainmenue->playerPlaneVisible())
			if (mainmenue->showTileTypes()) level.drawTileTypes(renderer, WorldCoords);
		// Grid
		if (mainmenue->showGrid()) drawGrid();

		// Widgets
		drawWidgets();
		// Mouse
		resources.Cursor.draw(renderer,mouse.p.x,mouse.p.y,1);
		presentScreen();
	}
}

void Game::quitEvent(ppl7::tk::Event *e)
{
	quitGame=true;
}

void Game::closeEvent(ppl7::tk::Event *e)
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



void Game::showTilesSelection()
{
	closeTileTypeSelection();
	closeSpriteSelection();
	if (tiles_selection) {
		closeTileSelection();
	} else {
		tiles_selection=new Decker::ui::TilesSelection(0,33,300,statusbar->y()-2-33,this);
		tiles_selection->setTileSet(1,"Bricks", &resources.Tiles);
		tiles_selection->setTileSet(2,"Nature", &resources.Tiles_Nature);
		this->addChild(tiles_selection);
		viewport.x1=300;
		world_widget->setViewport(viewport);
	}
}

void Game::showTileTypeSelection()
{
	closeSpriteSelection();
	closeTileSelection();
	if (tiletype_selection) {
		closeTileTypeSelection();
		mainmenue->setShowTileTypes(false);
	} else {
		tiletype_selection=new Decker::ui::TileTypeSelection(0,33,300,statusbar->y()-2-33,this, &resources.TileTypes);
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
	if (sprite_selection) {
		closeSpriteSelection();
	} else {
		sprite_selection=new Decker::ui::SpriteSelection(0,33,300,statusbar->y()-2-33,this);
		sprite_selection->setSpriteSet(1,"Nature", &resources.uiSpritesNature);
		//sprite_selection->setTileSet(2,"Nature", &resources.uiTilesNature);
		this->addChild(sprite_selection);
		viewport.x1=300;
		sprite_mode=spriteModeDraw;
		selected_sprite_id=-1;
		selected_sprite_system=NULL;
		world_widget->setViewport(viewport);
	}
}



void Game::handleMouseDrawInWorld(const ppl7::tk::MouseState &mouse)
{
	const Uint8 *state = SDL_GetKeyboardState(NULL);
	if (state[SDL_SCANCODE_LSHIFT]) return;

	if (tiletype_selection) {
		ppl7::grafix::Point coords=WorldCoords*planeFactor[0];
		int x=(mouse.p.x-viewport.x1+coords.x)/64;
		int y=(mouse.p.y-viewport.y1+coords.y)/64;
		Tile::TileType type=(Tile::TileType)tiletype_selection->tileType();
		if (mouse.buttonMask==ppl7::tk::MouseState::Left) {
			level.plane(0).setType(x,y,type);
		} else if (mouse.buttonMask==ppl7::tk::MouseState::Right) {
			level.plane(0).setType(x,y,Tile::TileType::NonBlocking);
		}
	} else if (tiles_selection) {
		int currentPlane=mainmenue->currentPlane();

		ppl7::grafix::Point coords=WorldCoords*planeFactor[currentPlane];
		int x=(mouse.p.x-viewport.x1+coords.x)/64;
		int y=(mouse.p.y-viewport.y1+coords.y)/64;

		int selectedTile=tiles_selection->selectedTile();
		int selectedTileSet=tiles_selection->currentTileSet();
		int currentLayer=tiles_selection->currentLayer();

		if (mouse.buttonMask==ppl7::tk::MouseState::Left && selectedTile>=0) {
			level.plane(currentPlane).setTile(x,y,
					currentLayer,
					selectedTileSet,
					selectedTile);
		} else if (mouse.buttonMask==ppl7::tk::MouseState::Right) {
			level.plane(currentPlane).clearTile(x,y,tiles_selection->currentLayer());
		}
	}
}

void Game::drawSelectedSprite(SDL_Renderer *renderer, const ppl7::grafix::Point &mouse)
{
	if (!sprite_selection) return;
	if (sprite_selection->selectedSprite()>=0 && sprite_mode!=spriteModeDraw) {
		selected_sprite_system=NULL;
		sprite_mode=spriteModeDraw;
	}
	if (sprite_mode==SpriteModeEdit && selected_sprite_id>=0 && selected_sprite_system!=NULL) {
		int currentPlane=mainmenue->currentPlane();
		//printf ("drawing selected sprite\n");
		selected_sprite_system->drawSelectedSpriteOutline(renderer, viewport,
				WorldCoords*planeFactor[currentPlane],selected_sprite_id);
	} else if (sprite_mode==spriteModeDraw) {
		if (!mouse.inside(viewport)) return;
		int nr=sprite_selection->selectedSprite()*4;
		if (nr<0) return;
		int spriteset=sprite_selection->currentSpriteSet();
		float scale=sprite_selection->spriteScale();
		if (!level.spriteset[spriteset]) return;
		level.spriteset[spriteset]->drawScaled(renderer,
				mouse.x, mouse.y, nr, scale);
		level.spriteset[spriteset]->drawOutlines(renderer,
					mouse.x, mouse.y, nr, scale);
	}
}

void Game::save()
{
	level.save("level/test.lvl");
}

void Game::load()
{
	printf ("Game::load\n");
	level.load("level/test.lvl");
}

void Game::mouseClickEvent(ppl7::tk::MouseEvent *event)
{
	if (world_widget!=NULL && event->widget()==world_widget) {
		//printf ("click\n");
	}
}

void Game::mouseDownEvent(ppl7::tk::MouseEvent *event)
{
	if (sprite_selection!=NULL && event->widget()==world_widget && event->buttonMask==ppl7::tk::MouseState::Left) {
		int nr=sprite_selection->selectedSprite()*4;
		if (nr<0) {
			selectSprite(event->p);
			return;
		}
		if (sprite_mode!=spriteModeDraw) return;
		int spriteset=sprite_selection->currentSpriteSet();
		float scale=sprite_selection->spriteScale();
		int layer=sprite_selection->currentLayer();
		if (layer<0 || layer>1 || spriteset>MAX_SPRITESETS) return;
		if (!level.spriteset[spriteset]) return;
		int currentPlane=mainmenue->currentPlane();
		SpriteSystem &ss=level.spritesystem(currentPlane, layer);
		ppl7::grafix::Point coords=WorldCoords*planeFactor[currentPlane];
		ss.addSprite(event->p.x+coords.x,
				event->p.y+coords.y,
				0,
				spriteset, nr+ppl7::rand(0, 3), scale);
	} else if (sprite_selection!=NULL && event->widget()==world_widget && event->buttonMask==ppl7::tk::MouseState::Right) {
		sprite_selection->setSelectedSprite(-1);
		sprite_mode=spriteModeDraw;
		selected_sprite_id=-1;
		selected_sprite_system=NULL;
	}
}

void Game::mouseWheelEvent(ppl7::tk::MouseEvent *event)
{
	if (sprite_selection!=NULL && event->widget()==world_widget) {
		float scale=sprite_selection->spriteScale();
		if (event->wheel.y<0 && scale>0.1) scale-=0.1;
		else if (event->wheel.y>0 && scale<1.0) scale+=0.1;
		sprite_selection->setSpriteScale(scale);
	}
}

void Game::selectSprite(const ppl7::grafix::Point &mouse)
{
	int currentPlane=mainmenue->currentPlane();
	ppl7::grafix::Point world=mouse+WorldCoords*planeFactor[currentPlane];
	int layer=sprite_selection->currentLayer();
	if (layer<0 || layer>1) return;

	SpriteSystem &ss=level.spritesystem(currentPlane, layer);
	SpriteSystem::Item sprite=ss.findMatchingSprite(world);
	if (sprite.id>=0) {
		wm->setKeyboardFocus(world_widget);
		sprite_mode=SpriteModeEdit;
		selected_sprite_id=sprite.id;
		selected_sprite_system=&ss;
	} else {
		selected_sprite_id=-1;
		selected_sprite_system=NULL;
	}
	//printf ("versuche sprite zu finden...\n");

}


void Game::keyDownEvent(ppl7::tk::KeyEvent *event)
{
	if (event->widget()==world_widget) {
		if (sprite_mode==SpriteModeEdit && selected_sprite_id>=0
				&& selected_sprite_system!=NULL) {
			if (event->key==ppl7::tk::KeyEvent::KEY_DELETE
					&& (event->modifier&ppl7::tk::KeyEvent::KEYMOD_MODIFIER)==0) {
				//printf ("KeyEvent\n");
				selected_sprite_system->deleteSprite(selected_sprite_id);
				selected_sprite_id=0;
				selected_sprite_system=NULL;
			}
		}
	}
}

