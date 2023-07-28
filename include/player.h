#ifndef INCLUDE_PLAYER_H_
#define INCLUDE_PLAYER_H_
#include <ppl7-grafix.h>

struct SDL_Renderer;
class SpriteTexture;

#include "animation.h"
#include "physic.h"

class TileTypePlane;
class Game;
namespace Decker {
namespace Objects {
class Object;
class ObjectSystem;
class Representation;
}
} // EOF namespace Decker::Objects



class Player : public Physic
{
private:
	const SpriteTexture* sprite_resource;
	const SpriteTexture* tiletype_resource;

	double next_keycheck;
	double next_animation;
	double idle_timeout;
	double last_aircheck;
	Game* game;

	AnimationCycle animation;
	ppl7::grafix::Color color_modulation;



	ppl7::grafix::Point lastSavePoint;

	std::map<int, Decker::Objects::Representation> Inventory;
	std::map<int, size_t>object_counter;
	bool godmode;
	bool dead;
	bool visible;
	bool autoWalk;
	bool waterSplashPlayed;

	AudioInstance* ambient_sound;
	AudioClip::Id ambient_playing;

	enum class ParticleReason
	{
		None=0,
		Drowned,
		Burning
	};
	double particle_end_time, next_particle_birth;
	ParticleReason particle_reason;



	void turn(PlayerOrientation target);
	void splashIntoWater(float gravity);
	void emmitParticles(double time);

	void moveOutOfWater(float angel, float speed);
	void checkCollisionWithObjects(Decker::Objects::ObjectSystem* objects);
	void checkCollisionWithWorld(const TileTypePlane& world);

	void handleDiving(double time, const TileTypePlane& world, Decker::Objects::ObjectSystem* objects, float frame_rate_compensation);

	void handleKeyboardWhileJumpOrFalling(double time, const TileTypePlane& world, Decker::Objects::ObjectSystem* objects, float frame_rate_compensation);
	void handleKeyboardWhileSwimming(double time, const TileTypePlane& world, Decker::Objects::ObjectSystem* objects, float frame_rate_compensation);

public:
	float x, y;
	int points, health, lifes;
	float air;
	float maxair;

	// is updated every frame
	ppl7::grafix::Point WorldCoords;
	ppl7::grafix::Rect Viewport;


	explicit Player(Game* game);
	~Player();
	ppl7::grafix::PointF position() const;
	void stand();
	int getKeyboardMatrix(const unsigned char* state=NULL);
	void resetState();
	void resetLevelObjects();
	void setZeroVelocity();
	void setVisible(bool flag);
	void addPoints(int points);
	void addHealth(int points);
	void addAir(float seconds);
	void addLife(int lifes);
	void countObject(int type);
	size_t getObjectCount(int type) const;
	void dropHealth(float points, HealthDropReason reason=HealthDropReason::Unknown);
	void addInventory(int object_id, const Decker::Objects::Representation& repr);
	bool isInInventory(int object_id) const;
	bool isDead() const;
	void dropLifeAndResetToLastSavePoint();
	void setGodMode(bool enabled);
	void setSavePoint(const ppl7::grafix::Point& p);
	void setSpriteResource(const SpriteTexture& resource);
	void setTileTypeResource(const SpriteTexture& resource);
	void draw(SDL_Renderer* renderer, const ppl7::grafix::Rect& viewport, const ppl7::grafix::Point& worldcoords) const;
	void drawCollision(SDL_Renderer* renderer, const ppl7::grafix::Rect& viewport, const ppl7::grafix::Point& worldcoords) const;
	void move(int x, int y);
	ppl7::grafix::Rect getBoundingBox() const;
	void setStandingOnObject(Decker::Objects::Object* object);
	void setAutoWalk(bool enabled);
	bool isAutoWalk() const;
	void startEmittingParticles(double endtime, ParticleReason reason);
	void update(double time, const TileTypePlane& world, Decker::Objects::ObjectSystem* objects, float frame_rate_compensation);



};
#endif /* INCLUDE_PLAYER_H_ */
