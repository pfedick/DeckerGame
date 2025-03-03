#include <stdio.h>
#include <stdlib.h>

#include "decker.h"
#include "screens.h"
#include "version.h"
#include "translate.h"
#include "player.h"

static void getDestinationRect(ppl7::grafix::Size img_size, ppltk::Window& window, SDL_Rect& dest)
{
	const ppl7::grafix::Size& wSize=window.windowSize();

	float aspect=(float)window.width() / (float)window.height();
	float img_aspect=(float)img_size.width / (float)img_size.height;
	dest.x=0;
	dest.y=0;
	dest.w=wSize.width;
	dest.h=wSize.height;
	if (dest.w > wSize.width) dest.w=wSize.width;
	dest.h=dest.w / aspect;
	if (dest.h > wSize.height) {
		dest.h=wSize.height;
		dest.w=dest.h * aspect;
	}
	dest.y=(wSize.height - dest.h) / 2;
	dest.h=dest.w / img_aspect;
	dest.x=(wSize.width - dest.w) / 2;
}

// const ppl7::grafix::Size& wSize=window.windowSize();

GameState Game::showStartScreen(AudioStream& GeorgeDeckerTheme)
{
	world_widget->setVisible(false);
	world_widget->setEnabled(false);
	ppl7::grafix::Color black(0, 0, 0, 255);
	SDL_Renderer* renderer=sdl.getRenderer();

	SDL_Texture* title_tex=sdl.createStreamingTexture("res/game_title.png");
	ppl7::grafix::Size title_size=sdl.getTextureSize(title_tex);
	SDL_Rect title_rect;
	getDestinationRect(title_size, *this, title_rect);
	//title_rect.y=0;


	StartScreen* start_screen=new StartScreen(*this, 0, title_size.height, this->width(), this->height() - title_size.height);
	this->addChild(start_screen);
	wm->setKeyboardFocus(start_screen);
	wm->setGameControllerFocus(start_screen);
	showUi(false);
	//int fade_to_black=255;
	int fade_state=0;
	quitGame=false;
	startLevel("level/start.lvl");
	enableControls(false);
	ppl7::grafix::Rect last_viewport=viewport;
	while (1) {
		wm->handleEvents();
		sdl.startFrame(black);
		ppltk::MouseState mouse=wm->getMouseState();
		drawWorld(renderer);
		SDL_SetRenderTarget(renderer, NULL);
		drawRenderTargetToScreen();



		if (last_viewport != viewport) {
			last_viewport=viewport;
			start_screen->setPos(0, title_rect.h);
			start_screen->setSize(this->width(), this->height() - title_rect.h);
			start_screen->resizeEvent(NULL);
		}


		if (fade_state == 0) {
			fade_to_black-=5 * frame_rate_compensation;
			if (fade_to_black <= 0.0f) {
				fade_state=1;
				fade_to_black=0;
			}
		} else if (fade_state == 2) {
			fade_to_black+=5 * frame_rate_compensation;
			if (fade_to_black >= 255.0f) {
				fade_state=3;
				fade_to_black=255.0f;
			}
		}
		if (fade_to_black > 0.0f) FadeToBlack(renderer, (int)fade_to_black);
		drawWidgets();
		getDestinationRect(title_size, *this, title_rect);
		SDL_RenderCopy(renderer, title_tex, NULL, &title_rect);

		resources.Cursor.draw(renderer, mouse.p.x, mouse.p.y, 11, ppl7::grafix::Color(255, 200, 0, 255));

		presentScreen();

		if (quitGame == true && fade_state == 1) {
			start_screen->setState(StartScreen::State::QuitGame);
		}
		if (start_screen->getState() != StartScreen::State::None && fade_state == 1) {
			fade_state = 2;
			fade_to_black=0;
			gameState=GameState::BlendOut;
			if (start_screen->getState() != StartScreen::State::ShowSettings && start_screen->getState() != StartScreen::State::SelectLevel)
				GeorgeDeckerTheme.fadeout(4.0f);
		} else if (start_screen->getState() != StartScreen::State::None && fade_state == 3) {
			break;
		}
		if (!audiosystem.isPlaying(&GeorgeDeckerTheme)) {
			GeorgeDeckerTheme.rewind();
			audiosystem.play(&GeorgeDeckerTheme);
		}
		if (gameState == GameState::StartNextLevel) {
			player->resetLevelObjects();
			startLevel(nextLevelFile);
		}

	}
	gameState=GameState::Running;
	StartScreen::State state=start_screen->getState();
	this->removeChild(start_screen);
	sdl.destroyTexture(title_tex);
	selectedLevelFilename=start_screen->selectedLevel();
	delete start_screen;
	start_screen=NULL;
	switch (state) {
		case StartScreen::State::QuitGame: return GameState::QuitGame;
		case StartScreen::State::StartGame: return GameState::StartGame;
		case StartScreen::State::StartTutorial: return GameState::StartTutorial;
		case StartScreen::State::SelectLevel: return GameState::SelectLevel;
		case StartScreen::State::ShowSettings: return GameState::ShowSettings;
		case StartScreen::State::StartEditor: return GameState::StartEditor;
		case StartScreen::State::StartLevel: return GameState::StartLevel;
		default: return GameState::QuitGame;
	}
	return GameState::QuitGame;
}


