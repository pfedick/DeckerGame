#include <stdio.h>
#include <ppl7.h>
#include "animation.h"

AnimationCycle::AnimationCycle()
{
	index=0;
	cycle=NULL;
	size=1;
	loop=false;
	endframe=0;
	finished=true;
	seq_start=0;
	seq_end=0;
}

void AnimationCycle::start(int* cycle_array, int size, bool loop, int endframe)
{
	//printf("start\n");
	cycle=cycle_array;
	this->size=size;
	this->loop=loop;
	this->endframe=endframe;
	finished=false;
	index=0;
}

void AnimationCycle::start(const AnimationCycle& other)
{
	cycle=other.cycle;
	index=other.index;
	size=other.size;
	endframe=other.endframe;
	seq_start=other.seq_start;
	seq_end=other.seq_end;
	loop=other.loop;
	finished=other.finished;
}

void AnimationCycle::startRandom(int* cycle_array, int size, bool loop, int endframe)
{
	start(cycle_array, size, loop, endframe);
	index=ppl7::rand(0, size);
	if (index >= size) index=0;
}

void AnimationCycle::setStaticFrame(int nr)
{
	cycle=NULL;
	seq_start=0;
	seq_end=0;
	index=0;
	loop=false;
	finished=true;
	endframe=nr;
}

void AnimationCycle::startSequence(int start, int end, bool loop, int endframe)
{
	cycle=NULL;
	seq_start=start;
	seq_end=end;
	index=0;
	if (seq_end > seq_start)
		size=seq_end - seq_start + 1;
	else
		size=-(seq_start - seq_end + 1);
	this->loop=loop;
	finished=false;
	this->endframe=endframe;
}

void AnimationCycle::startRandomSequence(int start, int end, bool loop, int endframe)
{
	cycle=NULL;
	seq_start=start;
	seq_end=end;
	if (seq_end > seq_start) {
		size=seq_end - seq_start + 1;
		index=ppl7::rand(0, size);
	} else {
		size=-(seq_start - seq_end + 1);
		index=-ppl7::rand(0, -size);
	}

	this->loop=loop;
	finished=false;
	this->endframe=endframe;
}

void AnimationCycle::update()
{
	if (finished) return;
	if (size >= 0) {
		index++;
		if (index >= size) {
			if (loop == true) {
				index=0;
			} else {
				finished=true;
			}
		}
	} else {
		index--;
		if (index <= size) {
			if (loop == true) {
				index=0;
			} else {
				finished=true;
			}
		}
	}
}

int AnimationCycle::getFrame() const
{
	if (finished) return endframe;
	if (cycle) return cycle[index];
	return seq_start + index;
}

int AnimationCycle::getIndex() const
{
	return index;
}

bool AnimationCycle::isFinished() const
{
	return finished;
}
