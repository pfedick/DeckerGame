#ifndef INCLUDE_ANIMATION_H_
#define INCLUDE_ANIMATION_H_

class AnimationCycle
{
private:
	int* cycle;
	int index;
	int size;
	int endframe;
	int seq_start, seq_end;
	bool loop;
	bool finished;
	float default_animation_speed;
	float current_animation_speed;

public:
	AnimationCycle();
	void setStaticFrame(int nr);
	void start(int* cycle_array, int size, bool loop, int endframe);
	void start(const AnimationCycle& other);
	void startRandom(int* cycle_array, int size, bool loop, int endframe);
	void startSequence(int start, int end, bool loop, int endframe);
	void startRandomSequence(int start, int end, bool loop, int endframe);
	void update();
	int getFrame() const;
	bool isFinished() const;
	int getIndex() const;
	void setSpeed(float seconds_per_frame);
	void setDefaultSpeed(float seconds_per_frame);
	void resetSpeed();
	float speed() const;

};


#endif // INCLUDE_ANIMATION_H_
