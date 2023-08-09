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





Player::Player(Game* game)
{
	x=y=0;
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
	air=30.0f;
	maxair=30.0f;
	particle_end_time=0.0f;
	next_particle_birth=0.0f;
	particle_reason=ParticleReason::None;
	color_modulation.setColor(255, 255, 255, 255);
	ambient_sound=NULL;
	expressionJump=false;
	hackingObject=NULL;
	hacking_end=0.0f;
	animation_speed=0.056f;
	hackingState=0;
	airStart=0.0f;
	voice=NULL;
	voiceDamageCooldown=0.0f;
	startIdle=ppl7::GetMicrotime() + 5.0f;
	time=startIdle;
	nextIdleSpeech=0.0f;
	greetingPlayed=false;
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

void Player::resetState()
{
	animation_speed=0.056f;
	points=0;
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
	expressionJump=false;
	hackingObject=NULL;
	hacking_end=0.0f;
	hackingState=0;
	color_modulation.setColor(255, 255, 255, 255);
	airStart=0.0f;
	nextIdleSpeech=0.0f;
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
	animation_speed=0.056f;
	Inventory.clear();
	object_counter.clear();
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
	sprite_resource->draw(renderer, p.x, p.y + 1, animation.getFrame(), color_modulation);
	/*
	SDL_SetRenderDrawColor(renderer,255,0,0,255);
	sprite_resource->drawBoundingBox(renderer,p.x,p.y+1,animation.getFrame());
	*/
}

void Player::drawCollision(SDL_Renderer* renderer, const ppl7::grafix::Rect& viewport, const ppl7::grafix::Point& worldcoords) const
{
	ppl7::grafix::Point p(x + viewport.x1 - worldcoords.x, y + viewport.y1 - worldcoords.y);
	if (tiletype_resource) {
		for (int cy=0;cy < 6;cy++) {
			for (int cx=0;cx < 4;cx++) {
				tiletype_resource->draw(renderer, p.x - (TILE_WIDTH * 2) + (cx * TILE_WIDTH), p.y - (5 * TILE_HEIGHT) + (cy * TILE_HEIGHT), collision_matrix[cx][cy]);
				//ppl7::PrintDebugTime("cx:cy %d:%d = %d\n", cx, cy, collision_matrix[cx][cy]);
			}
		}
	}
	tiletype_resource->draw(renderer, p.x + 80, p.y - 32, collision_at_pivoty[0]);
	tiletype_resource->draw(renderer, p.x + 80, p.y, collision_at_pivoty[1]);
	tiletype_resource->draw(renderer, p.x + 80, p.y + 32, collision_at_pivoty[2]);
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

int Player::getKeyboardMatrix(const Uint8* state)
{
	if (state == NULL) state=SDL_GetKeyboardState(NULL);
	int matrix=0;
	if (state[SDL_SCANCODE_LEFT]) matrix|=KeyboardKeys::Left;
	if (state[SDL_SCANCODE_J] || state[SDL_SCANCODE_A]) matrix|=KeyboardKeys::Left;
	if (state[SDL_SCANCODE_RIGHT]) matrix|=KeyboardKeys::Right;
	if (state[SDL_SCANCODE_L] || state[SDL_SCANCODE_D]) matrix|=KeyboardKeys::Right;
	if (state[SDL_SCANCODE_UP]) matrix|=KeyboardKeys::Up;
	if (state[SDL_SCANCODE_I] || state[SDL_SCANCODE_W]) matrix|=KeyboardKeys::Up;
	if (state[SDL_SCANCODE_DOWN]) matrix|=KeyboardKeys::Down;
	if (state[SDL_SCANCODE_K] || state[SDL_SCANCODE_S]) matrix|=KeyboardKeys::Down;
	if (state[SDL_SCANCODE_LSHIFT] || state[SDL_SCANCODE_RSHIFT]) matrix|=KeyboardKeys::Shift;
	if (state[SDL_SCANCODE_E] || state[SDL_SCANCODE_O]) matrix|=KeyboardKeys::Action;
	return matrix;
}

void Player::stand()
{
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
	if (orientation == Front && movement == Stand) {
		if (animation.getFrame() != 297) animation.setStaticFrame(297);
	}
	if (godmode) return;



	health-=points;
	if (health <= 0.0f && movement != Dead) {
		health=0;
		movement=Dead;
		fallstart=0.0f;
		// we can play different animations for different reasons
		if (reason == FallingDeep) {
			animation.start(death_by_falling, sizeof(death_by_falling) / sizeof(int), false, 106);
		} else if (reason == Drowned) {
			animation.startSequence(260, 281, false, 281);
		} else if (reason == Burned) {
			animation.startSequence(208, 216, false, 216);
			startEmittingParticles(time + 1.0f, ParticleReason::Burning);

		} else {
			animation.start(death_animation, sizeof(death_animation) / sizeof(int), false, 106);
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

void Player::playSoundOnAnimationSprite()
{
	int sprite=animation.getFrame();
	AudioPool& ap=getAudioPool();

	if (sprite == 245 || sprite == 224)  ap.playOnce(AudioClip::hackstone, 1.0f);
	if (sprite == 3 || sprite == 7 || sprite == 12 || sprite == 16 || sprite == 64 || sprite == 68
		|| sprite == 73 || sprite == 77) play_step(ap);


}

void Player::update(double time, const TileTypePlane& world, Decker::Objects::ObjectSystem* objects, float frame_rate_compensation)
{
	if (particle_reason != ParticleReason::None && particle_end_time > time) emmitParticles(time);
	this->time=time;
	if (time > next_animation) {
		next_animation=time + animation_speed;
		animation.update();
		playSoundOnAnimationSprite();
		if (phonetics.notEmpty()) playPhonetics();

	}
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
	if (collision_matrix[1][4] == TileType::Water || collision_matrix[2][4] == TileType::Water) {
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

	}


	//ppl7::PrintDebugTime("gravity: %0.3f, velocity_move x: %0.3f, y: %0.3f, acceleration_jump: %0.3f\n",
	//	gravity, velocity_move.x, velocity_move.y, acceleration_jump);

	x+=velocity_move.x;
	y+=velocity_move.y + gravity;


	if (movement == Turn) {
		if (!animation.isFinished()) return;
		//printf ("debug 2\n");
		movement=Stand;
		orientation=turnTarget;
		startIdle=time;
		velocity_move.stop();
		//printf("Turn done, movement=%d, orientation=%d\n", (int)movement, (int)orientation);
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

	if (movement == Swim || movement == SwimStraight || movement == SwimUp || movement == SwimDown) {
		//handleKeyboardWhileSwimming(time, world, objects, frame_rate_compensation);
		return;
	}



	acceleration_jump_sideways=0;
	//if (time>next_keycheck) {
	//next_keycheck=time+0.1f;
	const Uint8* state = SDL_GetKeyboardState(NULL);
	int keys=getKeyboardMatrix(state);
	if (keys & KeyboardKeys::Action) {
		checkActivationOfObjectsInRange(objects);
		if (movement == Hacking) return;
	}

	if (keys == KeyboardKeys::Left) {
		if (orientation != Left) { turn(Left); return; }
		if (movement != Walk) {
			movement=Walk;
			animation.start(walk_cycle_left, sizeof(walk_cycle_left) / sizeof(int), true, 0);
		}
	} else if (keys == (KeyboardKeys::Left | KeyboardKeys::Shift)) {
		if (movement != Run || orientation != Left) {
			movement=Run;
			orientation=Left;
			animation.start(run_cycle_left, sizeof(run_cycle_left) / sizeof(int), true, 0);
		}
	} else if (keys == KeyboardKeys::Right) {
		if (orientation != Right) { turn(Right); return; }
		if (movement != Walk) {
			movement=Walk;
			animation.start(walk_cycle_right, sizeof(walk_cycle_right) / sizeof(int), true, 0);
		}
	} else if (keys == (KeyboardKeys::Right | KeyboardKeys::Shift)) {
		if (movement != Run || orientation != Right) {
			movement=Run;
			orientation=Right;
			animation.start(run_cycle_right, sizeof(run_cycle_right) / sizeof(int), true, 0);
		}
	} else if ((keys == KeyboardKeys::Up || keys == (KeyboardKeys::Up | KeyboardKeys::Shift)) && movement != Falling && movement != Jump) {
		if (collision_matrix[1][4] == TileType::Ladder || collision_matrix[2][4] == TileType::Ladder) {
			if (movement != ClimbUp) {
				movement=ClimbUp;
				orientation=Back;
				animation.start(climb_up_cycle, sizeof(climb_up_cycle) / sizeof(int), true, 0);
			}
		} else {
			if (movement != Jump) {
				movement=Jump;
				if (keys & KeyboardKeys::Shift) {
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
	} else if ((keys & KeyboardKeys::JumpLeft) == KeyboardKeys::JumpLeft) {
		movement=Jump;
		orientation=Left;
		if (keys & KeyboardKeys::Shift) {
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
	} else if ((keys & KeyboardKeys::JumpRight) == KeyboardKeys::JumpRight) {
		movement=Jump;
		orientation=Right;
		if (keys & KeyboardKeys::Shift) {
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
	} else if (keys == KeyboardKeys::Down || keys == (KeyboardKeys::Down | KeyboardKeys::Shift)) {
		if (collision_matrix[1][4] == TileType::Ladder || collision_matrix[2][4] == TileType::Ladder
			|| collision_matrix[1][5] == TileType::Ladder || collision_matrix[2][5] == TileType::Ladder) {
			if (collision_matrix[1][5] != TileType::Blocking && collision_matrix[2][5] != TileType::Blocking) {
				if (movement != ClimbDown) {
					//printf ("climb down\n");
					movement=ClimbDown;
					orientation=Back;
					animation.start(climb_down_cycle, sizeof(climb_down_cycle) / sizeof(int), true, 0);
				}
			}
		}

	} else if (keys == (KeyboardKeys::Left) && movement == Jump) {
		if (!isCollisionLeft()) velocity_move.x=-2 * frame_rate_compensation;
	} else if (keys == (KeyboardKeys::Right) && movement == Jump) {
		if (!isCollisionLeft()) velocity_move.x=2 * frame_rate_compensation;
	} else if (keys == (KeyboardKeys::Left | KeyboardKeys::Shift) && movement == Jump) {
		if (!isCollisionLeft()) velocity_move.x=-8 * frame_rate_compensation;
	} else if (keys == (KeyboardKeys::Right | KeyboardKeys::Shift) && movement == Jump) {
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
	if (movement == Stand && orientation == Front) idleJokes(time);
	else startIdle=time;
//}
}

void Player::handleKeyboardWhileJumpOrFalling(double time, const TileTypePlane& world, Decker::Objects::ObjectSystem* objects, float frame_rate_compensation)
{
	const Uint8* state = SDL_GetKeyboardState(NULL);
	int keys=getKeyboardMatrix(state);
	if (movement == Jump) {
		if (!(keys & KeyboardKeys::Up)) {
			movement=Falling;
			return;
		}
		if ((keys & KeyboardKeys::Left) && velocity_move.x == 0) {
			if (acceleration_jump_sideways > -6.0f) acceleration_jump_sideways=-6.0f;
			velocity_move.x=acceleration_jump_sideways * frame_rate_compensation;
			if (keys & KeyboardKeys::Shift && acceleration_jump_sideways > -9.0f) acceleration_jump_sideways-=(0.2f * frame_rate_compensation);
			orientation=Left;
			animation.setStaticFrame(38);
		} else if ((keys & KeyboardKeys::Right) && velocity_move.x == 0) {
			if (acceleration_jump_sideways < 6.0f) acceleration_jump_sideways=6.0f;
			velocity_move.x=acceleration_jump_sideways * frame_rate_compensation;
			if (keys & KeyboardKeys::Shift && acceleration_jump_sideways < 9.0f) acceleration_jump_sideways+=(0.2f * frame_rate_compensation);
			orientation=Right;
			animation.setStaticFrame(39);
		}
	} else {
		if (keys & KeyboardKeys::Left) {
			if (acceleration_jump_sideways > -6.0f) acceleration_jump_sideways=-6.0f;
			velocity_move.x=acceleration_jump_sideways * frame_rate_compensation;
			if (keys & KeyboardKeys::Shift && acceleration_jump_sideways > -9.0f) acceleration_jump_sideways-=(0.2f * frame_rate_compensation);
			orientation=Left;
			animation.setStaticFrame(38);
		} else if (keys & KeyboardKeys::Right) {
			if (acceleration_jump_sideways < 6.0f) acceleration_jump_sideways=6.0f;
			velocity_move.x=acceleration_jump_sideways * frame_rate_compensation;
			if (keys & KeyboardKeys::Shift && acceleration_jump_sideways < 9.0f) acceleration_jump_sideways+=(0.2f * frame_rate_compensation);
			orientation=Right;
			animation.setStaticFrame(39);
		}
	}
	//ppl7::PrintDebugTime("acceleration_jump_sideways=%0.3f\n", acceleration_jump_sideways);
}

void Player::handleKeyboardWhileSwimming(double time, const TileTypePlane& world, Decker::Objects::ObjectSystem* objects, float frame_rate_compensation)
{
	//ppl7::PrintDebugTime("Player::handleKeyboardWhileSwimming: old movement: %s, ", (const char*)getState());
	const Uint8* state = SDL_GetKeyboardState(NULL);
	int keys=getKeyboardMatrix(state);

	if (keys & KeyboardKeys::Up) {
		if (collision_matrix[1][2] != TileType::Water && collision_matrix[2][2] != TileType::Water && movement != Jump) {
			if (keys & KeyboardKeys::Left) {
				movement=Jump;
				orientation=Left;
				if (keys & KeyboardKeys::Shift) {
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
			} else if (keys & KeyboardKeys::Right) {
				movement=Jump;
				orientation=Right;
				if (keys & KeyboardKeys::Shift) {
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
				if (keys & KeyboardKeys::Shift) {
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
	if (keys & KeyboardKeys::Shift) {
		speed=4.0f * frame_rate_compensation;
		keys-=KeyboardKeys::Shift;
	}
	if (keys == KeyboardKeys::Up) {
		if (movement != Swim || orientation != Front) {
			movement=Swim;
			orientation=Front;
			animation.start(swimm_inplace_front, sizeof(swimm_inplace_front) / sizeof(int), true, 0);
		}
		velocity_move.y=-speed;
		velocity_move.x=0;
	} else if (keys == KeyboardKeys::Down) {
		if (movement != Swim || orientation != Front) {
			movement=Swim;
			orientation=Front;
			animation.start(swimm_inplace_front, sizeof(swimm_inplace_front) / sizeof(int), true, 0);
		}
		velocity_move.y=speed;
		velocity_move.x=0;
	} else if (keys == KeyboardKeys::Right && (collision_matrix[1][1] != TileType::Water && collision_matrix[2][1] != TileType::Water)) {
		if (movement != Swim || orientation != Right) {
			movement=Swim;
			orientation=Right;
			animation.start(swimm_up_right, sizeof(swimm_up_right) / sizeof(int), true, 0);
		}
		velocity_move.x=speed;
		velocity_move.y=0;
	} else if (keys == KeyboardKeys::Right) {
		if (movement != SwimStraight || orientation != Right) {
			movement=SwimStraight;
			orientation=Right;
			animation.start(swimm_straigth_right, sizeof(swimm_straigth_right) / sizeof(int), true, 0);
		}
		velocity_move.x=speed;
		velocity_move.y=0;
	} else if (keys == KeyboardKeys::Left && (collision_matrix[1][1] != TileType::Water && collision_matrix[2][1] != TileType::Water)) {
		if (movement != Swim || orientation != Left) {
			movement=Swim;
			orientation=Left;
			animation.start(swimm_up_left, sizeof(swimm_up_left) / sizeof(int), true, 0);
		}
		velocity_move.x=-speed;
		velocity_move.y=0;
	} else if (keys == KeyboardKeys::Left) {
		if (movement != SwimStraight || orientation != Left) {
			movement=SwimStraight;
			orientation=Left;
			animation.start(swimm_straight_left, sizeof(swimm_straight_left) / sizeof(int), true, 0);
		}
		velocity_move.x=-speed;
		velocity_move.y=0;
	} else if (keys == (KeyboardKeys::Left | KeyboardKeys::Up)) {
		if (movement != SwimUp || orientation != Left) {
			movement=SwimUp;
			orientation=Left;
			animation.start(swimm_up_left, sizeof(swimm_up_left) / sizeof(int), true, 0);
		}
		velocity_move.x=-speed;
		velocity_move.y=-speed;
	} else if (keys == (KeyboardKeys::Right | KeyboardKeys::Up)) {
		if (movement != SwimUp || orientation != Right) {
			movement=SwimUp;
			orientation=Right;
			animation.start(swimm_up_right, sizeof(swimm_up_right) / sizeof(int), true, 0);
		}
		velocity_move.x=speed;
		velocity_move.y=-speed;
	} else if (keys == (KeyboardKeys::Right | KeyboardKeys::Down)) {
		if (movement != SwimDown || orientation != Right) {
			movement=SwimDown;
			orientation=Right;
			animation.start(swimm_down_right, sizeof(swimm_down_right) / sizeof(int), true, 0);
		}
		velocity_move.x=speed;
		velocity_move.y=speed;
	} else if (keys == (KeyboardKeys::Left | KeyboardKeys::Down)) {
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

void Player::checkCollisionWithWorld(const TileTypePlane& world)
{
	Physic::PlayerMovement new_movement=Physic::checkCollisionWithWorld(world, x, y);
	if (new_movement == Stand) stand();
	if (collision_type_count[TileType::Type::Speer] > 0) {
		this->dropHealth(10);
	}
	if (collision_type_count[TileType::Type::Fire] > 0) {
		this->dropHealth(10, Burned);
	}

}


void Player::checkCollisionWithObjects(Decker::Objects::ObjectSystem* objects, float frame_rate_compensation)
{
	// we try to find existing pixels inside the player boundary
	// to build a list with points we want to check against the
	// objects
	if (movement == Dead) return;
	std::list<ppl7::grafix::Point> checkpoints;
	checkpoints.push_back(ppl7::grafix::Point(x, y));

	const ppl7::grafix::Drawable& draw=sprite_resource->getDrawable(animation.getFrame());
	ppl7::grafix::Rect boundary=sprite_resource->spriteBoundary(animation.getFrame(), 1.0f, x, y);

	if (draw.width()) {
		//ppl7::PrintDebugTime("boundary= %d:%d - %d:%d\n", boundary.x1, boundary.y1, boundary.x2, boundary.y2);
		int stepx=boundary.width() / 8;
		int stepy=boundary.height() / 12;
		for (int py=boundary.y1;py < boundary.y2;py+=stepx) {
			for (int px=boundary.x1;px < boundary.x2;px+=stepy) {
				ppl7::grafix::Color c=draw.getPixel(px - boundary.x1, py - boundary.y1);
				if (c.alpha() > 92) {
					checkpoints.push_back(ppl7::grafix::Point(px, py));
				}
			}
		}
	}
	//printf ("check collision against %zd points:\n", checkpoints.size());

	Decker::Objects::Object* object=objects->detectCollision(checkpoints);
	if (!object) return;
	Decker::Objects::Collision col;
	col.frame_rate_compensation=frame_rate_compensation;
	col.detect(object, checkpoints, *this);
	col.bounding_box_player=getBoundingBox();
	col.bounding_box_object=object->boundary;
	col.bounding_box_intersection=col.bounding_box_player.intersected(col.bounding_box_object);

	object->handleCollision(this, col);

	//printf ("Detected Collision with Object: %s, ID: %d\n",
	//		(const char*)object->typeName(), object->id);
	//const ppl7::grafix::Rect &bbi=col.bounding_box_intersection;
	//printf ("BoundingBox Player: %d:%d - %d:%d\n", bbp.x1, bbp.y1, bbp.x2, bbp.y2);
	//printf ("BoundingBox Object: %d:%d - %d:%d\n", bbo.x1, bbo.y1, bbo.x2, bbo.y2);
	//printf ("Intersection:       %d:%d - %d:%d\n", bbi.x1, bbi.y1, bbi.x2, bbi.y2);


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
	}
}

bool Player::hackingInProgress()
{
	if (hackingObject != NULL) {
		movement=Hacking;
		animation_speed=0.03f;
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
				animation_speed=0.056f;
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
	}
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
	} else if (time > startIdle + 5) {
		if (nextIdleSpeech == 0.0f || time > nextIdleSpeech) {
			nextIdleSpeech=time + ppl7::randf(30.0f, 120.0f);
			//nextIdleSpeech=time + 5;
			if (!greetingPlayed) {
				speak(VoiceGeorge::hello_im_george, 0.6f, translate("Hello, I'm George!"), translate("HelloooaaiiemmgoorrSS"));
				greetingPlayed=true;
			}
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

/*
A   = 0
B   = 6
C   = 3
D   = 3
E   = 2
F   = 4
G   = 3
H   = -
I   = 0
J   = 3
K   = 3
L   = 5
M   = 6
N   = 3
O   = 7
P   = 6
Q   = 10
R   = 3
S   = 3
T
U   = 9
V   = 4
W   = 10
X
Y   = 3
Z   = 3
SCH = 18

*/
void Player::playPhonetics()
{
	if (phonetics.isEmpty()) return;
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

	animation.setStaticFrame(s);

}
