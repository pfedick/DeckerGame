#include "decker.h"
#include "physic.h"

static const char* movement_string[13]={"Unchanged", "Stand",
		"Turn", "Walk", "Run", "Pickup", "ClimbUp", "ClimbDown",
		"Jump","Falling", "Slide", "Floating", "Dead"};

static const char* orientation_string[4]={"Left",
		"Right", "Front", "Back"};


Physic::Physic()
{
	movement=Stand;
	orientation=Front;
	turnTarget=Front;
	acceleration_airstream=0.0f;
	acceleration_gravity=0.0f;
	gravity=0.0f;
	jump_climax=0.0f;
	acceleration_jump=0.0f;
	fallstart=0.0f;
	time=0.0f;
	for (int cy=0;cy<6;cy++) {
		for (int cx=0;cx<4;cx++) {
			collision_matrix[cx][cy]=TileType::NonBlocking;
		}
	}
	collision_at_pivoty[0]=0;
	collision_at_pivoty[1]=0;
	collision_at_pivoty[2]=0;
	for (int i=0;i<TileType::Type::MaxType;i++) collision_type_count[i]=0;
	player_stands_on_object=NULL;
	speed_walk=2.0f;
	speed_run=6.0f;
}

ppl7::String Physic::getState() const
{
	ppl7::String s;
	s.setf("%s:%s, velocity: %0.3f:%0.3f", movement_string[movement],
			orientation_string[orientation],
			velocity_move.x, velocity_move.y);
	return s;
}

Physic::PlayerMovement Physic::getMovement() const
{
	return movement;
}


bool Physic::updatePhysics(const TileTypePlane &world)
{
	if (movement==Jump || movement==Slide || movement==Dead) return false;
	bool match=false;
	if (collision_matrix[1][4]==TileType::NonBlocking && collision_matrix[2][4]==TileType::NonBlocking) {
		if (collision_matrix[1][5]==TileType::NonBlocking && collision_matrix[2][5]==TileType::NonBlocking) {
			if (!player_stands_on_object) {
				//printf ("gravity\n");
				if (acceleration_gravity<6.0f) acceleration_gravity+=0.05f;
				if (acceleration_gravity>6.0f) acceleration_gravity=6.0f;
				match=true;
			}
		}
	}
	if (acceleration_gravity>0.0f) {
		if (!match) {
			acceleration_gravity-=acceleration_gravity/2.0f;
			if (acceleration_gravity<0.0f) acceleration_gravity=0.0f;
		}
		gravity+=acceleration_gravity;
		if (gravity>16.0f) gravity=16.0f;
	}
	match=false;
	if (collision_matrix[1][4]==TileType::AirStream || collision_matrix[2][4]==TileType::AirStream) {
		//if (acceleration_airstream<8.0f) acceleration_airstream+=(0.1f+(acceleration_airstream/200.0f));
		if (acceleration_airstream<8.0f) acceleration_airstream+=0.02f;
		if (acceleration_airstream>8.0f) acceleration_airstream=8.0f;
		match=true;
		movement=Floating;
	}
	if (acceleration_airstream>0.0f) {
		if (!match) {
			acceleration_airstream-=acceleration_airstream/5.0f;
			if (acceleration_airstream<0.0f) acceleration_airstream=0.0f;
		}
		gravity-=acceleration_airstream;
		if (gravity<-8.0f) gravity=-8.0f;

	}
	if (collision_at_pivoty[1]==TileType::SteepRampLeft && movement!=Slide) {
		// Start Slide
		velocity_move.x=-((float)TILE_WIDTH/4.0f);
		velocity_move.y=((float)TILE_HEIGHT/4.0f);
		gravity=0.0f;
		acceleration_gravity=0.0f;
		movement=Slide;
		orientation=Left;
		return true;
	} else if (collision_at_pivoty[1]==TileType::SteepRampRight && movement!=Slide) {
		// Start Slide
		velocity_move.x=((float)TILE_WIDTH/4.0f);
		velocity_move.y=((float)TILE_HEIGHT/4.0f);
		gravity=0.0f;
		acceleration_gravity=0.0f;
		movement=Slide;
		orientation=Right;
		return true;
	}

	/*
	printf ("gravity: %2.3f, acceleration_gravity: %2.3f, acceleration_airstream: %2.3f, velocity_move.y=%0.3f\n",
			gravity, acceleration_gravity, acceleration_airstream, velocity_move.y);
	*/
	return false;
}