ppl7::String Game::selectedLevel() const
{
	return selectedLevelFilename;
}



StartScreen::StartScreen(Game& g, int x, int y, int width, int height)
	: game(g)
{
	settings_screen=NULL;
	level_select_screen=NULL;
	create(x, y, width, height);
	const ppltk::WidgetStyle& style=ppltk::GetWidgetStyle();
	ppl7::grafix::Font font=style.buttonFont;
	font.setName("NotoSansBlack");
	font.setBold(false);
	font.setSize(20);
	font.setOrientation(ppl7::grafix::Font::TOP);
	font.setColor(ppl7::grafix::Color(255, 255, 255, 255));

	state=State::None;

	ppl7::grafix::Color background(20, 10, 0, 192);

	menue=new ppltk::Frame(50, 50, 550, height - 100, ppltk::Frame::BorderStyle::NoBorder);
	menue->setBackgroundColor(background);
	this->addChild(menue);

	int yy=10;
	start_tutorial=new Decker::ui::GameMenuArea(10, yy, 520, 90, translate("Start Tutorial"));
	start_tutorial->setEventHandler(this);
	menue->addChild(start_tutorial);
	yy+=100;

	start_game=new Decker::ui::GameMenuArea(10, yy, 520, 90, translate("Start Game"));
	start_game->setEventHandler(this);
	menue->addChild(start_game);
	yy+=100;

	select_level=new Decker::ui::GameMenuArea(10, yy, 520, 90, translate("Select Level"));
	select_level->setEventHandler(this);
	menue->addChild(select_level);
	yy+=100;

	settings=new Decker::ui::GameMenuArea(10, yy, 520, 90, translate("Settings"));
	settings->setEventHandler(this);
	menue->addChild(settings);
	yy+=100;

	editor=new Decker::ui::GameMenuArea(10, yy, 520, 90, translate("Level editor"));
	editor->setEventHandler(this);
	menue->addChild(editor);
	yy+=100;

	end=new Decker::ui::GameMenuArea(10, yy, 520, 90, translate("Exit game"));
	end->setEventHandler(this);
	menue->addChild(end);
	yy+=100;

	version=new ppltk::Label(10, menue->height() - 50, 520, 50);
	version->setBorderStyle(ppltk::Frame::BorderStyle::NoBorder);
	version->setBackgroundColor(background);
	version->setFont(font);

	ppl7::String DateFormat=translate("%Y-%m-%d");
	int a, year, month, day;
	a=DECKER_BUILDDATE;
	year=a / 10000;
	a=a - year * 10000;
	month=a / 100;
	day=a - month * 100;
	ppl7::ppl_time_t t=ppl7::MkTime(year, month, day);
	ppl7::String Date=ppl7::MkDate(DateFormat, t);
	ppl7::String Version;
	Version.setf("%s: %s.%s", (const char*)translate("Version"), DECKER_VERSION, DECKER_REVSION);
	Version.appendf(", %s: %s", (const char*)translate("Builddate"), (const char*)Date);
	version->setText(Version);
	menue->addChild(version);

	/*
	filedialog=new Decker::ui::FileDialog(800,600);
	filedialog->setPos(800,300);
	filedialog->setEventHandler(this);
	filedialog->setFilename("C:\\git\\DeckerGame\\level\\test.lvl");
	this->addChild(filedialog);
	*/
	retranslateUi();
	if (game.config.tutorialPlayed == false) start_tutorial->setSelected(true);
	else start_game->setSelected(true);
	resizeEvent(NULL);
}

