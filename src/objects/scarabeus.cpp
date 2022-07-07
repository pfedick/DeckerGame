#include <ppl7.h>
#include <ppl7-grafix.h>
#include <math.h>
#include "objects.h"
#include "player.h"
#include "decker.h"
#include "audiopool.h"

namespace Decker::Objects {

static int fly_left[]={4,8};
static int fly_right[]={6,10};
static int fly_up[]={7,11};
static int fly_down[]={5,9};

static int scarab_sound[]={AudioClip::scarabeus1,
		AudioClip::scarabeus2,
		AudioClip::scarabeus3,
		AudioClip::scarabeus4,
		AudioClip::scarabeus5,
		AudioClip::scarabeus6,
		AudioClip::scarabeus7,
		AudioClip::scarabeus8,
		AudioClip::scarabeus9
};

static int scarab_death[]={AudioClip::scarabeus_death1,
		AudioClip::scarabeus_death2,
		AudioClip::scarabeus_death3,
		AudioClip::scarabeus_death4,
		AudioClip::scarabeus_death5
};


Representation Scarabeus::representation()
{
	return Representation(Spriteset::Scarabeus, 0);
}

static float scarab_rand_velocity()
{
	float d=ppl7::randf(-2.0f, +2.0f);
	float v=ppl7::randf(0.5f,3.0f);
	if (d<0) return v*-1.0f;
	return v;
}

Scarabeus::Scarabeus()
:Enemy(Type::ObjectType::Scarabeus)
{
	audio=NULL;
	sprite_set=Spriteset::Scarabeus;
	sprite_no=0;
	animation.setStaticFrame(0);
	sprite_no_representation=0;
	next_state=ppl7::randf(2.0f,15.0f)+ppl7::GetMicrotime();
	state=0;
	next_animation=0.0f;
	collisionDetection=true;
	velocity.setPoint(scarab_rand_velocity(), scarab_rand_velocity());
	update_animation();
}

Scarabeus::~Scarabeus()
{
	if (audio) {
		getAudioPool().stopInstace(audio);
		delete audio;
		audio=NULL;
	}
}


void Scarabeus::update_animation()
{
	float vh=fabsf(velocity.x);
	float vv=fabsf(velocity.y);
	if (vh>vv) {
		if (velocity.x<0.0f) animation.start(fly_left,sizeof(fly_left)/sizeof(int),true,0);
		else animation.start(fly_right,sizeof(fly_right)/sizeof(int),true,0);
	} else {
		if (velocity.y<0.0f) animation.start(fly_up,sizeof(fly_up)/sizeof(int),true,0);
		else animation.start(fly_down,sizeof(fly_down)/sizeof(int),true,0);
	}
}

void Scarabeus::update(double time, TileTypePlane &ttplane, Player &player)
{
	if (state==3) return;
	AudioPool &pool=getAudioPool();
	if (time>next_animation) {
		next_animation=time+0.07f;
		animation.update();
		sprite_no=animation.getFrame();
		updateBoundary();
	}
	if (state==0) {
		if (!audio) {
			audio=pool.getInstance((AudioClip::Id)scarab_sound[ppl7::rand(0,sizeof(scarab_sound)/sizeof(int)-1)]);
			audio->setVolume(1.0f);
			audio->setPositional(p, 1800);
			audio->setLoop(true);
			pool.playInstance(audio);
		}

		if (time>next_state) {
			if (velocity.x!=0) velocity.x=velocity.x*2.8f/3.0f;
			if (velocity.y!=0) velocity.y=velocity.y*2.8f/3.0f;
			if (fabsf(velocity.x)<0.1) velocity.x=0.0f;
			if (fabsf(velocity.y)<0.1) velocity.y=0.0f;
			if (velocity.x==0.0f && velocity.y==0.0f) {
				next_state=time+ppl7::randf(0.5f,2.0f);
				state=2;
			}
		}
		p+=velocity;
		audio->setPositional(p, 1800);
		TileType::Type t_left=ttplane.getType(ppl7::grafix::Point(p.x-32, p.y));
		TileType::Type t_right=ttplane.getType(ppl7::grafix::Point(p.x+32, p.y));
		TileType::Type t_top=ttplane.getType(ppl7::grafix::Point(p.x, p.y-45));
		TileType::Type t_bottom=ttplane.getType(ppl7::grafix::Point(p.x, p.y+8));
		if (t_bottom!=TileType::NonBlocking && velocity.y>0.0f) {
			float vh=fabsf(velocity.x);
			float vv=fabsf(velocity.y);
			if (vh>vv) {
				if (velocity.x<0.0f) animation.setStaticFrame(0);
				else animation.setStaticFrame(2);
			} else {
				if (velocity.y<0.0f) animation.setStaticFrame(3);
				else animation.setStaticFrame(1);
			}
			velocity.setPoint(0,0);
			state=1;
			next_state=time+ppl7::randf(0.2f,7.0f);
			p.y=(int)(p.y/TILE_HEIGHT)*TILE_HEIGHT+TILE_HEIGHT-5;
			pool.stopInstace(audio);
			delete audio;
			audio=NULL;
		}
		if (t_left!=TileType::NonBlocking || t_right!=TileType::NonBlocking) {
			velocity.x*=-1;
			p.x+=velocity.x;
			update_animation();
		}
		if (t_top!=TileType::NonBlocking && velocity.y<0.0f) {
			velocity.y*=-1;
			p.y+=velocity.y;
			update_animation();
		}
	} else if (state==1 && next_state<time) {
		state=0;
		next_state=time+ppl7::randf(2.0f,15.0f);
		velocity.setPoint(scarab_rand_velocity(), ppl7::randf(-3.0f, -0.5f));
		update_animation();
	} else if (state==2 && next_state<time) {
		velocity.setPoint(scarab_rand_velocity(), scarab_rand_velocity());
		update_animation();
		state=0;
		next_state=time+ppl7::randf(2.0f,15.0f);
	}
}

void Scarabeus::handleCollision(Player *player, const Collision &collision)
{
	//Player::PlayerMovement movement=player->getMovement();
	//printf ("collision: %d\n", (int)collision.onFoot());
	if (collision.bounding_box_object.y1>collision.bounding_box_player.y2-TILE_HEIGHT) {
		//animation.start(death_animation,sizeof(death_animation)/sizeof(int),false,100);
		//state=6;
		collisionDetection=false;
		enabled=false;
		player->addPoints(50);
		AudioPool &pool=getAudioPool();
		if (audio) {
			pool.stopInstace(audio);
			delete audio;
			audio=NULL;
		}
		state=3;

		pool.playOnce((AudioClip::Id)scarab_death[ppl7::rand(0,sizeof(scarab_death)/sizeof(int)-1)]);

	} else {
		player->dropHealth(1);
	}
}



}	// EOF namespace Decker::Objects
