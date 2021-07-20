#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"


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
	collsionDetection=false;
	sprite_no_representation=22;
}

void ThreeSpeers::update(double time)
{
	if (time>next_animation) {
		next_animation=time+0.056f;
		animation.update();
		int new_sprite=animation.getFrame();
		if (new_sprite!=sprite_no) {
			sprite_no=new_sprite;
			if (animation.isFinished()==true && collsionDetection==true && state==0)
				collsionDetection=false;
			updateBoundary();
		}
	}
	if (state==0 && next_state<time) {
		next_state=time+5.00;
		animation.start(trap_activation,sizeof(trap_activation)/sizeof(int),false,22);
		sprite_no=animation.getFrame();
		state=1;
		collsionDetection=true;
		updateBoundary();

	}
	if (state==1 && next_state<time) {
		next_state=time+10.00;
		animation.start(trap_deactivation,sizeof(trap_deactivation)/sizeof(int),false,18);
		sprite_no=animation.getFrame();
		state=0;
		collsionDetection=false;
		updateBoundary();

	}

}


}	// EOF namespace Decker::Objects