StartScreen::~StartScreen()
{
	//if (filedialog) delete filedialog;
	if (settings_screen) delete settings_screen;
	if (level_select_screen) delete level_select_screen;
	delete version;
	delete start_tutorial;
	delete start_game;
	delete settings;
	delete editor;
	delete end;
	this->removeChild(menue);
	delete menue;
}

ppl7::String StartScreen::selectedLevel() const
{
	return selectedLevelFilename;
}

void StartScreen::retranslateUi()
{
	start_tutorial->setText(translate("Start Tutorial"));
	start_game->setText(translate("Start Game"));
	settings->setText(translate("Settings"));
	editor->setText(translate("Level editor"));
	select_level->setText(translate("Select Level"));
	end->setText(translate("Exit game"));

}

StartScreen::State StartScreen::getState() const
{
	return state;
}

void StartScreen::setState(State state)
{
	this->state=state;
}

void StartScreen::showSettings()
{
	menue->setEnabled(false);
	menue->setVisible(false);
	if (!settings_screen) {
		settings_screen=new SettingsScreen(game,
			50, 50, this->width() - 100, this->height() - 100);
		resizeSettingsScreen();
	}
	this->addChild(settings_screen);
	this->needsRedraw();
}

void StartScreen::showLevelSection()
{
	menue->setEnabled(false);
	menue->setVisible(false);
	if (!level_select_screen) {
		level_select_screen=new LevelSelectScreen(game, 50, 50, this->width() - 100, this->height() - 100);
		resizeSettingsScreen();
	}
	this->addChild(level_select_screen);
	this->needsRedraw();
}

void StartScreen::paint(ppl7::grafix::Drawable& draw)
{
	draw.cls();
}


void StartScreen::mouseEnterEvent(ppltk::MouseEvent* event)
{
	//printf("StartScreen::mouseEnterEvent\n");

	if (event->widget() == start_tutorial) {
		start_tutorial->setSelected(true);
		start_game->setSelected(false);
		select_level->setSelected(false);
		settings->setSelected(false);
		editor->setSelected(false);
		end->setSelected(false);
	} else if (event->widget() == start_game) {
		start_tutorial->setSelected(false);
		start_game->setSelected(true);
		select_level->setSelected(false);
		settings->setSelected(false);
		editor->setSelected(false);
		end->setSelected(false);
	} else if (event->widget() == select_level) {
		start_tutorial->setSelected(false);
		start_game->setSelected(false);
		select_level->setSelected(true);
		settings->setSelected(false);
		editor->setSelected(false);
		end->setSelected(false);
	} else if (event->widget() == settings) {
		start_tutorial->setSelected(false);
		start_game->setSelected(false);
		select_level->setSelected(false);
		settings->setSelected(true);
		editor->setSelected(false);
		end->setSelected(false);

	} else if (event->widget() == editor) {
		start_tutorial->setSelected(false);
		start_game->setSelected(false);
		select_level->setSelected(false);
		settings->setSelected(false);
		editor->setSelected(true);
		end->setSelected(false);
	} else if (event->widget() == end) {
		start_tutorial->setSelected(false);
		start_game->setSelected(false);
		select_level->setSelected(false);
		settings->setSelected(false);
		editor->setSelected(false);
		end->setSelected(true);
	}

}

