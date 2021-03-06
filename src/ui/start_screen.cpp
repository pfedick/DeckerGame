#include <stdio.h>
#include <stdlib.h>

#include "decker.h"
#include "screens.h"
#include "version.h"
#include "translate.h"
#include "player.h"

static void FadeToBlack(SDL_Renderer* renderer, int fade_to_black)
{
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

static void getDestinationRect(ppl7::grafix::Size img_size, ppl7::tk::Window& window, SDL_Rect& dest)
{
	float aspect=(float)img_size.width / (float)img_size.height;
	dest.x=0;
	dest.y=0;
	dest.w=img_size.width;
	dest.h=img_size.height;
	if (dest.w > window.width()) dest.w=window.width();
	dest.h=dest.w / aspect;
	if (dest.h > window.height()) {
		dest.h=window.height();
		dest.w=dest.h * aspect;
	}
	dest.y=(window.height() - dest.h) / 2;
	dest.x=(window.width() - dest.w) / 2;
}

GameState Game::showStartScreen(AudioStream& GeorgeDeckerTheme)
{
	world_widget->setVisible(false);
	world_widget->setEnabled(false);
	ppl7::grafix::Color black(128, 0, 0, 255);
	SDL_Renderer* renderer=sdl.getRenderer();

	SDL_Texture* title_tex=sdl.createStreamingTexture("res/game_title.png");
	ppl7::grafix::Size title_size=sdl.getTextureSize(title_tex);
	SDL_Rect title_rect;
	getDestinationRect(title_size, *this, title_rect);
	title_rect.y=0;


	StartScreen* start_screen=new StartScreen(*this, 0, title_rect.h, this->width(), this->height() - title_rect.h);
	this->addChild(start_screen);
	wm->setKeyboardFocus(start_screen);
	showUi(false);
	int fade_to_black=255;
	int fade_state=0;
	quitGame=false;
	startLevel("level/start.lvl");
	player->setVisible(false);
	enableControls(false);
	ppl7::grafix::Rect last_viewport=viewport;
	while (1) {
		wm->handleEvents();
		sdl.startFrame(black);
		ppl7::tk::MouseState mouse=wm->getMouseState();
		drawWorld(renderer);
		getDestinationRect(title_size, *this, title_rect);
		title_rect.y=0;
		SDL_RenderCopy(renderer, title_tex, NULL, &title_rect);

		if (last_viewport != viewport) {
			last_viewport=viewport;
			start_screen->setPos(0, title_rect.h);
			start_screen->setSize(this->width(), this->height() - title_rect.h);
			start_screen->resizeEvent(NULL);
		}

		drawWidgets();
		resources.Cursor.draw(renderer, mouse.p.x, mouse.p.y, 1);
		if (fade_state == 0) {
			fade_to_black-=2;
			if (fade_to_black <= 0) {
				fade_state=1;
				fade_to_black=0;
			}
		} else if (fade_state == 2) {
			fade_to_black+=5;
			if (fade_to_black >= 255) {
				fade_state=3;
				fade_to_black=255;
			}
		}
		if (fade_to_black) FadeToBlack(renderer, fade_to_black);
		presentScreen();

		if (quitGame == true && fade_state == 1) {
			start_screen->setState(StartScreen::State::QuitGame);
		}
		if (start_screen->getState() != StartScreen::State::None && fade_state == 1) {
			fade_state = 2;
			fade_to_black=0;
			if (start_screen->getState() != StartScreen::State::ShowSettings)
				GeorgeDeckerTheme.fadeout(4.0f);
		} else if (start_screen->getState() != StartScreen::State::None && fade_state == 3) {
			break;
		}
		if (!audiosystem.isPlaying(&GeorgeDeckerTheme)) {
			GeorgeDeckerTheme.rewind();
			audiosystem.play(&GeorgeDeckerTheme);
		}

	}
	StartScreen::State state=start_screen->getState();
	this->removeChild(start_screen);
	delete start_screen;
	start_screen=NULL;
	switch (state) {
	case StartScreen::State::QuitGame: return GameState::QuitGame;
	case StartScreen::State::StartGame: return GameState::StartGame;
	case StartScreen::State::ShowSettings: return GameState::ShowSettings;
	case StartScreen::State::StartEditor: return GameState::StartEditor;
	default: return GameState::QuitGame;
	}
	return GameState::QuitGame;
}

GameMenuArea::GameMenuArea(int x, int y, int width, int height, const ppl7::String& text)
{
	create(x, y, width, height);
	this->text=text;
	selected=false;
	border_width=10;
	const ppl7::tk::WidgetStyle& style=ppl7::tk::GetWidgetStyle();
	font=style.buttonFont;
	font.setName("NotoSansBlack");
	font.setBold(false);
	font.setSize(30);
	font.setOrientation(ppl7::grafix::Font::TOP);

}

void GameMenuArea::setText(const ppl7::String& text)
{
	this->text=text;
	redrawRequired();
}

void GameMenuArea::setBorderWidth(int width)
{
	border_width=width;
}

void GameMenuArea::setFontSize(int size)
{
	font.setSize(size);
}

void GameMenuArea::paint(ppl7::grafix::Drawable& draw)
{
	ppl7::grafix::Color bg(20, 10, 0, 192);
	ppl7::grafix::Color border(128, 96, 0, 255);
	ppl7::grafix::Color fg(128, 128, 128, 255);
	if (selected) {
		bg.setColor(90, 70, 0, 192);
		border.setColor(255, 200, 0, 255);
		fg.setColor(255, 255, 255, 255);
	}
	draw.fillRect(0, 0, draw.width(), draw.height(), bg);
	for (int i=0;i < border_width;i++) {
		draw.drawRect(i, i, draw.width() - i, draw.height() - i, border);
	}
	font.setColor(fg);
	draw.print(font, border_width + 20, border_width + 10, text);
}

void GameMenuArea::setSelected(bool selected)
{
	if (this->selected != selected) {
		this->selected=selected;
		needsRedraw();
	}
}

bool GameMenuArea::isSelected() const
{
	return selected;
}





StartScreen::StartScreen(Game& g, int x, int y, int width, int height)
	: game(g)
{
	settings_screen=NULL;
	create(x, y, width, height);
	const ppl7::tk::WidgetStyle& style=ppl7::tk::GetWidgetStyle();
	ppl7::grafix::Font font=style.buttonFont;
	font.setName("NotoSansBlack");
	font.setBold(false);
	font.setSize(20);
	font.setOrientation(ppl7::grafix::Font::TOP);
	font.setColor(ppl7::grafix::Color(255, 255, 255, 255));

	state=State::None;

	ppl7::grafix::Color background(20, 10, 0, 192);

	menue=new ppl7::tk::Frame(50, 50, 550, height - 100, ppl7::tk::Frame::BorderStyle::NoBorder);
	menue->setBackgroundColor(background);
	this->addChild(menue);


	start_game=new GameMenuArea(10, 10, 520, 90, translate("Start Game"));
	start_game->setEventHandler(this);
	menue->addChild(start_game);
	settings=new GameMenuArea(10, 110, 520, 90, translate("Settings"));
	settings->setEventHandler(this);
	menue->addChild(settings);
	editor=new GameMenuArea(10, 210, 520, 90, translate("Level editor"));
	editor->setEventHandler(this);
	menue->addChild(editor);
	end=new GameMenuArea(10, 310, 520, 90, translate("Exit game"));
	end->setEventHandler(this);
	menue->addChild(end);

	version=new ppl7::tk::Label(10, menue->height() - 50, 520, 50);
	version->setBorderStyle(ppl7::tk::Frame::BorderStyle::NoBorder);
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

	start_game->setSelected(true);
	resizeEvent(NULL);
}

StartScreen::~StartScreen()
{
	//if (filedialog) delete filedialog;
	if (settings_screen) delete settings_screen;
	delete version;
	delete start_game;
	delete settings;
	delete editor;
	delete end;
	this->removeChild(menue);
	delete menue;
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

void StartScreen::paint(ppl7::grafix::Drawable& draw)
{
	draw.cls();
}


void StartScreen::mouseEnterEvent(ppl7::tk::MouseEvent* event)
{
	//printf("StartScreen::mouseEnterEvent\n");

	if (event->widget() == start_game) {
		start_game->setSelected(true);
		settings->setSelected(false);
		editor->setSelected(false);
		end->setSelected(false);
	} else if (event->widget() == settings) {
		start_game->setSelected(false);
		settings->setSelected(true);
		editor->setSelected(false);
		end->setSelected(false);
	} else if (event->widget() == editor) {
		start_game->setSelected(false);
		settings->setSelected(false);
		editor->setSelected(true);
		end->setSelected(false);
	} else if (event->widget() == end) {
		start_game->setSelected(false);
		settings->setSelected(false);
		editor->setSelected(false);
		end->setSelected(true);
	}

}

void StartScreen::mouseClickEvent(ppl7::tk::MouseEvent* event)
{
	if (event->widget() == end) {
		state=State::QuitGame;
	} else if (event->widget() == start_game) {
		state=State::StartGame;
	} else if (event->widget() == settings) {
		showSettings();
		//state=State::ShowSettings;
	} else if (event->widget() == editor) {
		state=State::StartEditor;
	}
}

void StartScreen::keyDownEvent(ppl7::tk::KeyEvent* event)
{
	//printf("StartScreen::keyDownEvent\n");
	if (event->key == ppl7::tk::KeyEvent::KEY_DOWN) {
		if (start_game->isSelected()) {
			start_game->setSelected(false);
			settings->setSelected(true);
		} else if (settings->isSelected()) {
			settings->setSelected(false);
			editor->setSelected(true);
		} else if (editor->isSelected()) {
			editor->setSelected(false);
			end->setSelected(true);
		}
	} else if (event->key == ppl7::tk::KeyEvent::KEY_UP) {
		if (end->isSelected()) {
			end->setSelected(false);
			editor->setSelected(true);
		} else if (editor->isSelected()) {
			editor->setSelected(false);
			settings->setSelected(true);
		} else if (settings->isSelected()) {
			settings->setSelected(false);
			start_game->setSelected(true);
		}
	} else if (event->key == ppl7::tk::KeyEvent::KEY_RETURN) {
		if (start_game->isSelected()) state=State::StartGame;
		else if (settings->isSelected()) showSettings();
		else if (editor->isSelected()) state=State::StartEditor;
		else if (end->isSelected()) state=State::QuitGame;
	}
}


void StartScreen::closeEvent(ppl7::tk::Event* event)
{
	if (event->widget() == settings_screen) {
		this->removeChild(settings_screen);
		delete settings_screen;
		settings_screen=NULL;
		menue->setEnabled(true);
		menue->setVisible(true);
		ppl7::tk::GetWindowManager()->setKeyboardFocus(this);
		needsRedraw();

	}
	/*
	if (event->widget()==filedialog) {
		if (filedialog->state()==Decker::ui::FileDialog::DialogState::OK) {
			printf ("we have a file: %s\n",(const char*)filedialog->filename());
			fflush(stdout);
		}
		filedialog->deleteLater();
		filedialog->setVisible(false);
		filedialog=NULL;
		redrawRequired();
	}
	*/
}

void StartScreen::resizeEvent(ppl7::tk::ResizeEvent* event)
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
	start_game->setPos(10, 10);				start_game->setSize(element_w, element_h);
	settings->setPos(10, 20 + element_h);	settings->setSize(element_w, element_h);
	editor->setPos(10, 30 + 2 * element_h);	editor->setSize(element_w, element_h);
	end->setPos(10, 40 + 3 * element_h);	end->setSize(element_w, element_h);

	start_game->setFontSize(font_size);
	settings->setFontSize(font_size);
	editor->setFontSize(font_size);
	end->setFontSize(font_size);

	start_game->setBorderWidth(border_width);
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
	needsRedraw();
}
