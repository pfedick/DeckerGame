#include <ppl7.h>
#include <ppl7-grafix.h>
#include "objects.h"
#include "player.h"
#include "decker.h"

namespace Decker::Objects {


Representation Crate::representation()
{
	return Representation(Spriteset::Crates, 4);
}

Crate::Crate()
	: Decker::Objects::Object(Type::Crate)
{
	sprite_set=Spriteset::Crates;
	sprite_no=4;
	sprite_no_representation=4;
	collisionDetection=true;
	pixelExactCollision=false;
	color_mod.setColor(ppl7::rand(220, 250),
		ppl7::rand(220, 250),
		ppl7::rand(220, 250),
		255);
	updateBoundary();
	audio=NULL;
	playerIsNearForDrag=false;
	playerIsDragging=false;
	last_drag_collision_frame=0;
	alwaysUpdate=false;
}

Crate::~Crate()
{
	if (audio) {
		getAudioPool().stopInstace(audio);
		delete audio;
		audio=NULL;
	}
}

size_t Crate::save(unsigned char* buffer, size_t size) const
{
	size_t bytes=Object::save(buffer, size);
	if (!bytes) return 0;
	ppl7::Poke8(buffer + bytes, 1);		// Object Version
	ppl7::Poke8(buffer + bytes + 1, sprite_no);
	int flags=0;
	ppl7::Poke8(buffer + bytes + 2, flags);
	ppl7::Poke32(buffer + bytes + 3, color_mod.color());
	return bytes + 7;
}

size_t Crate::saveSize() const
{
	return Object::saveSize() + 7;
}

size_t Crate::load(const unsigned char* buffer, size_t size)
{
	size_t bytes=Object::load(buffer, size);
	if (bytes == 0 || size < bytes + 1) return 0;
	int version=ppl7::Peek8(buffer + bytes);
	if (version != 1) return 0;
	sprite_no=ppl7::Peek8(buffer + bytes + 1);
	sprite_no_representation=sprite_no;
	//int flags=ppl7::Peek8(buffer + bytes + 2);  // not used yet
	color_mod.setColor(ppl7::Peek32(buffer + bytes + 3));
	updateBoundary();
	return size;
}


void Crate::handleDrag(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation)
{
	uint64_t this_frame=GetFrameNo();
	//ppl7::PrintDebug("Crate::handleDrag, this frame: %llu, last col: %llu, diff: %llu\n", this_frame, last_drag_collision_frame, this_frame - last_drag_collision_frame);

	if (this_frame - last_drag_collision_frame > 15 || player.velocity_move.y != 0.0f || (
		player.movement != Player::PlayerMovement::Walk && player.movement != Player::PlayerMovement::Run)) {
		playerIsDragging=false;
		playerIsNearForDrag=false;
		return;
	}
	if (player.velocity_move.x != 0.0f) {
		if (player.x < p.x && player.velocity_move.x>0.0f) {
			velocity.x=2.0f;
			playerIsDragging=true;
		} else if (player.x < p.x && player.velocity_move.x>0.0f) {
			velocity.x=0;
			playerIsDragging=false;
			playerIsNearForDrag=false;
		} else if (player.x > p.x && player.velocity_move.x < 0.0f) {
			velocity.x=-2.0f;
			playerIsDragging=true;
		} else if (player.x > p.x && player.velocity_move.x > 0.0f) {
			velocity.x=0;
			playerIsDragging=false;
			playerIsNearForDrag=false;
		}
	} else {
		velocity.x=0;
		playerIsDragging=false;
		playerIsNearForDrag=false;
	}


	if (playerIsDragging) {
		int blockcount=0;
		int xx=0;
		if (velocity.x < 0) xx=boundary.left() - 2;
		else if (velocity.x > 0) xx=boundary.right() + 2;
		for (int y=boundary.top();y < boundary.bottom();y+=8) {
			if (ttplane.getType(ppl7::grafix::Point(xx, y)) == TileType::Blocking) blockcount++;
		}
		if (blockcount > 0) {
			//ppl7::PrintDebug("crate is blocked [%d]\n", blockcount);
			velocity.x=0.0f;
			playerIsDragging=false;
			//player.stand();
		}
	}

}


void Crate::update(double time, TileTypePlane& ttplane, Player& player, float frame_rate_compensation)
{
	int blockcount=0;

	if (playerIsNearForDrag) handleDrag(time, ttplane, player, frame_rate_compensation);




	AudioPool& audiopool=getAudioPool();
	if (playerIsDragging == true && audio == NULL) {
		audio=audiopool.getInstance(AudioClip::crate_loop);
		if (audio) {
			audio->setVolume(0.6f);
			audio->setAutoDelete(false);
			audio->setLoop(true);
			audio->setPositional(p, 1200);
			audiopool.playInstance(audio);
		}
	} else if (playerIsDragging == false && audio != NULL) {
		audiopool.stopInstace(audio);
		delete audio;
		audio=NULL;
	}

	if (audio) audio->setPositional(p, 1600);



	ppl7::grafix::PointF old_p=p;
	p+=velocity;
	if (!playerIsDragging) velocity.x=0.0f;
	blockcount=0;
	for (int x=boundary.left();x < boundary.right();x+=8) {
		if (ttplane.getType(ppl7::grafix::Point(x, p.y)) == TileType::Blocking) blockcount++;
	}
	if (blockcount == 0) {
		velocity.y+=0.2 * frame_rate_compensation;
		if (velocity.y > 16.0f)velocity.y=16.0f;
	} else {
		if (velocity.y > 0.0f) {
			audiopool.playOnce(AudioClip::crate_falling, p, 1600, 1.0f);
		}
		velocity.y=0.0f;
		while (true) {
			blockcount=0;
			for (int x=boundary.left();x < boundary.right();x+=8) {
				if (ttplane.getType(ppl7::grafix::Point(x, p.y)) == TileType::Blocking) blockcount++;
			}
			if (blockcount == 0) {
				p.y++;
				break;
			}
			p.y--;
		}
	}
	updateBoundary();

	std::list<Object*> object_list;
	GetObjectSystem()->detectObjectCollision(this, object_list);
	std::list<Object*>::iterator it;
	ppl7::grafix::PointF movement=p - old_p;
	for (it=object_list.begin();it != object_list.end();++it) {
		if ((*it)->type() == Decker::Objects::Type::Skeleton) {
			if (velocity.y > 1) ((Decker::Objects::Skeleton*)(*it))->die();

		} else if ((*it)->type() == Decker::Objects::Type::Crate) {
			ObjectCollision col(this, (*it));
			if (movement.y > 0.0f && col.objectBottom(10)) {
				p.y=old_p.y;
				if (velocity.y > 1.0f) audiopool.playOnce(AudioClip::crate_falling, p, 1600, 1.0f);
				velocity.y=0;

				updateBoundary();
				col.update();

			}
			/*
			if (this->id == 188) {
				ppl7::PrintDebug("crate movement: %0.3f:%0.3f, collision: bottom: %d, top: %d, left: %d, right: %d\n",
					movement.x, movement.y, (int)col.objectBottom(10), (int)col.objectTop(10),
					(int)col.objectLeft(10), (int)col.objectRight(10));
			}
			*/
			if (movement.x < 0.0f && col.objectLeft(10)) p.x=old_p.x;
			if (movement.x > 0.0f && col.objectRight(10)) p.x=old_p.x;
			//p=old_p;
			updateBoundary();
		}
	}
}


void Crate::handleCollision(Player* player, const Collision& collision)
{
	/*
	ObjectSystem* objs=GetObjectSystem();
	const std::list<ppl7::grafix::Point>& checkpoints=player->getCollisionCheckpoints();
	while (objs->checkCollisionWithObject(checkpoints,this)
	GetObjectSystem()->checkCollisionWithObject()
	*/
	/*
	player->x-=player->velocity_move.x;
	player->y-=player->velocity_move.y;
	return;
	*/

	Collision col_recheck(collision);
	// We set a fixed size bounding box for the player
	col_recheck.bounding_box_player.x1=player->x - TILE_WIDTH;
	col_recheck.bounding_box_player.x2=player->x + TILE_WIDTH;
	//ppl7::PrintDebug("BoundingBox Player: %d:%d - %d:%d\n", col_recheck.bounding_box_player.x1, col_recheck.bounding_box_player.y1, col_recheck.bounding_box_player.x2, col_recheck.bounding_box_player.y2);
	//ppl7::PrintDebug("BoundingBox Object: %d:%d - %d:%d\n", col_recheck.bounding_box_object.x1, col_recheck.bounding_box_object.y1, col_recheck.bounding_box_object.x2, col_recheck.bounding_box_object.y2);
	//ppl7::PrintDebug("Intersection:       %d:%d - %d:%d\n", col_recheck.bounding_box_intersection.x1, col_recheck.bounding_box_intersection.y1, col_recheck.bounding_box_intersection.x2, col_recheck.bounding_box_intersection.y2);
	bool col=true;
	int tolerance=40;
	while (col) {
		col=false;
		col_recheck.bounding_box_object=boundary;
		col_recheck.bounding_box_player=player->getBoundingBox();
		if (col_recheck.objectTop(tolerance)) {
			player->fallstart=0.0f;
			player->y=boundary.y1;
			player->velocity_move.y=0;
			player->setStandingOnObject(this);
		} else if (col_recheck.objectBottom(tolerance)) {
			player->setZeroVelocity();
			player->y++;
			col=true;
		}
	}
	//if (player->y > col_recheck.bounding_box_object.x1 + tolerance) {
	if (!col) {

		if (col_recheck.objectRight(tolerance)) {
			player->x=col_recheck.bounding_box_object.x2 + 30;
			/*
			if (player->velocity_move.x < 0) player->x-=player->velocity_move.x;
			else player->x++;
			player->velocity_move.x=0;
			updateBoundary();
			*/
			if (player->y > p.y - 48 && player->y < p.y + 32) {
				playerIsNearForDrag=true;
				last_drag_collision_frame=GetFrameNo();
				//if (player->velocity_move.x < 0.0f) velocity.x=-2.0f * collision.frame_rate_compensation;
			}
			col=true;
		}

		if (col_recheck.objectLeft(tolerance)) {
			player->x=col_recheck.bounding_box_object.x1 - 30;
			/*
			if (player->velocity_move.x > 0) player->x-=player->velocity_move.x;
			else player->x--;
			player->velocity_move.x=0;
			*/
			if (player->y > p.y - 48 && player->y < p.y + 32) {
				playerIsNearForDrag=true;
				last_drag_collision_frame=GetFrameNo();
			}
				//if (player->velocity_move.x > 0.0f) velocity.x=2.0f * collision.frame_rate_compensation;
			col=true;
		}
	}
}


class CrateDialog : public Decker::ui::Dialog
{
private:
	ppltk::ComboBox* crate_style;
	Decker::ui::ColorSliderWidget* color;