void StartScreen::mouseClickEvent(ppltk::MouseEvent* event)
{
	if (event->widget() == end) {
		state=State::QuitGame;
	} else if (event->widget() == start_game) {
		state=State::StartGame;
	} else if (event->widget() == start_tutorial) {
		state=State::StartTutorial;
	} else if (event->widget() == settings) {
		showSettings();
	} else if (event->widget() == select_level) {
		showLevelSection();
	} else if (event->widget() == editor) {
		state=State::StartEditor;
	}
}

void StartScreen::handleKeyDownEvent(int key)
{
	if (key == ppltk::KeyEvent::KEY_DOWN) {
		if (start_tutorial->isSelected()) {
			start_tutorial->setSelected(false);
			start_game->setSelected(true);
		} else if (start_game->isSelected()) {
			start_game->setSelected(false);
			select_level->setSelected(true);
		} else if (select_level->isSelected()) {
			select_level->setSelected(false);
			settings->setSelected(true);
		} else if (settings->isSelected()) {
			settings->setSelected(false);
			editor->setSelected(true);
		} else if (editor->isSelected()) {
			editor->setSelected(false);
			end->setSelected(true);
		}
	} else if (key == ppltk::KeyEvent::KEY_UP) {
		if (end->isSelected()) {
			end->setSelected(false);
			editor->setSelected(true);
		} else if (editor->isSelected()) {
			editor->setSelected(false);
			settings->setSelected(true);
		} else if (select_level->isSelected()) {
			select_level->setSelected(false);
			start_game->setSelected(true);
		} else if (settings->isSelected()) {
			settings->setSelected(false);
			select_level->setSelected(true);
		} else if (start_game->isSelected()) {
			start_game->setSelected(false);
			start_tutorial->setSelected(true);
		}
	} else if (key == ppltk::KeyEvent::KEY_RETURN) {
		if (start_game->isSelected()) state=State::StartGame;
		else if (settings->isSelected()) showSettings();
		else if (select_level->isSelected()) showLevelSection();
		else if (editor->isSelected()) state=State::StartEditor;
		else if (end->isSelected()) state=State::QuitGame;
		else if (start_tutorial->isSelected()) state=State::StartTutorial;
	}
}

void StartScreen::keyDownEvent(ppltk::KeyEvent* event)
{
	//printf("StartScreen::keyDownEvent\n");
	handleKeyDownEvent(event->key);
}

void StartScreen::gameControllerButtonDownEvent(ppltk::GameControllerButtonEvent* event)
{
	GameControllerMapping::Button b=game.controller.mapping.getButton(event);
	if (b == GameControllerMapping::Button::MenuDown) handleKeyDownEvent(ppltk::KeyEvent::KEY_DOWN);
	else if (b == GameControllerMapping::Button::MenuUp) handleKeyDownEvent(ppltk::KeyEvent::KEY_UP);
	else if (b == GameControllerMapping::Button::Action || b == GameControllerMapping::Button::MenuRight) handleKeyDownEvent(ppltk::KeyEvent::KEY_RETURN);


}

void StartScreen::gameControllerDeviceAdded(ppltk::GameControllerEvent* event)
{
	GetGame().gameControllerDeviceAdded(event);
}

void StartScreen::gameControllerDeviceRemoved(ppltk::GameControllerEvent* event)
{
	GetGame().gameControllerDeviceRemoved(event);
}


void StartScreen::closeEvent(ppltk::Event* event)
{
	if (event->widget() == settings_screen) {
		this->removeChild(settings_screen);
		delete settings_screen;
		settings_screen=NULL;
		menue->setEnabled(true);
		menue->setVisible(true);
		retranslateUi();
		ppltk::GetWindowManager()->setKeyboardFocus(this);
		ppltk::GetWindowManager()->setGameControllerFocus(this);
		needsRedraw();

	} else if (event->widget() == level_select_screen) {
		//ppl7::PrintDebug("StartScreen::closeEvent\n");
		selectedLevelFilename=level_select_screen->selectedLevel();
		this->removeChild(level_select_screen);
		delete level_select_screen;
		level_select_screen=NULL;
		if (selectedLevelFilename.isEmpty()) {
			menue->setEnabled(true);
			menue->setVisible(true);
		}
		retranslateUi();
		ppltk::GetWindowManager()->setKeyboardFocus(this);
		ppltk::GetWindowManager()->setGameControllerFocus(this);


		if (selectedLevelFilename.notEmpty()) {
			state=State::StartLevel;
		}

		needsRedraw();
	}
}

