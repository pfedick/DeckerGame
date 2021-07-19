#include <stdio.h>
#include "animation.h"

AnimationCycle::AnimationCycle()
{
	index=0;
	cycle=NULL;
	size=1;
	loop=false;
	endframe=0;
	finished=true;
}

void AnimationCycle::start(int *cycle_array, int size, bool loop, int endframe)
{
	//printf("start\n");
	cycle=cycle_array;
	this->size=size;
	this->loop=loop;
	this->endframe=endframe;
	finished=false;
	index=0;
}

void AnimationCycle::setStaticFrame(int nr)
{
	cycle=NULL;
	index=0;
	loop=false;
	finished=true;
	endframe=nr;
}

void AnimationCycle::update()
{
	if (finished) return;
	index++;
	if (index>=size) {
		if (loop==true) {
			index=0;
		} else {
			finished=true;
		}
	}
}

int AnimationCycle::getFrame() const
{
	if(finished) return endframe;
	return cycle[index];
}

int AnimationCycle::getIndex() const
{
	return index;
}

bool AnimationCycle::isFinished() const
{
	return finished;
}
