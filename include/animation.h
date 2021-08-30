#ifndef INCLUDE_ANIMATION_H_
#define INCLUDE_ANIMATION_H_

class AnimationCycle
{
private:
	int *cycle;
	int index;
	int size;
	int endframe;
	int seq_start, seq_end;
	bool loop;
	bool finished;

public:
	AnimationCycle();
	void setStaticFrame(int nr);
	void start(int *cycle_array, int size, bool loop, int endframe);
	void start(const AnimationCycle &other);
	void startRandom(int *cycle_array, int size, bool loop, int endframe);
	void startSequence(int start, int end, bool loop, int endframe);
	void update();
	int getFrame() const;
	bool isFinished() const;
	int getIndex() const;
};


#endif // INCLUDE_ANIMATION_H_