void StartScreen::resizeEvent(ppltk::ResizeEvent* event)
{
	//printf("StartScreen: we got a resize event: %d x %d\n", width(), height());
	int menue_h=height() - 100;
	int menue_w=550;
	int element_h=90;
	int element_w=520;
	int font_size=30;
	int border_width=10;
	int font_size_version=20;

	if (height() < 600) {
		menue_h=height() - 80;
		menue_w=480;
		element_h=70;
		element_w=460;
		font_size=20;
		border_width=7;
		font_size_version=16;
	}
	if (height() < 500) {
		menue_h=height() - 50;
		menue_w=350;
		element_h=50;
		element_w=330;
		font_size=16;
		border_width=5;
		font_size_version=12;
	}
	menue->setSize(menue_w, menue_h);
	int yy=10;
	start_tutorial->setPos(10, yy);			start_tutorial->setSize(element_w, element_h);
	yy+=element_h + 10;
	start_game->setPos(10, yy);				start_game->setSize(element_w, element_h);
	yy+=element_h + 10;
	select_level->setPos(10, yy);				select_level->setSize(element_w, element_h);
	yy+=element_h + 10;
	settings->setPos(10, yy);	settings->setSize(element_w, element_h);
	yy+=element_h + 10;
	editor->setPos(10, yy);	editor->setSize(element_w, element_h);
	yy+=element_h + 10;
	end->setPos(10, yy);	end->setSize(element_w, element_h);
	yy+=element_h + 10;

	start_tutorial->setFontSize(font_size);
	start_game->setFontSize(font_size);
	select_level->setFontSize(font_size);
	settings->setFontSize(font_size);
	editor->setFontSize(font_size);
	end->setFontSize(font_size);

	start_tutorial->setBorderWidth(border_width);
	start_game->setBorderWidth(border_width);
	select_level->setBorderWidth(border_width);
	settings->setBorderWidth(border_width);
	editor->setBorderWidth(border_width);
	end->setBorderWidth(border_width);

	version->setPos(10, menue->height() - 50);
	ppl7::grafix::Font font=version->font();
	font.setSize(font_size_version);
	version->setFont(font);
	resizeSettingsScreen();
}

void StartScreen::resizeSettingsScreen()
{
	if (settings_screen) {
		ppl7::grafix::Point p1(50, 50);
		ppl7::grafix::Size s1(this->width() - 100, this->height() - 100);
		if (height() < 600 || width() < 1280) {
			p1.setPoint(30, 30);
			s1.setSize(this->width() - 60, this->height() - 60);

		}
		if (height() < 500 || width() < 1024) {
			p1.setPoint(30, 30);
			s1.setSize(this->width() - 60, this->height() - 60);
		}
		settings_screen->setSize(s1);
		settings_screen->setPos(p1);
		settings_screen->resizeEvent(NULL);
	}
	if (level_select_screen) {
		ppl7::grafix::Point p1(50, 50);
		ppl7::grafix::Size s1(this->width() - 100, this->height() - 100);
		if (height() < 600 || width() < 1280) {
			p1.setPoint(30, 30);
			s1.setSize(this->width() - 60, this->height() - 60);

		}
		if (height() < 500 || width() < 1024) {
			p1.setPoint(30, 30);
			s1.setSize(this->width() - 60, this->height() - 60);
		}
		level_select_screen->setSize(s1);
		level_select_screen->setPos(p1);
		level_select_screen->resizeEvent(NULL);
	}

	needsRedraw();
}
