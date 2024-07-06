#ifndef INCLUDE_PLAYER_H_
#define INCLUDE_PLAYER_H_
#include <ppl7-grafix.h>

struct SDL_Renderer;
class SpriteTexture;

#include "animation.h"
#include "physic.h"
#include "translate.h"

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
	double last_fullspeed;
	float animation_speed;
	double idle_timeout;
	double last_aircheck;
	Game* game;

	AnimationCycle animation;
	ppl7::grafix::Color color_modulation;



	ppl7::grafix::Point lastSavePoint;

	std::map<int, Decker::Objects::Representation> Inventory;
	std::map<int, size_t>object_counter;
	std::set<int>SpecialObjects;
	std::set<uint16_t> spokenText;
	bool godmode;
	bool dead;
	bool visible;
	bool autoWalk;
	bool waterSplashPlayed;
	bool expressionJump;
	bool talkie;
	double airStart;
	double voiceDamageCooldown;
	double startIdle;
	double nextIdleSpeech;
	double nextPhonetic;
	float frame_rate_compensation;
	bool greetingPlayed;
	bool flashlightOn;
	double actionToggleCooldown;

	AudioInstance* ambient_sound;
	AudioInstance* voice;
	AudioSample voice_sample;
	AudioClip::Id ambient_playing;
	LightObject flashlight1, flashlight2, flashlight3, flashlight2_ladder;

	enum class ParticleReason
	{
		None=0,
		Drowned,
		Burning,
		Smashed,
		SmashedSideways
	};
	double particle_end_time, next_particle_birth;
	ParticleReason particle_reason;

	Decker::Objects::Object* hackingObject;
	int hackingState;
	double hacking_end;
	ppl7::String phonetics;
	std::list<ppl7::grafix::Point> collision_checkpoints;


	class FlashLightPivot
	{
	public:
		FlashLightPivot(int x, int y, float angle);
		int x;
		int y;
		float angle;
	};
	std::map<int, FlashLightPivot> flashlight_pivots;
	void initFlashLightPivots();



	void turn(PlayerOrientation target);
	void crawlTurn(PlayerOrientation target);
	void splashIntoWater(float gravity);
	void emmitParticles(double time);

	void moveOutOfWater(float angel, float speed);
	void checkCollisionWithObjects(Decker::Objects::ObjectSystem* objects, float frame_rate_compensation);
	void checkCollisionWithWorld(const TileTypePlane& world);

	void handleDiving(double time, const TileTypePlane& world, Decker::Objects::ObjectSystem* objects, float frame_rate_compensation);
	bool hackingInProgress();

	void handleKeyboardWhileJumpOrFalling(double time, const TileTypePlane& world, Decker::Objects::ObjectSystem* objects, float frame_rate_compensation);
	void handleKeyboardWhileSwimming(double time, const TileTypePlane& world, Decker::Objects::ObjectSystem* objects, float frame_rate_compensation);
	void handleKeyboardWhileCrawling(double time, const TileTypePlane& world, Decker::Objects::ObjectSystem* objects, float frame_rate_compensation);

	void playSoundOnAnimationSprite();
	void checkActivationOfObjectsInRange(Decker::Objects::ObjectSystem* objectsystem);
	void toggleFlashlight();
	void idleJokes(double time);
	void playPhonetics();
	void initFlashLight();
public:
	float x, y;
	int points, lifes;
	float health;
	float air;
	float maxair;

	// is updated every frame
	ppl7::grafix::Point WorldCoords;
	ppl7::grafix::Rect Viewport;


	explicit Player(Game* game);
	~Player();
	ppl7::grafix::PointF position() const;
	void stand();
	void jumpExpression();
	Keys getKeyboardMatrix(const unsigned char* state=NULL);
	void resetState();
	void resetLevelObjects();
	void setZeroVelocity();
	void setVisible(bool flag);
	void enableTalkie(bool flag);
	void addPoints(int points);
	void addHealth(int points);
	void addAir(float seconds);
	void addLife(int lifes);
	void addSpecialObject(int type);
	bool hasSpecialObject(int type) const;
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
	void addFlashlightToLightSystem(LightSystem& lights);
	void drawCollision(SDL_Renderer* renderer, const ppl7::grafix::Rect& viewport, const ppl7::grafix::Point& worldcoords) const;
	void move(int x, int y);
	ppl7::grafix::Rect getBoundingBox() const;
	void setStandingOnObject(Decker::Objects::Object* object);
	void setAutoWalk(bool enabled);
	bool isAutoWalk() const;
	void startEmittingParticles(double endtime, ParticleReason reason);
	void startHacking(Decker::Objects::Object* object);
	void update(double time, const TileTypePlane& world, Decker::Objects::ObjectSystem* objects, float frame_rate_compensation);

	void speak(VoiceGeorge::Id id, float volume=0.7f, const ppl7::String& text=ppl7::String(), const ppl7::String& phonetics=ppl7::String());
	bool speak(uint16_t id, float volume=0.7f);
	bool isSpeaking() const;
	bool hasSpoken(uint16_t id) const;
	bool isFlashlightOn() const;
	void enableFlashlight(bool enable);

	const std::list<ppl7::grafix::Point>& getCollisionCheckpoints() const;

};
#endif /* INCLUDE_PLAYER_H_ */
