#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "player.h"
#include "audiopool.h"


namespace Decker::Objects {

static int trap_activation[]={18,19,20,21,22};
static int trap_deactivation[]={23,24,25,26,27,28,29,30,31,32,33,34};


Representation ThreeSpeers::representation()
{
	return Representation(Spriteset::ThreeSpeers, 22);
}

ThreeSpeers::ThreeSpeers()
: Trap(Type::ObjectType::ThreeSpeers)
{
	sprite_set=Spriteset::ThreeSpeers;
	animation.setStaticFrame(18);
	sprite_no=18;
	next_animation=next_state=0.0f;
	state=0;
	next_state=ppl7::GetMicrotime()+5.0;
	collisionDetection=false;
	sprite_no_representation=22;
}

void ThreeSpeers::update(double time, TileTypePlane &, Player &)
{
	if (time>next_animation) {
		next_animation=time+0.056f;
		animation.update();
		int new_sprite=animation.getFrame();
		if (new_sprite!=sprite_no) {
			sprite_no=new_sprite;
			if (animation.isFinished()==true && collisionDetection==true && state==0)
				collisionDetection=false;
			updateBoundary();
		}
	}
	if (state==0 && next_state<time) {
		getAudioPool().playOnce(AudioClip::threespeers_activation,p,1600,1.0f);
		next_state=time+ppl7::rand(2,5);
		animation.start(trap_activation,sizeof(trap_activation)/sizeof(int),false,22);
		sprite_no=animation.getFrame();
		state=1;
		collisionDetection=true;
		updateBoundary();
	}
	if (state==1 && next_state<time) {
		getAudioPool().playOnce(AudioClip::stone_drag_short,p,1600,1.0f);
		next_state=time+ppl7::rand(3,6);
		animation.start(trap_deactivation,sizeof(trap_deactivation)/sizeof(int),false,18);
		sprite_no=animation.getFrame();
		state=0;
		collisionDetection=false;
		updateBoundary();

	}

}

void ThreeSpeers::handleCollision(Player *player, const Collision &)
{
	player->dropHealth(2);
}

}	// EOF namespace Decker::Objects
