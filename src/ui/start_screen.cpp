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

GameState Game::showStartScreen(AudioStream& GeorgeDeckerTheme)
{
	world_widget->setVisible(false);
	world_widget->setEnabled(false);
	ppl7::grafix::Color black(128, 0, 0, 255);
	SDL_Renderer* renderer=sdl.getRenderer();
	StartScreen* widget=new StartScreen(*this, sdl, 0, 0, this->width(), this->height());
	this->addChild(widget);
	wm->setKeyboardFocus(widget);
	showUi(false);
	int fade_to_black=255;
	int fade_state=0;
	quitGame=false;
	player->setVisible(false);
	enableControls(false);
	startLevel("level/start.lvl");
	while (1) {
		wm->handleEvents();
		sdl.startFrame(black);
		ppl7::tk::MouseState mouse=wm->getMouseState();
		drawWorld(renderer);
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
			widget->setState(StartScreen::State::QuitGame);
		}
		if (widget->getState() != StartScreen::State::None && fade_state == 1) {
			fade_state = 2;
			fade_to_black=0;
			if (widget->getState() != StartScreen::State::ShowSettings)
				GeorgeDeckerTheme.fadeout(4.0f);
		} else if (widget->getState() != StartScreen::State::None && fade_state == 3) {
			break;
		}
		if (!audiosystem.isPlaying(&GeorgeDeckerTheme)) {
			GeorgeDeckerTheme.rewind();
			audiosystem.play(&GeorgeDeckerTheme);
		}

	}
	StartScreen::State state=widget->getState();
	this->removeChild(widget);
	delete widget;
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



StartScreen::StartScreen(Game& g, SDL& s, int x, int y, int width, int height)
	: game(g), sdl(s)
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
	TitleImage.load("res/game_title.png");
	menue=new ppl7::tk::Frame(50, TitleImage.height() + 50, 550, height - TitleImage.height() - 100, ppl7::tk::Frame::BorderStyle::NoBorder);
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
		settings_screen=new SettingsScreen(game, sdl,
			50, TitleImage.height() + 50, this->width() - 100, this->height() - 100 - TitleImage.height());
	}
	this->addChild(settings_screen);
	this->needsRedraw();
}

void StartScreen::paint(ppl7::grafix::Drawable& draw)
{
	draw.cls();
	ppl7::grafix::Color bg(40, 20, 0, 96);
	draw.fillRect(0, 0, draw.width(), TitleImage.height() + 8, bg);
	draw.bltAlpha(TitleImage, 0, 0);
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