int Physic::detectFallingDamage(double time)
{
	if (movement!=Falling && fallstart>0.0f) {
		double falltime=time-fallstart;
		//printf ("Falling ended at %0.3f, falltime: %0.3f\n", time, falltime);
		if (falltime>1.1f) return 100;
		else if (falltime>1.0f) return 80;
		else if (falltime>0.9f) return 50;
		else if (falltime>0.8f) return 20;
		fallstart=0.0f;
	} else if (movement==Falling && fallstart==0.0f && gravity>15.0f) {
		//printf ("we start to fall at %0.3f\n",time);
		fallstart=time;
	} else if (movement==Falling && fallstart>0.0f && time-fallstart>10.0f) {
		// the player probably falls through the level
		return 100;
	}
	return 0;
}

bool Physic::isCollisionLeft() const
{
	for (int cy=1;cy<5;cy++) {
		if (collision_matrix[0][cy]==TileType::Blocking) {
			return true;
		}
	}
	return false;
}

bool Physic::isCollisionRight() const
{
	for (int cy=1;cy<5;cy++) {
		if (collision_matrix[3][cy]==TileType::Blocking) {
			return true;
		}
	}
	return false;
}


static TileType::Type filterRelevantTileTypes(TileType::Type t)
{
	if (t==TileType::EnemyBlocker) return TileType::NonBlocking;
	return t;
}

