#ifndef INCLUDE_ANIMATION_H_
#define INCLUDE_ANIMATION_H_

class AnimationCycle
{
private:
	int *cycle;
	int index;
	int size;
	int endframe;
	bool loop;
	bool finished;

public:
	AnimationCycle();
	void setStaticFrame(int nr);
	void start(int *cycle_array, int size, bool loop, int endframe);
	void startRandom(int *cycle_array, int size, bool loop, int endframe);
	void update();
	int getFrame() const;
	bool isFinished() const;
	int getIndex() const;
};


#endif // INCLUDE_ANIMATION_H_
