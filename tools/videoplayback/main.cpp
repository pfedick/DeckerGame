//#include <stdio.h>
//#include <stdlib.h>
#include <SDL.h>
#include <ppl7-grafix.h>
#include <ppl7-tk.h>
#include "decker_sdl.h"
#include "audio.h"
#include "decker.h"
#include "videoplayer.h"


class VPlay : private ppl7::tk::Window
{
private:
	SDL sdl;
	ppl7::tk::WindowManager* wm;
	SDL_Renderer* renderer;
	bool quitPlayer;
	ppl7::tk::Label* metrics;
	FPS fps;
	ppl7::grafix::Font uifont;
	VideoPlayer player;

public:
	VPlay();
	~VPlay();
	void init();
	void run();
	void loadvideo();


	// EventHandler
	void quitEvent(ppl7::tk::Event* event);
	void closeEvent(ppl7::tk::Event* event);
	void resizeEvent(ppl7::tk::ResizeEvent* event);
	void mouseDownEvent(ppl7::tk::MouseEvent* event);
	void keyDownEvent(ppl7::tk::KeyEvent* event);

};


ppl7::grafix::Point GetViewPos()
{
	return ppl7::grafix::Point(0, 0);
}

void help()
{
	printf("Usage:\n\n");
	fflush(stdout);
}


void start(int argc, char** argv)
{
#ifdef WIN32
	ppl7::String::setGlobalEncoding("UTF-8");
#endif

	if (setlocale(LC_CTYPE, "") == NULL) {
		printf("setlocale fehlgeschlagen\n");
		throw std::exception();
	}
	if (setlocale(LC_NUMERIC, "C") == NULL) {
		printf("setlocale fuer LC_NUMERIC fehlgeschlagen\n");
		throw std::exception();
	}

	if (ppl7::HaveArgv(argc, argv, "-h") || ppl7::HaveArgv(argc, argv, "--help")) {
		help();
		return;
	}


	SDL_Init(SDL_INIT_VIDEO);

	ppl7::grafix::Grafix gfx;
	ppl7::tk::WindowManager_SDL2 wm;

	VPlay vplay;
	vplay.init();
	//vplay.loadvideo();
	vplay.run();


	//AudioStream GeorgeDeckerTheme("res/audio/PatrickF-George_Decker_Theme.mp3", AudioClass::Music);


}

#ifdef WIN32
int WinMain()
{
	start(__argc, __argv);
	return 0;
	try {
		start(__argc, __argv);
		return 0;
	} catch (const ppl7::Exception& ex) {
		ex.print();
		throw;
		return 1;
	}
	return 0;
}
#endif

int main(int argc, char** argv)
{

	start(argc, argv);
	return 0;
}


VPlay::VPlay()
{
	wm=ppl7::tk::GetWindowManager();
	quitPlayer=false;
	renderer=NULL;
}

VPlay::~VPlay()
{

}

void VPlay::init()
{
	setFlags(ppl7::tk::Window::WaitVsync | ppl7::tk::Window::Resizeable);
	setWindowTitle("George Decker - Videoplayer");
	ppl7::grafix::Image icon;
	icon.load("res/icon_128.png");
	setWindowIcon(icon);
	setRGBFormat(ppl7::grafix::RGBFormat::A8R8G8B8);
	setBackgroundColor(ppl7::grafix::Color(0, 0, 0, 0));
	setSize(1920, 1080);
	wm->createWindow(*this);
	renderer=(SDL_Renderer*)getRenderer();
	sdl.setRenderer(renderer);
	SDL_ShowCursor(SDL_DISABLE);
	uifont.setAntialias(true);
	uifont.setBold(true);
	uifont.setColor(ppl7::grafix::Color(255, 255, 255, 255));

	metrics=new ppl7::tk::Label(10, 0, 400, 30, "Test");
	metrics->setFont(uifont);
	metrics->setColor(ppl7::grafix::Color(255, 255, 255, 255));
	addChild(metrics);
	player.setRenderer(renderer);

	if (!player.load("res/video/george_decker_game.ivf")) {
		ppl7::PrintDebugTime("ERROR: Could not load video\n");
	}

}

void VPlay::run()
{
	double frame_time=0.0f;
	double next_video_frame=0.0f;
	double video_frametime=1.0f / (double)player.framerate();
	size_t video_frame_count=0;
	ppl7::ppl_time_t last_second=ppl7::GetTime();
	wm->setKeyboardFocus(this);

	while (!quitPlayer) {
		double frame_start=ppl7::GetMicrotime();
		if (next_video_frame == 0.0f) next_video_frame=frame_start;
		fps.update();
		ppl7::ppl_time_t current_second=ppl7::GetTime();
		if (current_second > last_second) {
			last_second=current_second;
			metrics->setText(ppl7::ToString("%d fps, frametime: %0.3f ms", fps.getFPS(), frame_time / fps.getFPS() * 1000.0f));
			frame_time=0.0f;
		}
		wm->handleEvents();
		if (next_video_frame <= frame_start) {
			//ppl7::PrintDebugTime("nextframe: %zd\n", video_frame_count);
			if (player.eof()) quitPlayer=true;
			player.nextFrame();
			video_frame_count++;
			next_video_frame+=video_frametime;
		}



		SDL_SetRenderDrawColor(renderer, 50, 0, 0, 0);
		SDL_RenderClear(renderer);
		player.renderFrame();

		drawWidgets();
		frame_time+=(ppl7::GetMicrotime() - frame_start);
		presentScreen();
		if (player.eof()) break;
	}
}

void VPlay::loadvideo()
{
	unsigned int fps[2], num_frames=0, timebase[2];
	fps[0]=0;
	fps[1]=0;
	timebase[0]=0;
	timebase[1]=0;
	/*
	if (!demuxer.open("M:/Decker2/av1/george_decker_game.ivf", fps, &num_frames, timebase)) {
		ppl7::PrintDebugTime("could not open input video\n");
	}
	*/

	ppl7::PrintDebugTime("fps[0]=%d\n", fps[0]);
	ppl7::PrintDebugTime("fps[1]=%d\n", fps[1]);
	ppl7::PrintDebugTime("num_frames=%d\n", num_frames);
	ppl7::PrintDebugTime("timebase[0]=%d\n", timebase[0]);
	ppl7::PrintDebugTime("timebase[1]=%d\n", timebase[1]);
}

void VPlay::quitEvent(ppl7::tk::Event* e)
{
	quitPlayer=true;
}

void VPlay::closeEvent(ppl7::tk::Event* e)
{
	quitPlayer=true;
}


void VPlay::resizeEvent(ppl7::tk::ResizeEvent* event)
{
	ppl7::PrintDebugTime("Game::resizeEvent, Window sagt: %d x %d\n", this->width(), this->height());
}

void VPlay::mouseDownEvent(ppl7::tk::MouseEvent* event)
{
	quitPlayer=true;
}

void VPlay::keyDownEvent(ppl7::tk::KeyEvent* event)
{
	quitPlayer=true;
}