Physic::PlayerMovement Physic::checkCollisionWithWorld(const TileTypePlane &world, float &x, float &y)
{
	Physic::PlayerMovement new_movement=Unchanged;
	if (movement==Dead) return new_movement;
	for (int i=0;i<TileType::Type::MaxType;i++) collision_type_count[i]=0;
	//TileType::Type t;
	for (int cy=1;cy<5;cy++) {
		for (int cx=0;cx<4;cx++) {
			TileType::Type t=world.getType(ppl7::grafix::Point(x+(TILE_WIDTH/2)-2*TILE_WIDTH+(cx*TILE_WIDTH),
					y+(TILE_HEIGHT/2)-(5*TILE_HEIGHT)+(cy*TILE_HEIGHT)));
			collision_matrix[cx][cy]=filterRelevantTileTypes(t);
			collision_type_count[t]+=1;
		}
	}
	collision_at_pivoty[0]=filterRelevantTileTypes(world.getType(ppl7::grafix::Point(x, y-1)));
	collision_at_pivoty[1]=filterRelevantTileTypes(world.getType(ppl7::grafix::Point(x, y)));
	collision_at_pivoty[2]=filterRelevantTileTypes(world.getType(ppl7::grafix::Point(x, y-(TILE_WIDTH/2))));
	collision_matrix[1][4]=filterRelevantTileTypes(world.getType(ppl7::grafix::Point(x-(TILE_WIDTH/2), y-1)));
	collision_matrix[2][4]=filterRelevantTileTypes(world.getType(ppl7::grafix::Point(x+(TILE_WIDTH/2), y-1)));
	for (int cx=0;cx<4;cx++) {
		collision_matrix[cx][0]=filterRelevantTileTypes(world.getType(ppl7::grafix::Point(x+(TILE_WIDTH/2)-2*TILE_WIDTH+(cx*TILE_WIDTH),
				y-(TILE_HEIGHT*4))));

		collision_matrix[cx][5]=filterRelevantTileTypes(world.getType(ppl7::grafix::Point(x+(TILE_WIDTH/2)-2*TILE_WIDTH+(cx*TILE_WIDTH),
				y)));

	}
	if (movement==Slide) {
		if (orientation==Left) {
			if (collision_matrix[1][4]==TileType::Blocking) {
				new_movement=Stand;
			}
		} else {
			if (collision_matrix[2][4]==TileType::Blocking) {
				new_movement=Stand;
			}
		}
		while (world.getType(ppl7::grafix::Point(x,y-1))==TileType::Blocking) {
			y--;
		}
		return new_movement;
	}

	if (collision_at_pivoty[1]==TileType::Plate2h) {
		int th=TILE_HEIGHT/3;
		int ty=(((int)((y)/TILE_HEIGHT))*TILE_HEIGHT)+th;
		//printf ("Plate2h 1, y=%d, ty=%d\n", (int)y, ty);
		if (y<ty) {
			y+=3;
			if (y>ty) y=ty;
		} else if (y>ty) {
			y-=3;
			if (y<ty) y=ty;
		}
		//y=ty;
		if (movement==Falling || movement==Jump || movement==ClimbDown) new_movement=Stand;
		return new_movement;
	} else if (collision_at_pivoty[0]==TileType::Plate2h) {
		int th=TILE_HEIGHT/3;
		int ty=(((int)((y)/TILE_HEIGHT))*TILE_HEIGHT)+th-(2*th);
		//printf ("Plate2h 2, y=%d, ty=%d\n", (int)y, ty);
		//y=ty;
		if (movement==Falling || movement==Jump || movement==ClimbDown) new_movement=Stand;
		return new_movement;

	} else if (collision_at_pivoty[1]==TileType::Plate1h) {
		int th=TILE_HEIGHT/3;
		int ty=((int)((y-1)/TILE_HEIGHT))*TILE_HEIGHT+2*(th);
		if (y<ty) {
			y+=3;
			if (y>ty) y=ty;
		} else if (y>ty) {
			y-=3;
			if (y<ty) y=ty;
		}
		if (movement==Falling || movement==Jump || movement==ClimbDown) new_movement=Stand;
		return new_movement;
	} else if (collision_at_pivoty[0]==TileType::Plate1h) {
		int th=TILE_HEIGHT/3;
		int ty=((int)((y-1)/TILE_HEIGHT))*TILE_HEIGHT+2*th;
		if (y<ty) {
			y+=3;
			if (y>ty) y=ty;
		} else if (y>ty) {
			y-=3;
			if (y<ty) y=ty;
		}
		if (movement==Falling || movement==Jump || movement==ClimbDown) new_movement=Stand;
		return new_movement;
	} else if (collision_at_pivoty[1]==TileType::ShallowRampLeftUpper) {
		int tx=(int)x%TILE_WIDTH;
		int th=TILE_HEIGHT/2;
		int ty=(((int)((y)/TILE_HEIGHT))*TILE_HEIGHT)+th-(th*tx/TILE_WIDTH);
		//printf ("ShallowRampLeftUpper y=%d, ty=%d, tx=%d\n", (int)y, ty, tx);
		y=ty;
		if (movement==Falling || movement==Jump || movement==ClimbDown) new_movement=Stand;
		return new_movement;
	} else if (collision_at_pivoty[1]==TileType::ShallowRampLeftLower) {
		int tx=(int)x%TILE_WIDTH;
		int th=TILE_HEIGHT/2;
		int ty=(((int)((y)/TILE_HEIGHT))*TILE_HEIGHT)+TILE_HEIGHT-(th*tx/TILE_WIDTH);
		//printf ("ShallowRampLeftLower 1, y=%d, ty=%d, tx=%d\n", (int)y, ty, tx);
		y=ty;
		if (movement==Falling || movement==Jump || movement==ClimbDown) new_movement=Stand;
		return new_movement;
	} else if (collision_at_pivoty[0]==TileType::ShallowRampLeftLower) {
		int tx=(int)x%TILE_WIDTH;
		int th=TILE_HEIGHT/2;
		int ty=(((int)((y)/TILE_HEIGHT))*TILE_HEIGHT)+TILE_HEIGHT-(th*tx/TILE_WIDTH)-TILE_HEIGHT;
		//printf ("ShallowRampLeftLower 2, y=%d, ty=%d, tx=%d\n", (int)y, ty, tx);
		y=ty;
		if (movement==Falling || movement==Jump || movement==ClimbDown) new_movement=Stand;
		return new_movement;
	} else if (collision_at_pivoty[1]==TileType::ShallowRampRightUpper) {
		int tx=TILE_WIDTH-(int)x%TILE_WIDTH;
		int th=TILE_HEIGHT/2;
		int ty=(((int)((y)/TILE_HEIGHT))*TILE_HEIGHT)+th-(th*tx/TILE_WIDTH);
		//printf ("ShallowRampLeftUpper y=%d, ty=%d, tx=%d\n", (int)y, ty, tx);
		y=ty;
		if (movement==Falling || movement==Jump || movement==ClimbDown) new_movement=Stand;
		return new_movement;
	} else if (collision_at_pivoty[1]==TileType::ShallowRampRightLower) {
		int tx=TILE_WIDTH-(int)x%TILE_WIDTH;
		int th=TILE_HEIGHT/2;
		int ty=(((int)((y)/TILE_HEIGHT))*TILE_HEIGHT)+TILE_HEIGHT-(th*tx/TILE_WIDTH);
		//printf ("ShallowRampLeftLower 1, y=%d, ty=%d, tx=%d\n", (int)y, ty, tx);
		y=ty;
		if (movement==Falling || movement==Jump || movement==ClimbDown) new_movement=Stand;
		return new_movement;
	} else if (collision_at_pivoty[0]==TileType::ShallowRampRightLower) {
		int tx=TILE_WIDTH-(int)x%TILE_WIDTH;
		int th=TILE_HEIGHT/2;
		int ty=(((int)((y)/TILE_HEIGHT))*TILE_HEIGHT)+TILE_HEIGHT-(th*tx/TILE_WIDTH)-TILE_HEIGHT;
		//printf ("ShallowRampLeftLower 2, y=%d, ty=%d, tx=%d\n", (int)y, ty, tx);
		y=ty;
		if (movement==Falling || movement==Jump || movement==ClimbDown) new_movement=Stand;
		return new_movement;
	}

	if (collision_matrix[1][0]==TileType::Blocking || collision_matrix[2][0]==TileType::Blocking) {
		acceleration_gravity=0.1f;
		gravity=1.0f;
		velocity_move.y=1;
		acceleration_jump=0.0f;
		movement=Falling;
		//if (movement==Falling || movement==Jump || movement==ClimbDown) stand();
	}
	if (collision_matrix[1][5]==TileType::Blocking || collision_matrix[2][5]==TileType::Blocking
			|| collision_matrix[1][5]==TileType::Ladder || collision_matrix[2][5]==TileType::Ladder ) {
		if (gravity>0.0f || movement==Falling) {
			//printf ("col 2\n");
			acceleration_gravity=0.0f;
			gravity=0.0f;
			y=(y/TILE_HEIGHT)*TILE_HEIGHT;
			if (movement==Falling || movement==Jump || movement==ClimbDown) new_movement=Stand;
		}
	}
	if (collision_matrix[1][4]==TileType::Blocking || collision_matrix[2][4]==TileType::Blocking) {
		//printf ("col 1\n");
		while (world.getType(ppl7::grafix::Point(x-(TILE_WIDTH/2), y-1))==TileType::Blocking
				|| world.getType(ppl7::grafix::Point(x+(TILE_WIDTH/2), y-1))==TileType::Blocking) {
			y--;
		}
		velocity_move.x=0;
		velocity_move.y=0;
		acceleration_gravity=0.0f;
		gravity=0.0f;
		if (movement==ClimbDown) new_movement=Stand;
	}

	if (player_stands_on_object) {
		if ((movement==Falling || movement==Jump) && gravity>0.0f) {
			new_movement=Stand;
		}
		acceleration_gravity=0.0f;
		gravity=0.0f;

	}

	if (movement==Walk || movement==Run || movement==Jump || movement==Falling) {
		if (orientation==Left) {
			if (isCollisionLeft()) {
				velocity_move.x=0;
				//printf ("debug 3a\n");
				if (movement!=Jump && movement!=Falling) new_movement=Stand;
				//else movement=Falling;
			}
		} else if (orientation==Right) {
			if (isCollisionRight()) {
				velocity_move.x=0;
				//printf ("debug 3b\n");
				if (movement!=Jump && movement!=Falling) new_movement=Stand;
				//else movement=Falling;

			}
		}
	}
	return new_movement;
}



