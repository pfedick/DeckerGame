#include "decker.h"
#include <SDL.h>
#include <ppl7-grafix.h>
#include "player.h"
#include "objects.h"

static int walk_cycle_left[]={ 1,2,3,4,5,6,7,8 };
static int walk_cycle_right[]={ 10,11,12,13,14,15,16,17 };
static int turn_from_left_to_mid[]={ 22,23,24,27 };
static int turn_from_left_to_right[]={ 22,23,24,25,26 };
static int turn_from_right_to_mid[]={ 18,19,20 };
static int turn_from_right_to_left[]={ 18,19,20,21,22 };
static int turn_from_mid_to_left[]={ 27,20,21,22 };
static int turn_from_mid_to_right[]={ 27,24,25,26 };
//static int turn_from_back_to_front[]={30,31,32,23,24};
static int run_cycle_left[]={ 61,62,63,64,65,66,67,68 };
static int run_cycle_right[]={ 70,71,72,73,74,75,76,77 };
static int climb_up_cycle[]={ 91,92,93,94,95,96,97,98,99,100,101 };
static int climb_down_cycle[]={ 101,100,99,98,97,96,95,94,93,92,91 };

static int slide_left[]={ 83,84,85,86 };
static int slide_right[]={ 79,80,81,82 };

static int death_animation[]={ 102,103,105,105,105,106,106,105,105,106,106,
		105,105,106,106,105,104,105,106,105,104,103,104,105,106 };
static int death_by_falling[]={ 89,89,106,106,89,89,106,106,89,106,89,106,89,89,
		106,106,89 };

static int swimm_inplace_front[]={ 126,127,128,129,130,131,132,133,134,135 };
static int swimm_inplace_left[]={ 106,107,108,109,110,111,112,113,114,115 };
static int swimm_inplace_right[]={ 116,117,118,119,120,121,122,123,124,125 };
//static int swimm_inplace_back[]={ 136,137,138,139,140,141,142,143,144,145 };
static int swimm_up_left[]={ 146,147,148,149,150,151,152,153,154,155 };
static int swimm_straight_left[]={ 196,197,198,199,200,201,202,203,204,205 };
static int swimm_down_left[]={ 156,157,158,159,160,161,162,163,164,165 };
static int swimm_up_right[]={ 166,167,168,169,170,171,172,173,174,175 };
static int swimm_straigth_right[]={ 186,187,188,189,190,191,192,193,194,195 };
static int swimm_down_right[]={ 176,177,178,179,180,181,182,183,184,185 };


static float getMaxAirFromDifficultyLevel(Config::DifficultyLevel level)
{
	switch (level) {
		case Config::DifficultyLevel::easy: return 45.0f;
		case Config::DifficultyLevel::normal: return 30.0f;
		case Config::DifficultyLevel::hard: return 20.0f;
	}
	return 30.0f;
}

Player::FlashLightPivot::FlashLightPivot(int x, int y, float angle)
{
	this->x=x;
	this->y=y;
	this->angle=angle;
}


Player::Player(Game* game)
{
	x=y=0;
	last_animation_sound_played=-1;
	sprite_resource=NULL;
	tiletype_resource=NULL;
	next_keycheck=0.0f;
	next_animation=0.0f;
	idle_timeout=0.0f;
	animation.setStaticFrame(27);
	points=0;
	health=100;
	lifes=3;
	godmode=false;
	this->game=game;
	dead=false;
	visible=true;
	autoWalk=false;
	waterSplashPlayed=false;
	maxair=getMaxAirFromDifficultyLevel(game->config.difficulty);
	air=maxair;
	flashlightOn=false;
	actionToggleCooldown=0.0f;

	particle_end_time=0.0f;
	next_particle_birth=0.0f;
	particle_reason=ParticleReason::None;
	color_modulation.setColor(255, 255, 255, 255);
	ambient_sound=NULL;
	expressionJump=false;
	hackingObject=NULL;
	hacking_end=0.0f;
	animation.setDefaultSpeed(0.056f);
	hackingState=0;
	airStart=0.0f;
	voice=NULL;
	voiceDamageCooldown=0.0f;
	time=ppl7::GetMicrotime();
	startIdle=time + 1.0f;
	nextIdleSpeech=0.0f;
	nextPhonetic=0.0f;
	greetingPlayed=true;
	talkie=true;
	last_fullspeed=0.0f;
	frame_rate_compensation=0.0f;
	petrifiedTimeout=0.0f;
	petrified=false;
	initFlashLightPivots();
	initFlashLight();
}

Player::~Player()
{
	if (ambient_sound) {
		getAudioPool().stopInstace(ambient_sound);
		delete ambient_sound;
		ambient_sound=NULL;
	}
	if (voice) {
		getAudioPool().stopInstace(voice);
		delete voice;
		voice=NULL;
	}
}

