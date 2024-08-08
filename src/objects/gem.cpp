#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "audiopool.h"
#include "player.h"

namespace Decker::Objects {

static int diamond_rotate[]={ 4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,
	21,22,23,24,25,26,27,28,29,30,31,32,33 };
static int crystal_rotate[]={ 120,121,122,123,124,125,126,127,128,129,130,
	131,132,133,134,135,136,137,138,139,140,141,142,143,144,145,146,
	147,148,149 };

struct color_struct {
	uint8_t r, g, b; ;
} color_struct;
static struct color_struct random_colors[]={
	{192,64,255},
	{64,64,255},
	{128,255,64},
	{255,128,255},
	{192,128,64}
};

Representation GemReward::representation()
{
	return Representation(Spriteset::GenericObjects, 4);
}

GemReward::GemReward()
	: Object(Type::ObjectType::Diamond)
{
	sprite_set=Spriteset::GenericObjects;
	animation.startRandom(diamond_rotate, sizeof(diamond_rotate) / sizeof(int), true, 0);
	next_animation=0.0f;
	collisionDetection=true;
	alwaysUpdate=false;
	sprite_no_representation=4;
	int r=ppl7::rand(0, 4);
	light_glow.color.set(random_colors[r].r, random_colors[r].g, random_colors[r].b, 255);
	light_glow.sprite_no=0;
	light_glow.scale_x=0.3f;
	light_glow.scale_y=0.3f;
	light_glow.intensity=192;
	light_glow.plane=static_cast<int>(LightPlaneId::Player);
	light_glow.playerPlane= static_cast<int>(LightPlayerPlaneMatrix::Player) | static_cast<int>(LightPlayerPlaneMatrix::Back);

}

void GemReward::update(double time, TileTypePlane&, Player&, float)
{
	if (!enabled) return;
	if (!isInViewport) return;
	if (time > next_animation) {
		next_animation=time + 0.056f;
		animation.update();
		int new_sprite=animation.getFrame();
		if (new_sprite != sprite_no) {
			sprite_no=new_sprite;
			updateBoundary();
		}
	}
	light_glow.x=p.x;
	int yy=sprite_no - 4;
	if (yy > 15) yy=30 - yy;
	light_glow.y=p.y - 15 - yy * 2;

	light_glow.scale_x=0.3f + yy * 0.02;
	light_glow.scale_y=0.3f;


	LightSystem& lights=GetGame().getLightSystem();
	lights.addObjectLight(&light_glow);
}

void GemReward::handleCollision(Player* player, const Collision&)
{
	enabled=false;
	if (spawned) deleteDefered=true;
	player->addPoints(50);
	player->countObject(type());
	AudioPool& audio=getAudioPool();
	audio.playOnce(AudioClip::coin2, 0.3f);
}



Representation CrystalReward::representation()
{
	return Representation(Spriteset::GenericObjects, 0);
}

CrystalReward::CrystalReward()
	: Object(Type::ObjectType::Crystal)
{
	sprite_set=Spriteset::GenericObjects;
	animation.startRandom(crystal_rotate, sizeof(crystal_rotate) / sizeof(int), true, 0);
	next_animation=0.0f;
	collisionDetection=true;
	alwaysUpdate=false;
	int r=ppl7::rand(0, 4);
	light_glow.color.set(random_colors[r].r, random_colors[r].g, random_colors[r].b, 255);

	light_glow.sprite_no=0;
	light_glow.scale_x=0.3f;
	light_glow.scale_y=0.3f;
	light_glow.intensity=192;
	light_glow.plane=static_cast<int>(LightPlaneId::Player);
	light_glow.playerPlane= static_cast<int>(LightPlayerPlaneMatrix::Player) | static_cast<int>(LightPlayerPlaneMatrix::Back);

}

void CrystalReward::update(double time, TileTypePlane&, Player&, float)
{
	if (!enabled) return;
	if (!isInViewport) return;
	if (time > next_animation) {
		next_animation=time + 0.056f;
		animation.update();
		int new_sprite=animation.getFrame();
		if (new_sprite != sprite_no) {
			sprite_no=new_sprite;
			updateBoundary();
		}
	}
	light_glow.x=p.x;
	int yy=sprite_no - 120;
	if (yy > 15) yy=30 - yy;
	light_glow.y=p.y - 15 - yy * 2;

	light_glow.scale_x=0.4f + yy * 0.02;
	light_glow.scale_y=0.4f;


	LightSystem& lights=GetGame().getLightSystem();
	lights.addObjectLight(&light_glow);
}

void CrystalReward::handleCollision(Player* player, const Collision&)
{
	enabled=false;
	if (spawned) deleteDefered=true;
	player->addPoints(100);
	player->countObject(type());
	AudioPool& audio=getAudioPool();
	int sample=ppl7::rand(0, 2);
	switch (sample) {
	case 0:	audio.playOnce(AudioClip::crystal2, 0.4f); break;
	case 1: audio.playOnce(AudioClip::crystal3, 0.4f); break;
	case 2:	audio.playOnce(AudioClip::crystal, 0.4f);
	}

}


}	// EOF namespace Decker::Objects