void Physic::updateMovement()
{
	if (movement==Slide || movement==Dead) return;
	if (movement==Walk) {
		if (orientation==Left) {
			velocity_move.x=-speed_walk;
		} else if (orientation==Right) {
			velocity_move.x=speed_walk;
		}
	} else if (movement==Run) {
		if (orientation==Left) {
			velocity_move.x=-speed_run;
		} else if (orientation==Right) {
			velocity_move.x=speed_run;
		}
	} else if (movement==ClimbUp) {
		velocity_move.y=-4;
	} else if (movement==ClimbDown) {
		velocity_move.y=4;
	} else if (movement==Jump) {
		//printf ("we are jumping... ");
		if (jump_climax>time) {
			if (acceleration_jump<4.0f) acceleration_jump+=0.1f;
			if (acceleration_jump>4.0f) acceleration_jump=4.0f;
			//printf ("under climax, accelerating %0.3f ", acceleration_jump);
			velocity_move.y-=acceleration_jump;
			if (velocity_move.y<-12.0f) velocity_move.y=-12.0f;
		} else {
			if (acceleration_jump>0) acceleration_jump-=acceleration_jump/1.5;
			if (acceleration_jump<0.5f) {
				acceleration_jump=0.5;
			}
			velocity_move.y+=acceleration_jump;
			if (velocity_move.y>-0.1f) {
				velocity_move.y=0.0f;
				acceleration_jump=0.0f;
				movement=Falling;
			}
		}
		/*
		printf ("Jump, climax in %0.3f s, acceleration=%0.3f, velocity=%0.3f\n",
				time<jump_climax?jump_climax-time:0.0f,
						acceleration_jump, velocity_move.y);
		*/
	} else if (movement==Falling) {
		if (velocity_move.y<-0.1f) velocity_move.y-=velocity_move.y/3.0f;
		if (velocity_move.y>-0.1f) velocity_move.y=0;
		//printf ("Player::updateMovement, velocity_move.y=%0.3f\n",velocity_move.y);

	} else {
		velocity_move.y=0;
		velocity_move.x=0;
	}
	if(gravity>10) movement=Falling;
}