	Crate* object;

public:
	CrateDialog(Crate* object);
	//void toggledEvent(ppltk::Event* event, bool checked) override;
	void valueChangedEvent(ppltk::Event* event, int value) override;
	//void valueChangedEvent(ppltk::Event* event, int64_t value) override;

};

void Crate::openUi()
{
	CrateDialog* dialog=new CrateDialog(this);
	GetGameWindow()->addChild(dialog);

}

CrateDialog::CrateDialog(Crate* object)
	: Decker::ui::Dialog(570, 480, Dialog::Buttons::OK)
{
	this->object=object;
	this->setWindowTitle(ppl7::ToString("Crate, ID: %d", object->id));
	ppl7::grafix::Rect client=clientRect();
	int y=0;
	addChild(new ppltk::Label(0, y, 100, 30, "Crate-Style: "));
	crate_style=new ppltk::ComboBox(100, y, 300, 30);
	crate_style->add("Big Barrel", "0");
	crate_style->add("Small Barrel", "1");
	crate_style->add("Crate low 1", "2");
	crate_style->add("Crate low 2", "3");
	crate_style->add("Crate hight 1", "4");
	crate_style->add("Crate hight 2", "5");
	crate_style->add("Big Chest", "6");
	crate_style->add("Big Crate", "7");
	crate_style->setCurrentIdentifier(ppl7::ToString("%d", static_cast<int>(object->sprite_no)));
	crate_style->setEventHandler(this);
	addChild(crate_style);
	y+=40;

	addChild(new ppltk::Label(0, y, 80, 30, "Color modification:"));
	y+=30;
	color=new Decker::ui::ColorSliderWidget(0, y, client.width(), 100);
	color->setEventHandler(this);
	color->setColor(object->color_mod);
	color->setColorPreviewSize(64, 90);
	this->addChild(color);
	y+=110;
}


void CrateDialog::valueChangedEvent(ppltk::Event* event, int value)
{
	ppltk::Widget* widget=event->widget();
	if (widget == crate_style) {
		object->sprite_no=crate_style->currentIdentifier().toInt();
		object->sprite_no_representation=object->sprite_no;
		object->updateBoundary();
	} else if (widget == color) {
		object->color_mod=color->color();
	}
}


}	// EOF namespace Decker::Objects
