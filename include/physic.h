#ifndef INCLUDE_PHYSIC_H_
#define INCLUDE_PHYSIC_H_

class Velocity
{
public:
	Velocity(float x=0.0f, float y=0.0f) {
		this->x=x;
		this->y=y;
	}
	float x;
	float y;
	void stop() {
		x=0.0f;
		y=0.0f;
	}
};

#endif // INCLUDE_PHYSIC_H_