void Player::initFlashLightPivots()
{
	flashlight_pivots.clear();
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(314, FlashLightPivot(-45, -63, 90.7f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(315, FlashLightPivot(-47, -64, 92.1f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(316, FlashLightPivot(-53, -66, 95.8f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(317, FlashLightPivot(-50, -68, 96.5f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(318, FlashLightPivot(-46, -65, 93.6f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(319, FlashLightPivot(-47, -62, 91.4f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(320, FlashLightPivot(-44, -58, 85.7f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(321, FlashLightPivot(-43, -59, 86.4f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(322, FlashLightPivot(-41, -59, 87.1f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(323, FlashLightPivot(44, -63, 268.6f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(324, FlashLightPivot(47, -64, 266.4f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(325, FlashLightPivot(49, -65, 265.7f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(326, FlashLightPivot(48, -65, 264.9f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(327, FlashLightPivot(44, -63, 267.9f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(328, FlashLightPivot(45, -62, 269.3f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(329, FlashLightPivot(46, -60, 272.2f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(330, FlashLightPivot(43, -59, 273.6f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(331, FlashLightPivot(44, -62, 270.0f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(332, FlashLightPivot(44, -63, 269.3f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(333, FlashLightPivot(6, -66, 264.1f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(334, FlashLightPivot(-38, -62, 243.4f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(335, FlashLightPivot(-58, -62, 90.0f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(336, FlashLightPivot(-44, -62, 89.3f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(337, FlashLightPivot(-57, -61, 90.0f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(338, FlashLightPivot(-38, -61, 296.6f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(339, FlashLightPivot(3, -61, 271.0f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(340, FlashLightPivot(42, -61, 270.7f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(341, FlashLightPivot(-38, -61, -1.0f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(342, FlashLightPivot(36, -62, 63.4f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(343, FlashLightPivot(56, -62, 271.0f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(344, FlashLightPivot(36, -62, 90.0f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(345, FlashLightPivot(-7, -64, 93.9f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(346, FlashLightPivot(-45, -63, 90.7f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(347, FlashLightPivot(-4, -61, 88.0f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(348, FlashLightPivot(36, -62, 135.0f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(349, FlashLightPivot(58, -65, 264.8f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(350, FlashLightPivot(43, -63, 269.3f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(351, FlashLightPivot(-44, -62, 90.0f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(352, FlashLightPivot(-44, -54, 80.7f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(353, FlashLightPivot(45, -56, 277.9f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(354, FlashLightPivot(-48, -67, 95.8f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(355, FlashLightPivot(49, -71, 260.0f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(356, FlashLightPivot(-37, -76, 187.4f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(357, FlashLightPivot(-70, -11, 100.0f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(358, FlashLightPivot(-70, -11, 100.0f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(359, FlashLightPivot(-69, -11, 97.9f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(360, FlashLightPivot(-68, -12, 96.5f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(361, FlashLightPivot(-69, -11, 98.6f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(362, FlashLightPivot(-70, -10, 99.3f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(363, FlashLightPivot(-70, -12, 98.7f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(364, FlashLightPivot(-67, -11, 95.7f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(365, FlashLightPivot(-70, -10, 99.3f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(366, FlashLightPivot(60, -10, 260.7f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(367, FlashLightPivot(59, -10, 262.1f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(368, FlashLightPivot(60, -12, 261.4f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(369, FlashLightPivot(60, -13, 262.1f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(370, FlashLightPivot(61, -12, 259.8f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(371, FlashLightPivot(62, -13, 257.0f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(372, FlashLightPivot(62, -13, 258.4f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(373, FlashLightPivot(58, -10, 264.9f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(374, FlashLightPivot(60, -11, 262.0f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(375, FlashLightPivot(-44, -62, 90.0f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(376, FlashLightPivot(-44, -58, 85.7f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(377, FlashLightPivot(-39, -51, 75.4f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(378, FlashLightPivot(-41, -56, 81.4f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(379, FlashLightPivot(-43, -61, 89.3f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(380, FlashLightPivot(-51, -65, 93.6f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(381, FlashLightPivot(-58, -69, 98.6f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(382, FlashLightPivot(-52, -66, 95.7f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(383, FlashLightPivot(-44, -62, 90.0f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(384, FlashLightPivot(44, -63, 267.9f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(385, FlashLightPivot(45, -60, 271.4f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(386, FlashLightPivot(42, -52, 282.3f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(387, FlashLightPivot(43, -59, 273.6f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(388, FlashLightPivot(44, -63, 268.6f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(389, FlashLightPivot(49, -66, 265.0f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(390, FlashLightPivot(61, -75, 254.1f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(391, FlashLightPivot(51, -69, 262.1f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(392, FlashLightPivot(44, -63, 268.6f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(393, FlashLightPivot(-70, -11, 100.0f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(394, FlashLightPivot(-78, -12, 102.9f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(395, FlashLightPivot(-73, -10, 105.1f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(396, FlashLightPivot(-58, -11, 120.3f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(397, FlashLightPivot(-32, -11, 201.8f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(398, FlashLightPivot(12, -11, 251.6f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(399, FlashLightPivot(46, -9, 260.4f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(400, FlashLightPivot(60, -10, 260.7f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(401, FlashLightPivot(60, -10, 260.7f)));

	// Spider Web
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(433, FlashLightPivot(-38, -61, -1.0f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(434, FlashLightPivot(-38, -61, -1.0f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(435, FlashLightPivot(-38, -61, -1.0f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(436, FlashLightPivot(-38, -61, -1.0f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(437, FlashLightPivot(-38, -61, -1.0f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(438, FlashLightPivot(-38, -61, -1.0f)));

	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(439, FlashLightPivot(-45, -63, 90.7f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(440, FlashLightPivot(-45, -63, 90.7f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(441, FlashLightPivot(-45, -63, 90.7f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(442, FlashLightPivot(-45, -63, 90.7f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(443, FlashLightPivot(-45, -63, 90.7f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(444, FlashLightPivot(-45, -63, 90.7f)));

	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(445, FlashLightPivot(44, -63, 268.6f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(446, FlashLightPivot(44, -63, 268.6f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(447, FlashLightPivot(44, -63, 268.6f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(448, FlashLightPivot(44, -63, 268.6f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(449, FlashLightPivot(44, -63, 268.6f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(450, FlashLightPivot(44, -63, 268.6f)));

	// Sliding right 317x130  Pivot: -px 256 -py 195 22°  => if (movement == Slide) p.y+=35;
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(451, FlashLightPivot(61, -30, 248.0f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(452, FlashLightPivot(61, -30, 248.0f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(453, FlashLightPivot(61, -30, 248.0f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(454, FlashLightPivot(61, -30, 248.0f)));

	// Sliding left 198 x 130
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(455, FlashLightPivot(-58, -31, 111.0f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(456, FlashLightPivot(-58, -31, 111.0f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(457, FlashLightPivot(-58, -31, 111.0f)));
	flashlight_pivots.insert(std::pair<int, FlashLightPivot>(458, FlashLightPivot(-58, -31, 111.0f)));

}

void Player::resetState()
{
	animation.resetSpeed();
	last_animation_sound_played=-1;
	points=0;
	talkie=true;
	health=100;
	lifes=3;
	godmode=false;
	dead=false;
	visible=true;
	air=maxair;
	last_aircheck=0;
	waterSplashPlayed=false;
	stand();
	particle_end_time=0.0f;
	next_particle_birth=0.0f;
	particle_reason=ParticleReason::None;
	Inventory.clear();
	object_counter.clear();
	SpecialObjects.clear();
	spokenText.clear();
	expressionJump=false;
	hackingObject=NULL;
	hacking_end=0.0f;
	hackingState=0;
	color_modulation.setColor(255, 255, 255, 255);
	airStart=0.0f;
	nextIdleSpeech=0.0f;
	nextPhonetic=0.0f;
	startIdle=ppl7::GetMicrotime() + 5.0f;
	if (ambient_sound) {
		ambient_sound->setAutoDelete(true);
		ambient_sound->fadeout(2.0f);
		ambient_sound=NULL;
	}
	if (voice) {
		getAudioPool().stopInstace(voice);
		delete voice;
		voice=NULL;
	}
}

void Player::resetLevelObjects()
{
	last_animation_sound_played=-1;
	animation.resetSpeed();
	Inventory.clear();
	object_counter.clear();
	spokenText.clear();
	air=maxair;
	last_aircheck=0;
	SpecialObjects.clear();
	hackingObject=NULL;
	hacking_end=0.0f;
	hackingState=0;
}



void Player::setVisible(bool flag)
{
	visible=flag;
}



ppl7::grafix::PointF Player::position() const
{
	return ppl7::grafix::PointF(x, y);
}


void Player::setZeroVelocity()
{
	velocity_move.x=0;
	velocity_move.y=0;
}

void Player::setSpriteResource(const SpriteTexture& resource)
{
	sprite_resource=&resource;
}

void Player::setTileTypeResource(const SpriteTexture& resource)
{
	tiletype_resource=&resource;
}

void Player::setGodMode(bool enabled)
{
	godmode=enabled;
}

void Player::move(int x, int y)
{
	this->x=x;
	this->y=y;
}


void Player::draw(SDL_Renderer* renderer, const ppl7::grafix::Rect& viewport, const ppl7::grafix::Point& worldcoords) const
{
	if (!visible) return;
	ppl7::grafix::Point p(x + viewport.x1 - worldcoords.x, y + viewport.y1 - worldcoords.y);
	if (movement == Slide) p.y+=35;
	int frame=animation.getFrame();
	if (flashlightOn) {
		if (frame >= 0 && frame <= 78) frame+=314;
		else if (frame >= 79 && frame <= 86) frame+=372; // 451
		else if (frame >= 305 && frame <= 313) frame+=88;
		else if (frame >= 415 && frame <= 432) frame+=18;
	}
	sprite_resource->draw(renderer, p.x, p.y + 1, frame, color_modulation);
}

void Player::initFlashLight()
{
	flashlight1.color.set(255, 255, 255, 255);
	flashlight1.sprite_no=2;
	flashlight1.scale_x=0.2f;
	flashlight1.scale_y=0.2f;
	flashlight1.plane=static_cast<int>(LightPlaneId::Player);
	flashlight1.playerPlane= static_cast<int>(LightPlayerPlaneMatrix::Player);
	flashlight1.has_lensflare=true;
	flashlight1.flare_intensity=255;
	flashlight1.flarePlane=static_cast<int>(LightPlayerPlaneMatrix::Player);

	flashlight2.color.set(255, 255, 255, 255);
	flashlight2.intensity=128;
	flashlight2.sprite_no=0;
	flashlight2.scale_x=0.6f;
	flashlight2.scale_y=0.6f;
	flashlight2.plane=static_cast<int>(LightPlaneId::Player);
	flashlight2.playerPlane=static_cast<int>(LightPlayerPlaneMatrix::Back) | static_cast<int>(LightPlayerPlaneMatrix::Player);

	flashlight3.color.set(255, 255, 255, 255);
	flashlight3.sprite_no=13;
	flashlight3.scale_x=0.8f;
	flashlight3.scale_y=1.0f;
	flashlight3.plane=static_cast<int>(LightPlaneId::Player);
	flashlight3.playerPlane=static_cast<int>(LightPlayerPlaneMatrix::Back) | static_cast<int>(LightPlayerPlaneMatrix::Player);


	flashlight2_ladder.color.set(255, 255, 255, 255);
	flashlight2_ladder.intensity=255;
	flashlight2_ladder.sprite_no=2;
	flashlight2_ladder.scale_x=1.0f;
	flashlight2_ladder.scale_y=1.0f;
	flashlight2_ladder.plane=static_cast<int>(LightPlaneId::Player);
	flashlight2_ladder.playerPlane=static_cast<int>(LightPlayerPlaneMatrix::Back);
}

void Player::addFlashlightToLightSystem(LightSystem& lights)
{
	if (!flashlightOn) return;
	int frame=animation.getFrame();
	if (frame == 28 || (frame >= 91 && frame <= 101)) { // stand back or climb animation
		frame=342;

	} else if (frame >= 0 && frame <= 78) frame+=314;
	else if (frame >= 79 && frame <= 86) frame+=372; // 451
	else if (frame >= 305 && frame <= 313) frame+=88;
	else if (frame >= 415 && frame <= 432) frame+=18;
	//if ((frame >= 314 && frame <= 401) || (frame>=433 && frame<=450) ) {
	std::map<int, FlashLightPivot>::const_iterator it;
	ppl7::grafix::Point pf(x, y);
	it=flashlight_pivots.find(frame);
	if (it != flashlight_pivots.end()) {
		pf.x+=it->second.x;
		pf.y+=it->second.y;
		flashlight1.x=pf.x;
		flashlight1.y=pf.y - 1;
		flashlight2.x=pf.x;
		flashlight2.y=pf.y + 1;
		if (it->second.angle > 0.0f && frame != 342) {
			flashlight3.x=pf.x;
			flashlight3.y=pf.y;
			flashlight3.angle=it->second.angle;
			lights.addObjectLight(&flashlight3);
			if (it->second.angle < 180) flashlight1.x -= 8;
			if (it->second.angle > 180) flashlight1.x += 8;
		}
		if (frame == 342) {
			flashlight2_ladder.x=pf.x;
			flashlight2_ladder.y=pf.y + 1;

			flashlight1.has_lensflare=false;
			lights.addObjectLight(&flashlight2_ladder);
		} else {
			flashlight1.has_lensflare=true;
			lights.addObjectLight(&flashlight2);
		}
		lights.addObjectLight(&flashlight1);


	} else {
		ppl7::PrintDebug("tracking for frame not found: %d\n", frame);
	}

//} else {
//	ppl7::PrintDebug("frame out of range: %d\n", frame);
//}

}

void Player::drawCollision(SDL_Renderer* renderer, const ppl7::grafix::Rect& viewport, const ppl7::grafix::Point& worldcoords) const
{
	ppl7::grafix::Point p(x + viewport.x1 - worldcoords.x, y + viewport.y1 - worldcoords.y);
	if (tiletype_resource) {
		for (int cy=0;cy < 6;cy++) {
			for (int cx=0;cx < 6;cx++) {
				tiletype_resource->draw(renderer, p.x - (TILE_WIDTH * 3) + (cx * TILE_WIDTH), p.y - (5 * TILE_HEIGHT) + (cy * TILE_HEIGHT), collision_matrix[cx][cy]);
				//ppl7::PrintDebugTime("cx:cy %d:%d = %d\n", cx, cy, collision_matrix[cx][cy]);
			}
		}
	}
	tiletype_resource->draw(renderer, p.x + 100, p.y - 32, collision_at_pivoty[0]);
	tiletype_resource->draw(renderer, p.x + 100, p.y, collision_at_pivoty[1]);
	tiletype_resource->draw(renderer, p.x + 100, p.y + 32, collision_at_pivoty[2]);
}


void Player::turn(PlayerOrientation target)
{
	movement=Turn;
	turnTarget=target;
	if (orientation == Front) {
		if (target == Left) {
			animation.start(turn_from_mid_to_left, sizeof(turn_from_mid_to_left) / sizeof(int), false, 0);
		} else {
			animation.start(turn_from_mid_to_right, sizeof(turn_from_mid_to_right) / sizeof(int), false, 9);
		}
	} else if (orientation == Left) {
		if (target == Right) {
			animation.start(turn_from_left_to_right, sizeof(turn_from_left_to_right) / sizeof(int), false, 9);
		} else if (target == Front) {
			animation.start(turn_from_left_to_mid, sizeof(turn_from_left_to_mid) / sizeof(int), false, 27);
		}
	} else if (orientation == Right) {
		if (target == Left) {
			animation.start(turn_from_right_to_left, sizeof(turn_from_right_to_left) / sizeof(int), false, 0);
		} else if (target == Front) {
			animation.start(turn_from_right_to_mid, sizeof(turn_from_right_to_mid) / sizeof(int), false, 27);
		}
	}
}

void Player::crawlTurn(PlayerOrientation target)
{
	movement=CrawlTurn;
	turnTarget=target;
	if (orientation == Front) {
		if (target == Left) {
			animation.startSequence(309, 305, false, 43);
		} else {
			animation.startSequence(309, 313, false, 52);
		}
	} else if (orientation == Left) {
		if (target == Left) {
			movement=Crouch;
		} else {
			animation.startSequence(305, 313, false, 52);
		}
	} else if (orientation == Right) {
		if (target == Left) {
			animation.startSequence(313, 305, false, 43);
		} else {
			movement=Crouch;
		}
	}
}

Player::Keys Player::getKeyboardMatrix(const Uint8* state)
{
	if (state == NULL) state=SDL_GetKeyboardState(NULL);
	Keys k;
	k.matrix=0;
	k.velocity_x=0;
	k.velocity_y=0;
	if (petrified) return k;

	if (state[SDL_SCANCODE_LEFT]) k.matrix|=KeyboardKeys::Left;
	if (state[SDL_SCANCODE_J] || state[SDL_SCANCODE_A]) k.matrix|=KeyboardKeys::Left;
	if (state[SDL_SCANCODE_RIGHT]) k.matrix|=KeyboardKeys::Right;
	if (state[SDL_SCANCODE_L] || state[SDL_SCANCODE_D]) k.matrix|=KeyboardKeys::Right;
	if (state[SDL_SCANCODE_UP]) k.matrix|=KeyboardKeys::Up;
	if (state[SDL_SCANCODE_I] || state[SDL_SCANCODE_W]) k.matrix|=KeyboardKeys::Up;
	if (state[SDL_SCANCODE_DOWN]) k.matrix|=KeyboardKeys::Down;
	if (state[SDL_SCANCODE_LCTRL]) k.matrix|=KeyboardKeys::Crouch;
	if (state[SDL_SCANCODE_K] || state[SDL_SCANCODE_S]) k.matrix|=KeyboardKeys::Down;
	if (state[SDL_SCANCODE_LSHIFT] || state[SDL_SCANCODE_RSHIFT]) k.matrix|=KeyboardKeys::Shift;
	if (state[SDL_SCANCODE_E] || state[SDL_SCANCODE_O]) k.matrix|=KeyboardKeys::Action;
	if (state[SDL_SCANCODE_F]) k.matrix|=KeyboardKeys::Flashlight;

	//ppl7::PrintDebugTime("keys: %4d, velocity x: %5d, velocity y: %5d --- ", k.matrix, k.velocity_x, k.velocity_y);


	if (game->controller.isOpen()) {
		GameController& gc=game->controller;
		k.velocity_x=gc.getAxisState(gc.mapping.getSDLAxis(GameControllerMapping::Axis::Walk));
		if (k.velocity_x > 0) k.matrix|=KeyboardKeys::Right;
		if (k.velocity_x > 20000) k.matrix|=KeyboardKeys::Shift;
		if (k.velocity_x < 0) k.matrix|=KeyboardKeys::Left;
		if (k.velocity_x < -20000) k.matrix|=KeyboardKeys::Shift;

		k.velocity_y=gc.getAxisState(gc.mapping.getSDLAxis(GameControllerMapping::Axis::Jump));
		//if (k.velocity_y > 0) k.matrix|=KeyboardKeys::Down;
		if (k.velocity_y > 16384) k.matrix|=KeyboardKeys::Down | KeyboardKeys::Shift;
		//if (k.velocity_y < 0) k.matrix|=KeyboardKeys::Up;
		if (k.velocity_y < -16384) k.matrix|=KeyboardKeys::Up | KeyboardKeys::Shift;

		if (gc.getAxisState(gc.mapping.getSDLAxis(GameControllerMapping::Axis::Crouch)) > 0) {
			k.matrix|=KeyboardKeys::Crouch;
		}

		if (gc.getButtonState(gc.mapping.getSDLButton(GameControllerMapping::Button::Action))) k.matrix|=KeyboardKeys::Action;
		if (gc.getButtonState(gc.mapping.getSDLButton(GameControllerMapping::Button::Flashlight))) k.matrix|=KeyboardKeys::Flashlight;
		if (gc.getButtonState(gc.mapping.getSDLButton(GameControllerMapping::Button::Crouch))) k.matrix|=KeyboardKeys::Crouch;

		if (gc.getButtonState(gc.mapping.getSDLButton(GameControllerMapping::Button::Jump))) k.matrix|=KeyboardKeys::Up | KeyboardKeys::Shift;
		if (gc.getButtonState(gc.mapping.getSDLButton(GameControllerMapping::Button::MenuLeft))) k.matrix|=KeyboardKeys::Left | KeyboardKeys::Shift;
		if (gc.getButtonState(gc.mapping.getSDLButton(GameControllerMapping::Button::MenuRight))) k.matrix|=KeyboardKeys::Right | KeyboardKeys::Shift;
		if (gc.getButtonState(gc.mapping.getSDLButton(GameControllerMapping::Button::MenuUp))) k.matrix|=KeyboardKeys::Up | KeyboardKeys::Shift;
		if (gc.getButtonState(gc.mapping.getSDLButton(GameControllerMapping::Button::MenuDown))) k.matrix|=KeyboardKeys::Down | KeyboardKeys::Shift;


	}
	//ppl7::PrintDebugTime("keys: %4d, velocity x: %5d, velocity y: %5d\n", k.matrix, k.velocity_x, k.velocity_y);

	if (k.matrix == KeyboardKeys::Right && k.velocity_x > 0 && last_fullspeed > time - 0.5) k.matrix|=KeyboardKeys::Shift;
	if (k.matrix == KeyboardKeys::Left && k.velocity_x<0 && last_fullspeed>time - 0.5) k.matrix|=KeyboardKeys::Shift;
	if (k.matrix & KeyboardKeys::Shift) last_fullspeed=time;


	return k;
}

void Player::stand()
{
	if (petrified) return;
	waterSplashPlayed=false;
	movement=Stand;
	if (orientation == Left) animation.setStaticFrame(0);
	else if (orientation == Right) animation.setStaticFrame(9);
	else if (orientation == Front) animation.setStaticFrame(27);
	else if (orientation == Back) animation.setStaticFrame(28);
	idle_timeout=time + 4.0;
	startIdle=idle_timeout;
}

void Player::addPoints(int points)
{
	if (movement == Dead) return;
	this->points+=points;
}

void Player::addHealth(int points)
{
	if (movement == Dead) return;
	this->health+=points;
	if (health > 100) health=100;
}

void Player::addLife(int lifes)
{
	if (movement == Dead) return;
	this->lifes+=lifes;
}

void Player::countObject(int type)
{
	object_counter[type]++;
}


size_t Player::getObjectCount(int type) const
{
	std::map<int, size_t>::const_iterator it;
	it=object_counter.find(type);
	if (it != object_counter.end()) return (*it).second;
	return 0;
}

void Player::dropHealth(float points, HealthDropReason reason)
{

	if (movement == Dead) return;
	if (points == 0.0f) return;
	if (godmode) return;
	if (reason == HealthDropReason::FallingDeep && game->config.difficulty < Config::DifficultyLevel::normal) return;
	if (game->config.difficulty == Config::DifficultyLevel::easy) points*=0.5f;
	else if (game->config.difficulty == Config::DifficultyLevel::hard) points*=2.0f;

	if (orientation == Front && movement == Stand) {
		if (animation.getFrame() != 297) animation.setStaticFrame(297);
	}

	//game->controller.rumbleTrigger(0xffff, 0xffff, 16);
	if (game->config.controller.use_rumble)	game->controller.rumble(0xffff, 0xffff, 100);
	health-=(points * frame_rate_compensation);
	if (health <= 0.0f && movement != Dead) {
		health=0;
		flashlightOn=false;
		movement=Dead;
		fallstart=0.0f;
		// we can play different animations for different reasons
		if (reason == FallingDeep) {
			animation.start(death_by_falling, sizeof(death_by_falling) / sizeof(int), false, 106);
		} else if (reason == Drowned) {
			animation.startSequence(260, 281, false, 281);
		} else if (reason == Smashed) {
			animation.startSequence(403, 408, false, 408);
			ppl7::grafix::Point p(x, y);
			getAudioPool().playOnce(AudioClip::stamper_squish, p, 1600, 1.0f);
			startEmittingParticles(time + 1.0f, ParticleReason::Smashed);
		} else if (reason == SmashedSideways) {
			animation.startSequence(409, 414, false, 414);
			ppl7::grafix::Point p(x, y);
			getAudioPool().playOnce(AudioClip::stamper_squish, p, 1600, 1.0f);
			startEmittingParticles(time + 1.0f, ParticleReason::Smashed);

		} else if (reason == Burned) {
			animation.startSequence(208, 216, false, 216);
			startEmittingParticles(time + 1.0f, ParticleReason::Burning);

		} else {
			animation.start(death_animation, sizeof(death_animation) / sizeof(int), false, 106);
		}
	} else if (health > 0.0f && movement != Dead) {
		if (time > voiceDamageCooldown) {
			int r=ppl7::rand(1, 4);
			switch (r) {
				case 1: speak(VoiceGeorge::aua1); break;
				case 2: speak(VoiceGeorge::aua2); break;
				case 3: speak(VoiceGeorge::aua3); break;
				default: speak(VoiceGeorge::aua4); break;
			}
			voiceDamageCooldown=time + ppl7::randf(0.0f, 4.0f);
		}
	}

}

void Player::addInventory(int object_id, const Decker::Objects::Representation& repr)
{
	Inventory.insert(std::pair<int, Decker::Objects::Representation>(object_id, repr));

}

bool Player::isInInventory(int object_id) const
{
	if (object_id > 0) {
		std::map<int, Decker::Objects::Representation>::const_iterator it;
		it=Inventory.find(object_id);
		if (it != Inventory.end()) return true;
	}
	return false;
}

bool Player::isDead() const
{
	if (movement == Dead) return true;
	return dead;
}

void Player::setSavePoint(const ppl7::grafix::Point& p)
{
	lastSavePoint=p;
}

void Player::setStandingOnObject(Decker::Objects::Object* object)
{
	player_stands_on_object=object;
}

void Player::dropLifeAndResetToLastSavePoint()
{
	dead=false;
	lifes--;
	health=100.0f;
	x=lastSavePoint.x;
	y=lastSavePoint.y;
	color_modulation.setColor(255, 255, 255, 255);
	stand();
}

void Player::addAir(float seconds)
{
	air+=seconds;
	if (air > maxair) air=maxair;
}

void Player::handleDiving(double time, const TileTypePlane& world, Decker::Objects::ObjectSystem* objects, float frame_rate_compensation)
{
	AudioPool& ap=getAudioPool();
	if (isDiving()) {
		if (ambient_sound != NULL && ambient_playing != AudioClip::underwaterloop1) {
			ambient_sound->setAutoDelete(true);
			ambient_sound->fadeout(1.0f);
			ambient_sound=NULL;
		}
		if (!ambient_sound) {
			ambient_sound=ap.getInstance(AudioClip::underwaterloop1, AudioClass::Ambience);
			ambient_sound->setLoop(true);
			ambient_sound->setVolume(0.8);
			ap.playInstance(ambient_sound);
			ambient_playing=AudioClip::underwaterloop1;

		}
		if (last_aircheck > 0.0f) {
			if (air > 0.0f) air-=time - last_aircheck;
			if (air < 0.0f) air=0.0f;
			if (air <= 0.0f) {
				dropHealth(0.5f * frame_rate_compensation, HealthDropReason::Drowned);
				if (health <= 0) startEmittingParticles(time + 1.0f, ParticleReason::Drowned);
			}
		}
	} else {
		if (ambient_sound != NULL && ambient_playing == AudioClip::underwaterloop1) {
			ambient_sound->setAutoDelete(true);
			ambient_sound->fadeout(2.0f);
			ambient_sound=NULL;
		}
		if (air < maxair) air+=0.08333333 * frame_rate_compensation;
		if (air > maxair) air=maxair;
	}
	last_aircheck=time;
}

static void play_step(AudioPool& ap)
{
	int r=ppl7::rand(1, 5);
	switch (r) {
		case 1: ap.playOnce(AudioClip::george_step1, 0.5f); break;
		case 2: ap.playOnce(AudioClip::george_step2, 0.5f); break;
		case 3: ap.playOnce(AudioClip::george_step3, 0.5f); break;
		case 4: ap.playOnce(AudioClip::george_step4, 0.5f); break;
		case 5: ap.playOnce(AudioClip::george_step5, 0.5f); break;
		default: ap.playOnce(AudioClip::george_step1, 0.5f); break;
	}
}

static void play_ladder(AudioPool& ap)
{
	int r=ppl7::rand(1, 7);
	switch (r) {
		case 1: ap.playOnce(AudioClip::ladder_step1, 0.5f); break;
		case 2: ap.playOnce(AudioClip::ladder_step2, 0.5f); break;
		case 3: ap.playOnce(AudioClip::ladder_step3, 0.5f); break;
		case 4: ap.playOnce(AudioClip::ladder_step4, 0.5f); break;
		case 5: ap.playOnce(AudioClip::ladder_step5, 0.5f); break;
		case 6: ap.playOnce(AudioClip::ladder_step6, 0.5f); break;
		default: ap.playOnce(AudioClip::ladder_step7, 0.5f); break;
	}
}




void Player::playSoundOnAnimationSprite()
{
	int sprite=animation.getFrame();
	if (sprite == last_animation_sound_played) return;
	last_animation_sound_played=sprite;
	AudioPool& ap=getAudioPool();

	if (sprite == 245 || sprite == 224)  ap.playOnce(AudioClip::hackstone, 1.0f);
	if (sprite == 3 || sprite == 7 || sprite == 12 || sprite == 16 || sprite == 64 || sprite == 68
		|| sprite == 73 || sprite == 77) play_step(ap);
	if (movement == ClimbUp && (sprite == 91 || sprite == 96)) play_ladder(ap);
	if (movement == ClimbDown && (sprite == 101 || sprite == 96)) play_ladder(ap);


}


void Player::update(double time, const TileTypePlane& world, Decker::Objects::ObjectSystem* objects, float frame_rate_compensation)
{
	if (particle_reason != ParticleReason::None && particle_end_time > time) emmitParticles(time);
	this->time=time;
	this->frame_rate_compensation=frame_rate_compensation;
	if (time > next_animation) {
		next_animation=time + animation.speed();
		animation.update();
		if (phonetics.notEmpty()) playPhonetics();
	}
	playSoundOnAnimationSprite();
	if (petrified == true && petrifiedTimeout < time) {
		petrified=false;
		airStart=0.0f;
		animation.resetSpeed();
		stand();
		return;
	}
	if ((isSwimming() || isDiving()) && flashlightOn == true) {
		toggleFlashlight();
	}
	const Uint8* state = SDL_GetKeyboardState(NULL);
	keys=getKeyboardMatrix(state);

	maxair=getMaxAirFromDifficultyLevel(game->config.difficulty);
	if (voice) voice->setPositional(ppl7::grafix::Point(x, y), 1600);
	if (movement == Dead) {
		if (animation.isFinished()) {
			dead=true;
		}
		return;
	}
	if (hackingInProgress()) return;
	handleDiving(time, world, objects, frame_rate_compensation);

	if (dead) return;

	AudioPool& ap=getAudioPool();
	if (movement == Jump || movement == Falling || movement == Slide) {
		if (airStart == 0) {
			airStart=time;
			if (ppl7::rand(0, 2) == 0)	speak(static_cast<VoiceGeorge::Id>(ppl7::rand(0, 4) + static_cast<int>(VoiceGeorge::hepp1)), 0.1f);

		}
	} else if (airStart > 0.0f) {
		double volume=(time - airStart);
		if (volume > 1.0f) volume=1.0f;
		airStart=0.0f;
		ap.playOnce(AudioClip::george_jump_land, volume);
	}

	dropHealth(detectFallingDamage(time, frame_rate_compensation), HealthDropReason::FallingDeep);
	updateMovement(frame_rate_compensation);
	player_stands_on_object=NULL;
	checkCollisionWithObjects(objects, frame_rate_compensation);
	if (petrified) keys=getKeyboardMatrix(state);
	if (movement == Hacking) return;
	if (movement == Dead) return;
	checkCollisionWithWorld(world);
	if (autoWalk) return;
	//PlayerMovement last_movement=movement;
	if (updatePhysics(world, frame_rate_compensation)) {
		//ppl7::PrintDebugTime("update Physics movement=%d\n", movement);
		/*
		if (last_movement == PlayerMovement::Falling && isSwimming()) {
			//ppl7::PrintDebugTime("SPLASH!\n");
			splashIntoWater(gravity);
		}
		*/
		if (!petrified) {
			if (movement == Slide && orientation == Left) {
				animation.start(slide_left, sizeof(slide_left) / sizeof(int), false, 86);
			} else if (movement == Slide && orientation == Right) {
				animation.start(slide_right, sizeof(slide_right) / sizeof(int), false, 82);
			} else if (movement == Swim && orientation == Left) {
				animation.start(swimm_inplace_left, sizeof(swimm_inplace_left) / sizeof(int), true, 106);
			} else if (movement == Swim && orientation == Right) {
				animation.start(swimm_inplace_right, sizeof(swimm_inplace_right) / sizeof(int), true, 106);
			} else if (movement == Swim) {
				animation.start(swimm_inplace_front, sizeof(swimm_inplace_front) / sizeof(int), true, 106);
			}
		}
	}
	//ppl7::PrintDebugTime("updatePhysics, gravity: %0.3f, acc y: %0.3f, velo y: %0.3f\n", gravity, acceleration.y, velocity_move.y);
	if (collision_matrix[2][4] == TileType::Water || collision_matrix[3][4] == TileType::Water) {
		if (!waterSplashPlayed && gravity > 0.0f) {
			waterSplashPlayed=true;
			splashIntoWater(gravity);
		} else if (!waterSplashPlayed && movement == Slide) {
			waterSplashPlayed=true;
			splashIntoWater(16.0f);
		}
	} else {
		waterSplashPlayed=false;
	}

	if (movement == Swim || movement == SwimStraight || movement == SwimUp || movement == SwimDown) {
		handleKeyboardWhileSwimming(time, world, objects, frame_rate_compensation);

	} else if (movement == Crawling || movement == Crouch) {
		handleKeyboardWhileCrawling(time, world, objects, frame_rate_compensation);

	}



	//ppl7::PrintDebugTime("gravity: %0.3f, velocity_move x: %0.3f, y: %0.3f, acceleration_jump: %0.3f\n",
	//	gravity, velocity_move.x, velocity_move.y, acceleration_jump);

	x+=velocity_move.x;
	y+=velocity_move.y + gravity;

	if (petrified) return;

	if (movement == Turn) {
		if (!animation.isFinished()) return;
		//printf ("debug 2\n");
		movement=Stand;
		orientation=turnTarget;
		startIdle=time;
		velocity_move.stop();
		//printf("Turn done, movement=%d, orientation=%d\n", (int)movement, (int)orientation);
	} else if (movement == CrawlTurn) {
		if (!animation.isFinished()) return;
		movement=Crouch;
		orientation=turnTarget;
		startIdle=time;
		velocity_move.stop();

	}
	if (movement == Slide || movement == Dead) {
		return;
	}
	if (expressionJump == true) {
		if (movement != Stand) return;
		expressionJump=false;
	}
	if (movement == Jump || movement == Falling) {
		handleKeyboardWhileJumpOrFalling(time, world, objects, frame_rate_compensation);
		return;
	}


	if (keys.matrix & KeyboardKeys::Flashlight) {
		toggleFlashlight();
	} else if (keys.matrix & KeyboardKeys::Action) {
		checkActivationOfObjectsInRange(objects);
		if (movement == Hacking) return;
	}


	if (movement == Swim || movement == SwimStraight || movement == SwimUp || movement == SwimDown || movement == Crawling || movement == Crouch || movement == CrawlTurn) {
		//handleKeyboardWhileSwimming(time, world, objects, frame_rate_compensation);
		return;
	}

	acceleration_jump_sideways=0;

	//ppl7::PrintDebugTime("keys matrix: %d\n", keys.matrix);

	if (keys.matrix == KeyboardKeys::Left) {
		if (orientation != Left) { turn(Left); return; }
		if (movement != Walk) {
			movement=Walk;
			animation.start(walk_cycle_left, sizeof(walk_cycle_left) / sizeof(int), true, 0);
		}
	} else if (keys.matrix == (KeyboardKeys::Left | KeyboardKeys::Shift)) {
		if (movement != Run || orientation != Left) {
			movement=Run;
			orientation=Left;
			animation.start(run_cycle_left, sizeof(run_cycle_left) / sizeof(int), true, 0);
		}
	} else if (keys.matrix == KeyboardKeys::Right) {
		if (orientation != Right) { turn(Right); return; }
		if (movement != Walk) {
			movement=Walk;
			animation.start(walk_cycle_right, sizeof(walk_cycle_right) / sizeof(int), true, 0);
		}
	} else if (keys.matrix == (KeyboardKeys::Right | KeyboardKeys::Shift)) {
		if (movement != Run || orientation != Right) {
			movement=Run;
			orientation=Right;
			animation.start(run_cycle_right, sizeof(run_cycle_right) / sizeof(int), true, 0);
		}
	} else if ((keys.matrix == KeyboardKeys::Up || keys.matrix == (KeyboardKeys::Up | KeyboardKeys::Shift)) && movement != Falling && movement != Jump) {
		if (collision_matrix[2][4] == TileType::Ladder || collision_matrix[3][4] == TileType::Ladder) {
			if (movement != ClimbUp) {
				movement=ClimbUp;
				orientation=Back;
				animation.start(climb_up_cycle, sizeof(climb_up_cycle) / sizeof(int), true, 0);
				animation.setSpeed(0.03f);
			}
		} else {
			if (movement != Jump) {
				movement=Jump;
				if (keys.matrix & KeyboardKeys::Shift) {
					jump_climax=time + 0.45f;
					acceleration_jump=2.0f * frame_rate_compensation;
					acceleration_jump_sideways=0;
				} else {
					jump_climax=time + 0.3f;
					acceleration_jump=0.3f * frame_rate_compensation;
					acceleration_jump_sideways=0;
				}

				if (orientation == Front) animation.setStaticFrame(42);
				else if (orientation == Left) animation.setStaticFrame(40);
				else if (orientation == Right) animation.setStaticFrame(41);
				else animation.setStaticFrame(28);
			}
		}
	} else if ((keys.matrix & KeyboardKeys::JumpLeft) == KeyboardKeys::JumpLeft) {
		movement=Jump;
		orientation=Left;
		if (keys.matrix & KeyboardKeys::Shift) {
			jump_climax=time + 0.45f;
			acceleration_jump=2.0f * frame_rate_compensation;
			acceleration_jump_sideways=-6;

		} else {
			jump_climax=time + 0.3f;
			acceleration_jump=0.3f * frame_rate_compensation;
			acceleration_jump_sideways=-2;
		}
		velocity_move.x=acceleration_jump_sideways * frame_rate_compensation;
		animation.setStaticFrame(38);
	} else if ((keys.matrix & KeyboardKeys::JumpRight) == KeyboardKeys::JumpRight) {
		movement=Jump;
		orientation=Right;
		if (keys.matrix & KeyboardKeys::Shift) {
			jump_climax=time + 0.45f;
			acceleration_jump=2.0f * frame_rate_compensation;
			acceleration_jump_sideways=6.0f;
			velocity_move.x=8 * frame_rate_compensation;
		} else {
			jump_climax=time + 0.3f;
			acceleration_jump=0.3f * frame_rate_compensation;
			acceleration_jump_sideways=2.0f;
		}
		velocity_move.x=acceleration_jump_sideways * frame_rate_compensation;
		animation.setStaticFrame(39);
	} else if (keys.matrix == KeyboardKeys::Down || keys.matrix == (KeyboardKeys::Down | KeyboardKeys::Shift)) {
		//ppl7::PrintDebugTime("down\n");
		if (collision_matrix[2][4] == TileType::Ladder || collision_matrix[3][4] == TileType::Ladder
			|| collision_matrix[2][5] == TileType::Ladder || collision_matrix[3][5] == TileType::Ladder) {
			if (collision_matrix[2][5] != TileType::Blocking && collision_matrix[3][5] != TileType::Blocking) {
				if (movement != ClimbDown) {
					//printf ("climb down\n");
					movement=ClimbDown;
					orientation=Back;
					animation.start(climb_down_cycle, sizeof(climb_down_cycle) / sizeof(int), true, 0);
					animation.setSpeed(0.03f);
				}
			}
		}
	} else if ((keys.matrix & KeyboardKeys::Crouch) && (movement != Crouch && movement != Crawling && movement != CrawlTurn)) {
		if (collision_matrix[2][5] == TileType::Blocking || collision_matrix[3][5] == TileType::Blocking
			|| collision_matrix[2][5] == TileType::BlockFromTop || collision_matrix[3][5] == TileType::BlockFromTop) {
			movement=Crouch;
			if (orientation == Left) animation.setStaticFrame(43);
			else if (orientation == Right) animation.setStaticFrame(52);
			else animation.setStaticFrame(309);
			//ppl7::PrintDebugTime("crawling\n");
		}
	} else if (keys.matrix == (KeyboardKeys::Left) && movement == Jump) {
		if (!isCollisionLeft()) velocity_move.x=-2 * frame_rate_compensation;
	} else if (keys.matrix == (KeyboardKeys::Right) && movement == Jump) {
		if (!isCollisionLeft()) velocity_move.x=2 * frame_rate_compensation;
	} else if (keys.matrix == (KeyboardKeys::Left | KeyboardKeys::Shift) && movement == Jump) {
		if (!isCollisionLeft()) velocity_move.x=-8 * frame_rate_compensation;
	} else if (keys.matrix == (KeyboardKeys::Right | KeyboardKeys::Shift) && movement == Jump) {
		if (!isCollisionLeft()) velocity_move.x=8 * frame_rate_compensation;

	} else {
		if (movement != Stand && movement != Jump && movement != Falling) {
			//printf ("debug 1\n");
			stand();
		} else if (movement == Stand && time > idle_timeout && orientation != Front) {
			turn(Front);
			//startIdle=time;
		}
	}
	if (movement == Stand && orientation == Front && flashlightOn == false) idleJokes(time);
	else startIdle=time;
//}
}

void Player::handleKeyboardWhileJumpOrFalling(double time, const TileTypePlane& world, Decker::Objects::ObjectSystem* objects, float frame_rate_compensation)
{
	//const Uint8* state = SDL_GetKeyboardState(NULL);
	//Player::Keys keys=getKeyboardMatrix(state);
	if (movement == Jump) {
		if (!(keys.matrix & KeyboardKeys::Up)) {
			movement=Falling;
			return;
		}
		if ((keys.matrix & KeyboardKeys::Left) && velocity_move.x == 0) {
			if (acceleration_jump_sideways > -6.0f) acceleration_jump_sideways=-6.0f;
			velocity_move.x=acceleration_jump_sideways * frame_rate_compensation;
			if (keys.matrix & KeyboardKeys::Shift && acceleration_jump_sideways > -9.0f) acceleration_jump_sideways-=(0.2f * frame_rate_compensation);
			orientation=Left;
			animation.setStaticFrame(38);
		} else if ((keys.matrix & KeyboardKeys::Right) && velocity_move.x == 0) {
			if (acceleration_jump_sideways < 6.0f) acceleration_jump_sideways=6.0f;
			velocity_move.x=acceleration_jump_sideways * frame_rate_compensation;
			if (keys.matrix & KeyboardKeys::Shift && acceleration_jump_sideways < 9.0f) acceleration_jump_sideways+=(0.2f * frame_rate_compensation);
			orientation=Right;
			animation.setStaticFrame(39);
		}
	} else {
		if (keys.matrix & KeyboardKeys::Left) {
			if (acceleration_jump_sideways > -6.0f) acceleration_jump_sideways=-6.0f;
			velocity_move.x=acceleration_jump_sideways * frame_rate_compensation;
			if (keys.matrix & KeyboardKeys::Shift && acceleration_jump_sideways > -9.0f) acceleration_jump_sideways-=(0.2f * frame_rate_compensation);
			orientation=Left;
			animation.setStaticFrame(38);
		} else if (keys.matrix & KeyboardKeys::Right) {
			if (acceleration_jump_sideways < 6.0f) acceleration_jump_sideways=6.0f;
			velocity_move.x=acceleration_jump_sideways * frame_rate_compensation;
			if (keys.matrix & KeyboardKeys::Shift && acceleration_jump_sideways < 9.0f) acceleration_jump_sideways+=(0.2f * frame_rate_compensation);
			orientation=Right;
			animation.setStaticFrame(39);
		}
	}
	//ppl7::PrintDebugTime("acceleration_jump_sideways=%0.3f\n", acceleration_jump_sideways);
}

void Player::handleKeyboardWhileSwimming(double time, const TileTypePlane& world, Decker::Objects::ObjectSystem* objects, float frame_rate_compensation)
{
	//ppl7::PrintDebugTime("Player::handleKeyboardWhileSwimming: old movement: %s, ", (const char*)getState());
	//const Uint8* state = SDL_GetKeyboardState(NULL);
	//Player::Keys keys=getKeyboardMatrix(state);

	if (keys.matrix & KeyboardKeys::Up) {
		if (collision_matrix[2][2] != TileType::Water && collision_matrix[3][2] != TileType::Water && movement != Jump) {
			if (keys.matrix & KeyboardKeys::Left) {
				movement=Jump;
				orientation=Left;
				if (keys.matrix & KeyboardKeys::Shift) {
					jump_climax=time + 0.3f;
					acceleration_jump=2.0f * frame_rate_compensation;
					acceleration_jump_sideways=-6;
					moveOutOfWater(160.0f, 1.0f);
				} else {
					jump_climax=time + 0.2f;
					acceleration_jump=0.3f * frame_rate_compensation;
					acceleration_jump_sideways=-2;
					moveOutOfWater(160.0f, 0.5f);
				}
				velocity_move.x=acceleration_jump_sideways * frame_rate_compensation;
				animation.setStaticFrame(38);
				return;
			} else if (keys.matrix & KeyboardKeys::Right) {
				movement=Jump;
				orientation=Right;
				if (keys.matrix & KeyboardKeys::Shift) {
					jump_climax=time + 0.3f;
					acceleration_jump=2.0f * frame_rate_compensation;
					acceleration_jump_sideways=6.0f;
					moveOutOfWater(200.0f, 1.0f);
				} else {
					jump_climax=time + 0.2f;
					acceleration_jump=0.3f * frame_rate_compensation;
					acceleration_jump_sideways=2.0f;
					moveOutOfWater(200.0f, 0.5f);
				}
				velocity_move.x=acceleration_jump_sideways * frame_rate_compensation;
				animation.setStaticFrame(39);
				return;
			} else {
				movement=Jump;
				orientation=Back;
				if (keys.matrix & KeyboardKeys::Shift) {
					jump_climax=time + 0.3f;
					acceleration_jump=2.0f * frame_rate_compensation;
					acceleration_jump_sideways=0.0f;
					moveOutOfWater(180.0f, 1.0f);
				} else {
					jump_climax=time + 0.2f;
					acceleration_jump=0.3f * frame_rate_compensation;
					acceleration_jump_sideways=0.0f;
					moveOutOfWater(180.0f, 0.5f);
				}
				velocity_move.x=acceleration_jump_sideways * frame_rate_compensation;
				animation.setStaticFrame(42);
				return;

			}
		}
	}
	float speed=2.0f * frame_rate_compensation;
	if (keys.matrix & KeyboardKeys::Shift) {
		speed=4.0f * frame_rate_compensation;
		keys.matrix-=KeyboardKeys::Shift;
	}
	if (keys.matrix == KeyboardKeys::Up) {
		if (movement != Swim || orientation != Front) {
			movement=Swim;
			orientation=Front;
			animation.start(swimm_inplace_front, sizeof(swimm_inplace_front) / sizeof(int), true, 0);
		}
		velocity_move.y=-speed;
		velocity_move.x=0;
	} else if (keys.matrix == KeyboardKeys::Down) {
		if (movement != Swim || orientation != Front) {
			movement=Swim;
			orientation=Front;
			animation.start(swimm_inplace_front, sizeof(swimm_inplace_front) / sizeof(int), true, 0);
		}
		velocity_move.y=speed;
		velocity_move.x=0;
	} else if (keys.matrix == KeyboardKeys::Right && (collision_matrix[2][1] != TileType::Water && collision_matrix[3][1] != TileType::Water)) {
		if (movement != Swim || orientation != Right) {
			movement=Swim;
			orientation=Right;
			animation.start(swimm_up_right, sizeof(swimm_up_right) / sizeof(int), true, 0);
		}
		velocity_move.x=speed;
		velocity_move.y=0;
	} else if (keys.matrix == KeyboardKeys::Right) {
		if (movement != SwimStraight || orientation != Right) {
			movement=SwimStraight;
			orientation=Right;
			animation.start(swimm_straigth_right, sizeof(swimm_straigth_right) / sizeof(int), true, 0);
		}
		velocity_move.x=speed;
		velocity_move.y=0;
	} else if (keys.matrix == KeyboardKeys::Left && (collision_matrix[2][1] != TileType::Water && collision_matrix[3][1] != TileType::Water)) {
		if (movement != Swim || orientation != Left) {
			movement=Swim;
			orientation=Left;
			animation.start(swimm_up_left, sizeof(swimm_up_left) / sizeof(int), true, 0);
		}
		velocity_move.x=-speed;
		velocity_move.y=0;
	} else if (keys.matrix == KeyboardKeys::Left) {
		if (movement != SwimStraight || orientation != Left) {
			movement=SwimStraight;
			orientation=Left;
			animation.start(swimm_straight_left, sizeof(swimm_straight_left) / sizeof(int), true, 0);
		}
		velocity_move.x=-speed;
		velocity_move.y=0;
	} else if (keys.matrix == (KeyboardKeys::Left | KeyboardKeys::Up)) {
		if (movement != SwimUp || orientation != Left) {
			movement=SwimUp;
			orientation=Left;
			animation.start(swimm_up_left, sizeof(swimm_up_left) / sizeof(int), true, 0);
		}
		velocity_move.x=-speed;
		velocity_move.y=-speed;
	} else if (keys.matrix == (KeyboardKeys::Right | KeyboardKeys::Up)) {
		if (movement != SwimUp || orientation != Right) {
			movement=SwimUp;
			orientation=Right;
			animation.start(swimm_up_right, sizeof(swimm_up_right) / sizeof(int), true, 0);
		}
		velocity_move.x=speed;
		velocity_move.y=-speed;
	} else if (keys.matrix == (KeyboardKeys::Right | KeyboardKeys::Down)) {
		if (movement != SwimDown || orientation != Right) {
			movement=SwimDown;
			orientation=Right;
			animation.start(swimm_down_right, sizeof(swimm_down_right) / sizeof(int), true, 0);
		}
		velocity_move.x=speed;
		velocity_move.y=speed;
	} else if (keys.matrix == (KeyboardKeys::Left | KeyboardKeys::Down)) {
		if (movement != SwimDown || orientation != Left) {
			movement=SwimDown;
			orientation=Left;
			animation.start(swimm_down_left, sizeof(swimm_down_left) / sizeof(int), true, 0);
		}
		velocity_move.x=-speed;
		velocity_move.y=speed;

	}
	//printf(", new movement: %s\n", (const char*)getState());
	//fflush(stdout);

}

void Player::handleKeyboardWhileCrawling(double time, const TileTypePlane& world, Decker::Objects::ObjectSystem* objects, float frame_rate_compensation)
{
	if (movement == CrawlTurn) return;

	if (keys.matrix & KeyboardKeys::Up || ((keys.matrix & KeyboardKeys::Crouch) == 0 && (movement == Crouch || movement == Crawling))) {
		if (collision_matrix[2][0] != TileType::Blocking && collision_matrix[3][0] != TileType::Blocking
			&& collision_matrix[2][1] != TileType::Blocking && collision_matrix[3][1] != TileType::Blocking
			&& collision_matrix[2][2] != TileType::Blocking && collision_matrix[3][2] != TileType::Blocking
			) {
			stand();
			return;
		}
	} else if (keys.matrix & KeyboardKeys::Left) {
		if (orientation != Left) { crawlTurn(Left); return; }
		if (movement != Crawling) {
			movement=Crawling;
			orientation=Left;
			animation.startSequence(43, 51, true, 43);
		}
	} else if (keys.matrix & KeyboardKeys::Right) {
		if (orientation != Right) { crawlTurn(Right); return; }
		if (movement != Crawling) {
			movement=Crawling;
			orientation=Right;
			animation.startSequence(52, 60, true, 52);
		}
	} else {
		movement=Crouch;
		if (orientation == Left) animation.setStaticFrame(43);
		else if (orientation == Right) animation.setStaticFrame(52);
		else animation.setStaticFrame(309);
	}
}

void Player::checkCollisionWithWorld(const TileTypePlane& world)
{
	Physic::PlayerMovement new_movement=Physic::checkCollisionWithWorld(world, x, y);
	if (new_movement == Stand) stand();
	if (collision_type_count[TileType::Type::Speer] > 0) {
		if (checkCollisionMatrixBody(TileType::Type::Speer)) this->dropHealth(10);
	}
	if (collision_type_count[TileType::Type::Fire] > 0) {
		if (checkCollisionMatrixBody(TileType::Type::Fire)) this->dropHealth(10, Burned);

	}

}


void Player::checkCollisionWithObjects(Decker::Objects::ObjectSystem* objects, float frame_rate_compensation)
{
	// we try to find existing pixels inside the player boundary
	// to build a list with points we want to check against the
	// objects
	if (movement == Dead) return;
	collision_checkpoints.clear();
	collision_checkpoints.push_back(ppl7::grafix::Point(x, y));

	const ppl7::grafix::Drawable& draw=sprite_resource->getDrawable(animation.getFrame());
	ppl7::grafix::Rect boundary=sprite_resource->spriteBoundary(animation.getFrame(), 1.0f, x, y);

	if (draw.width()) {
		//ppl7::PrintDebugTime("boundary= %d:%d - %d:%d\n", boundary.x1, boundary.y1, boundary.x2, boundary.y2);
		int stepx=boundary.width() / 16;
		int stepy=boundary.height() / 16;
		for (int py=boundary.y1;py < boundary.y2;py+=stepx) {
			for (int px=boundary.x1;px < boundary.x2;px+=stepy) {
				ppl7::grafix::Color c=draw.getPixel(px - boundary.x1, py - boundary.y1);
				if (c.alpha() > 92) {
					collision_checkpoints.push_back(ppl7::grafix::Point(px, py));
				}
			}
		}
	}
	//printf ("check collision against %zd points:\n", checkpoints.size());

	//Decker::Objects::Object* object=objects->detectCollision(checkpoints);
	std::list<Decker::Objects::Object*> object_list;
	objects->detectCollision(collision_checkpoints, object_list);
	if (object_list.empty()) return;
	std::list<Decker::Objects::Object*>::iterator it;
	for (it=object_list.begin();it != object_list.end();++it) {
		Decker::Objects::Collision col(this, (*it), frame_rate_compensation);
		col.detect((*it), collision_checkpoints, *this);
		(*it)->handleCollision(this, col);
	}


	//printf ("Detected Collision with Object: %s, ID: %d\n",
	//		(const char*)object->typeName(), object->id);
	//const ppl7::grafix::Rect &bbi=col.bounding_box_intersection;
	//printf ("BoundingBox Player: %d:%d - %d:%d\n", bbp.x1, bbp.y1, bbp.x2, bbp.y2);
	//printf ("BoundingBox Object: %d:%d - %d:%d\n", bbo.x1, bbo.y1, bbo.x2, bbo.y2);
	//printf ("Intersection:       %d:%d - %d:%d\n", bbi.x1, bbi.y1, bbi.x2, bbi.y2);


}

const std::list<ppl7::grafix::Point>& Player::getCollisionCheckpoints() const
{
	return collision_checkpoints;
}

ppl7::grafix::Rect Player::getBoundingBox() const
{
	return sprite_resource->spriteBoundary(animation.getFrame(), 1.0f, x, y);

}

void Player::setAutoWalk(bool enabled)
{
	autoWalk=enabled;
	if (autoWalk) {
		visible=false;
		dead=false;
		godmode=true;
	}
}

bool Player::isAutoWalk() const
{
	return autoWalk;
}


void Player::splashIntoWater(float gravity)
{
	ParticleSystem* ps=GetParticleSystem();
	int new_particles=ppl7::rand(111, 192);
	gravity+=4.0f;
	float min_speed=6.226 * gravity / 18.0f;
	float max_speed=11.887 * gravity / 18.0f;
	float direction=0;
	if (velocity_move.x > 0) direction=15.0f;
	if (velocity_move.x < 0) direction=-15.0f;
	float min_dir=-15.283f + direction;
	float max_dir=+15.283f + direction;
	//ppl7::PrintDebugTime("SPLASH gravity=%0.3f, velocity.x=%0.3f\n", gravity, velocity_move.x);
	ppl7::grafix::PointF p(x, y + TILE_HEIGHT / 2);
	if (gravity > 16.0f) p.y+=TILE_HEIGHT;
	for (int i=0;i < new_particles;i++) {
		Particle* particle=new Particle();
		particle->birth_time=time;
		particle->death_time=randf(1.706, 0.387) + time;
		particle->p=getBirthPosition(p, EmitterType::Rectangle, ppl7::grafix::Size(68, 1), 0.000);
		particle->layer=Particle::Layer::BehindBricks;
		particle->weight=randf(0.340, 0.821);
		particle->gravity.setPoint(0.000, 0.415);
		particle->velocity=calculateVelocity(randf(min_speed, max_speed), 0.000 + randf(min_dir, max_dir));
		particle->scale=randf(0.300, 1.000);
		particle->color_mod.set(181, 187, 255, 255);
		particle->initAnimation(Particle::Type::RotatingParticleTransparent);
		ps->addParticle(particle);
	}
	AudioPool& audio=getAudioPool();
	AudioClip::Id id=AudioClip::watersplash1;
	switch (ppl7::rand(0, 4)) {
		case 0:
		case 1:
			id=AudioClip::watersplash1; break;
		case 2:
			id=AudioClip::watersplash2; break;
		case 3:
			id=AudioClip::watersplash3; break;
		case 4:
			id=AudioClip::watersplash4; break;

	}
	audio.playOnce(id, gravity / 21.0f);
}


void Player::moveOutOfWater(float angel, float speed)
{
	AudioPool& audio=getAudioPool();
	if (speed < 1.0f) audio.playOnce(AudioClip::water_pouring2, 0.5f);
	else audio.playOnce(AudioClip::water_pouring1, 0.5f);
	ppl7::grafix::PointF p(x, y - 2 * TILE_HEIGHT);

	ParticleSystem* ps=GetParticleSystem();
	ppl7::grafix::Size size(20, 40);
	if (speed >= 0.8) {
		p.y-=TILE_HEIGHT * 2;
		size.height=TILE_HEIGHT * 3;
	}

	int new_particles=ppl7::rand(130, 250);
	for (int i=0;i < new_particles;i++) {
		Particle* particle=new Particle();
		particle->birth_time=time;
		particle->death_time=randf(0.104, 0.670) + time;
		particle->p=getBirthPosition(p, EmitterType::Rectangle, size, 180.000);
		particle->layer=Particle::Layer::BehindPlayer;
		particle->weight=randf(0.142, 0.481);
		particle->gravity.setPoint(0.000, 0.208);
		particle->velocity=calculateVelocity(randf(1.887, 1.887), angel + randf(-30.0f, 30.0f));
		particle->scale=randf(0.300, 1.000);
		particle->color_mod.set(172, 199, 255, 255);
		particle->initAnimation(Particle::Type::RotatingParticleTransparent);
		ps->addParticle(particle);
	}
}

void Player::startEmittingParticles(double endtime, ParticleReason reason)
{
	particle_end_time=endtime;
	particle_reason=reason;
	next_particle_birth=0.0f;
}

void Player::jumpExpression()
{
	if (isDiving()) return;
	if (movement != Stand && movement != Turn && movement != Walk && movement != Run) return;
	expressionJump=true;
	movement=Jump;
	orientation=Front;
	turnTarget=Front;
	animation.setStaticFrame(42);
	jump_climax=time + 0.2f;
	acceleration_jump=2.0f;
	acceleration_jump_sideways=0;
	velocity_move.x=0;
	//velocity_move.y=0;
}

void Player::addSpecialObject(int type)
{
	SpecialObjects.insert(type);
}

bool Player::hasSpecialObject(int type) const
{
	std::set<int>::const_iterator it;
	it=SpecialObjects.find(type);
	if (it != SpecialObjects.end()) return true;
	return false;
}

void Player::startHacking(Decker::Objects::Object* object)
{
	if (hasSpecialObject(Decker::Objects::Type::Hammer) && hackingObject == NULL) {
		movement=Hacking;
		hackingObject=object;
		hacking_end=ppl7::GetMicrotime() + 2.0f;
		if (object->p.x < x) {
			//left
			orientation=Left;
			x=object->p.x + 96;
			animation.startSequence(218, 238, true, 218);
		} else {
			orientation=Right;
			x=object->p.x - 96;
			animation.startSequence(239, 259, true, 239);
		}
		next_animation=0.0f;
		hackingState=1;
		speak(5);
	}
}

bool Player::hackingInProgress()
{
	if (hackingObject != NULL) {
		movement=Hacking;
		animation.setSpeed(0.03f);
		if (time > hacking_end) {
			if (hackingState == 1) {
				Decker::Objects::BreakingWall* wall=static_cast<Decker::Objects::BreakingWall*>(hackingObject);
				wall->breakWall(this);
				hackingState=2;
				hacking_end=time + 0.8f;
				return true;
			} else if (hackingState == 2) {
				stand();
				hackingObject=NULL;
				hackingState=0;
				animation.resetSpeed();
			}
		} else {
			return true;
		}


	}
	return false;
}

void Player::checkActivationOfObjectsInRange(Decker::Objects::ObjectSystem* objectsystem)
{
	std::list<Decker::Objects::Object*>object_list;

	if (objectsystem->findObjectsInRange(position(), 200, object_list)) {
		//ppl7::PrintDebugTime("Found %zd objects in range\n", object_list.size());
		std::list<Decker::Objects::Object*>::iterator it;
		for (it=object_list.begin();it != object_list.end();++it) {
			if ((*it)->type() == Decker::Objects::Type::BreakingWall) {
				double dist=ppl7::grafix::Distance((*it)->p, position());
				if (dist < 100)startHacking((*it));
			}
		}
	}
}

void Player::emmitParticles(double time)
{
	ParticleSystem* ps=GetParticleSystem();
	if (particle_reason == ParticleReason::Drowned && next_particle_birth < time) {
		std::list<Particle::ScaleGradientItem>scale_gradient;
		scale_gradient.push_back(Particle::ScaleGradientItem(0.005, 0.044));
		scale_gradient.push_back(Particle::ScaleGradientItem(1.000, 1.000));
		ppl7::grafix::PointF p(x, y - 2 * TILE_HEIGHT);
		next_particle_birth=time + randf(0.020, 0.300);
		int new_particles=ppl7::rand(3, 15);
		for (int i=0;i < new_particles;i++) {
			Particle* particle=new Particle();
			particle->birth_time=time;
			particle->death_time=randf(0.481, 1.424) + time;
			particle->p=p;
			particle->layer=Particle::Layer::BehindPlayer;
			particle->weight=randf(0.000, 0.000);
			particle->gravity.setPoint(0.000, 0.000);
			particle->velocity=calculateVelocity(randf(0.755, 3.208), 0.000 + randf(-11.887, 11.887));
			particle->scale=randf(0.300, 1.000);
			particle->color_mod.set(255, 255, 255, 255);
			particle->initAnimation(Particle::Type::RotatingParticleTransparent);
			particle->initScaleGradient(scale_gradient, particle->scale);
			//particle->initColorGradient(color_gradient);
			ps->addParticle(particle);
		}
	} else if (particle_reason == ParticleReason::Burning && next_particle_birth < time) {
		ppl7::grafix::PointF p(x, y);
		next_particle_birth=time + randf(0.020, 0.300);
		int h=color_modulation.red();
		h-=50;
		if (h < 0) h=0;
		color_modulation.setColor(h, h, h, 255);

		std::list<Particle::ScaleGradientItem>scale_gradient;
		std::list<Particle::ColorGradientItem>color_gradient;
		scale_gradient.push_back(Particle::ScaleGradientItem(0.000, 0.063));
		scale_gradient.push_back(Particle::ScaleGradientItem(0.068, 0.235));
		scale_gradient.push_back(Particle::ScaleGradientItem(0.320, 0.353));
		scale_gradient.push_back(Particle::ScaleGradientItem(0.604, 0.241));
		scale_gradient.push_back(Particle::ScaleGradientItem(1.000, 1.000));
		color_gradient.push_back(Particle::ColorGradientItem(0.000, ppl7::grafix::Color(255, 255, 0, 220)));
		color_gradient.push_back(Particle::ColorGradientItem(0.149, ppl7::grafix::Color(255, 99, 0, 101)));
		color_gradient.push_back(Particle::ColorGradientItem(0.311, ppl7::grafix::Color(40, 41, 43, 101)));
		color_gradient.push_back(Particle::ColorGradientItem(0.428, ppl7::grafix::Color(11, 14, 12, 69)));
		color_gradient.push_back(Particle::ColorGradientItem(0.590, ppl7::grafix::Color(126, 127, 130, 0)));
		color_gradient.push_back(Particle::ColorGradientItem(0.775, ppl7::grafix::Color(255, 255, 255, 16)));
		color_gradient.push_back(Particle::ColorGradientItem(1.000, ppl7::grafix::Color(114, 116, 116, 0)));

		int new_particles=ppl7::rand(83, 109);
		for (int i=0;i < new_particles;i++) {
			Particle* particle=new Particle();
			particle->birth_time=time;
			particle->death_time=randf(1.017, 2.649) + time;
			particle->p=getBirthPosition(p, EmitterType::Rectangle, ppl7::grafix::Size(39, 1), 0.000);
			particle->layer=Particle::Layer::BeforePlayer;
			particle->weight=randf(0.000, 0.000);
			particle->gravity.setPoint(0.000, 0.000);
			particle->velocity=calculateVelocity(randf(2.462, 5.102), 0.000 + randf(-11.887, 11.887));
			particle->scale=randf(0.512, 3.211);
			particle->color_mod.set(226, 126, 69, 41);
			particle->initAnimation(Particle::Type::RotatingParticleWhite);
			particle->initScaleGradient(scale_gradient, particle->scale);
			particle->initColorGradient(color_gradient);
			ps->addParticle(particle);
		}
	} else if (particle_reason == ParticleReason::Smashed && next_particle_birth < time) {
		ppl7::grafix::PointF p(x, y);
		std::list<Particle::ScaleGradientItem>scale_gradient;
		scale_gradient.push_back(Particle::ScaleGradientItem(0.009, 1.000));
		scale_gradient.push_back(Particle::ScaleGradientItem(0.450, 1.000));
		scale_gradient.push_back(Particle::ScaleGradientItem(1.000, 0.010));
		next_particle_birth=time + randf(1.440, 2.118);
		ParticleSystem* ps=GetParticleSystem();
		particle_end_time=0.0f;
		int new_particles=ppl7::rand(84, 150);
		for (int i=0;i < new_particles;i++) {
			Particle* particle=new Particle();
			particle->birth_time=time;
			particle->death_time=randf(0.198, 0.764) + time;
			particle->p=getBirthPosition(p, EmitterType::Rectangle, ppl7::grafix::Size(87, 1), 0.000);
			particle->layer=Particle::Layer::BeforePlayer;
			particle->weight=randf(0.377, 0.726);
			particle->gravity.setPoint(0.000, 0.396);
			particle->velocity=calculateVelocity(randf(3.396, 7.736), 0.000 + randf(-32.264, 32.264));
			particle->scale=randf(0.217, 1.000);
			particle->color_mod.set(116, 0, 0, 255);
			particle->initAnimation(Particle::Type::RotatingParticleWhite);
			particle->initScaleGradient(scale_gradient, particle->scale);
			ps->addParticle(particle);
		}
	}
}




void Player::enableTalkie(bool flag)
{
	talkie=flag;
}

void Player::speak(VoiceGeorge::Id id, float volume, const ppl7::String& text, const ppl7::String& phonetics)
{
	AudioPool& ap=getAudioPool();

	if (voice && voice->finished() == false) return;
	if (voice) {
		if (voice->finished() == false) return;
		ap.stopInstace(voice);
		delete voice;
		voice=NULL;
	}

	if (id < VoiceGeorge::maxClips) {
		voice=new AudioInstance(ap.voice_george[id], AudioClass::Speech);
		voice->setVolume(volume);
		voice->setAutoDelete(false);
		ap.playInstance(voice);
		this->phonetics=phonetics;

	}
}

bool Player::speak(uint16_t id, float volume)
{
	if (id < 1) return false;
	AudioPool& ap=getAudioPool();
	if (voice && voice->finished() == false) return false;
	if (voice) {
		if (voice->finished() == false) return false;
		ap.stopInstace(voice);
		delete voice;
		voice=NULL;
	}
	spokenText.insert(id);
	if (!talkie) return true;
	Translator::Speech speech=translate(id);
	const ppl7::String& lang=game->config.SpeechLanguage;
	ppl7::String filepath;
	if (speech.audiofile.isEmpty()) speech.audiofile.setf("%04d", id);
	if (speech.audiofile.notEmpty()) {
		speech.audiofile=ppl7::File::getFilename(speech.audiofile);
		if (speech.audiofile.right(4) == ".mp3") speech.audiofile=speech.audiofile.left(speech.audiofile.size() - 4);
		filepath="res/audio/george/" + lang + "/" + speech.audiofile + ".mp3";
		if (!ppl7::File::exists(filepath)) {
			filepath="res/audio/george/en/" + speech.audiofile + ".mp3";
			if (!ppl7::File::exists(filepath)) {
				filepath="res/audio/george/de/" + speech.audiofile + ".mp3";
				if (!ppl7::File::exists(filepath)) {
					filepath.clear();
				}
			}
		}
	}
	//ppl7::PrintDebugTime("speak called with id %d, text=>>%s<<, file=%s, real: %s\n", id, (const char*)speech.text, (const char*)speech.audiofile, (const char*)filepath);

	if (speech.text.notEmpty()) {
		game->message_overlay.setText(MessageOverlay::Character::George, speech.text, speech.phonetics);
	}
	this->phonetics=speech.phonetics;
	if (speech.phonetics.isEmpty()) this->phonetics=speech.text;

	//ppl7::PrintDebugTime("George: %s\n", (const char*)text);
	//ppl7::PrintDebugTime("File: %s\n", (const char*)filepath);
	if (filepath.notEmpty()) {
		try {
			voice_sample.load(filepath);
			voice=new AudioInstance(voice_sample, AudioClass::Speech);
			voice->setVolume(volume * 0.7f);
			voice->setAutoDelete(false);
			ap.playInstance(voice);
		} catch (const ppl7::Exception& exp) {
			exp.print();
			ppl7::PrintDebugTime("\n");
		}
	}
	return true;
}

bool Player::hasSpoken(uint16_t id) const
{
	std::set<uint16_t>::const_iterator it;
	it=spokenText.find(id);
	if (it != spokenText.end()) return true;
	return false;
}

bool Player::isSpeaking() const
{
	if (game->message_overlay.hasMessage()) return true;
	if (voice && voice->finished() == false) return true;
	return false;
	/*
	ppl7::PrintDebugTime("phonetics: %zd, voice=%zd\n", phonetics.size(), voice);
	if (phonetics.size() > 0) return true;
	if (voice && voice->finished() == false) return true;
	return false;
	*/
}


void Player::idleJokes(double time)
{
	//AudioPool& ap=getAudioPool();
	if (time > startIdle + 3600.0f) {
		if (animation.getFrame() != 304) animation.setStaticFrame(304);
	} else if (time > startIdle + 1200.0f) {
		if (animation.getFrame() != 303) animation.setStaticFrame(303);
	} else if (time > startIdle + 600.0f) {
		speak(VoiceGeorge::snort, 0.3f);
		if (animation.getFrame() != 302) animation.setStaticFrame(302);
	} else if (time > startIdle) {
		if (nextIdleSpeech == 0.0f) {
			nextIdleSpeech=time + ppl7::randf(30.0f, 120.0f);
			if (!greetingPlayed) {
				speak(VoiceGeorge::hello_im_george, 0.6f, translate("Hello, I'm George!"), translate("Heeelllooooo     aaiiemmdSoooorrSSSSS  "));
				greetingPlayed=true;
				return;
			}
		}

		if (nextIdleSpeech != 0.0f && time > nextIdleSpeech) {
			nextIdleSpeech=time + ppl7::randf(30.0f, 120.0f);
			int r=ppl7::rand(1, 4);
			switch (r) {
				case 1: speak(VoiceGeorge::hello, 0.6f, translate("Hello!"), translate("elooo")); break;
				case 2: speak(VoiceGeorge::hello_questioned, 0.6f, translate("Hello?"), translate("eloooo")); break;
				case 3: speak(VoiceGeorge::hello_here_i_am, 0.6f, translate("Hello, here I am!"), translate("elloooooeerraiieemmm")); break;
				default: speak(VoiceGeorge::play_with_me, 0.6f, translate("Play with me!"), translate("pleiiwissme")); break;
			}
		}
	} else {
		nextIdleSpeech=0.0f;;
	}
}


void Player::playPhonetics()
{
	if (phonetics.isEmpty()) return;
	if (time < nextPhonetic) return;
	ppl7::String p=phonetics.left(1);
	phonetics.chopLeft();
	if (phonetics.isEmpty() && p != "-") phonetics="-";
	if (movement != Stand || orientation != Front) return;
	int s=284;
	if (p == "a" || p == "i") s=283 + 0;
	if (p == "b" || p == "m") s=283 + 6;
	if (p == "c" || p == "d" || p == "g" || p == "j" || p == "k" || p == "n" || p == "r" || p == "s" || p == "y" || p == "z") s=283 + 3;
	if (p == "e") s=283 + 2;
	if (p == "f" || p == "v") s=283 + 4;
	if (p == "l") s=283 + 5;
	if (p == "m" || p == "p") s=283 + 6;
	if (p == "o") s=283 + 7;
	if (p == "q" || p == "w") s=283 + 10;
	if (p == "u") s=283 + 9;
	if (p == "S") s=283 + 18;
	nextPhonetic=time + 0.08f;
	animation.setStaticFrame(s);

}

void Player::toggleFlashlight()
{
	if (this->time > actionToggleCooldown) {
		if (hasSpecialObject(Decker::Objects::Type::Flashlight)) {
			flashlightOn=!flashlightOn;
			actionToggleCooldown=time + 0.2f;
			ppl7::grafix::Point p(x, y);
			getAudioPool().playOnce(AudioClip::light_switch1, p, 1600, 1.0f);
		} else if (flashlightOn == true) {
			flashlightOn=false;
			actionToggleCooldown=time + 0.2f;
		}
	} else {
		actionToggleCooldown=this->time + 0.2f;
	}
}

void Player::enableFlashlight(bool enable)
{
	if (hasSpecialObject(Decker::Objects::Type::Flashlight)) {
		flashlightOn=enable;
		actionToggleCooldown=time + 0.2f;
		ppl7::grafix::Point p(x, y);
		getAudioPool().playOnce(AudioClip::light_switch1, p, 1600, 1.0f);
	}
}

bool Player::isFlashlightOn() const {
	return flashlightOn;
}


void Player::hitBySpiderWeb()
{
	if (petrified) return;
	petrified=true;
	petrifiedTimeout=time + 5.0f;
	movement = PlayerMovement::Petrified;
	getAudioPool().playOnce(AudioClip::spider_net_unfold, 1.0f);

	animation.setSpeed(0.03f);
	if (orientation == PlayerOrientation::Front || orientation == PlayerOrientation::Back) {
		animation.startSequence(415, 420, false, 420);
	} else if (orientation == PlayerOrientation::Left) {
		animation.startSequence(421, 426, false, 426);
	} else if (orientation == PlayerOrientation::Right) {
		animation.startSequence(427, 432, false, 432);
	}

	if (!hasSpoken(23)) speak(23);

}

bool Player::isPetrified() const
{
	return petrified;
}
