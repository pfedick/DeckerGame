#include <stdio.h>
#include <stdlib.h>
#include "decker.h"
#include "waynet.h"


WayPoint::WayPoint(uint16_t x, uint16_t y)
{
	this->x=x;
	this->y=y;
}

WayPoint::WayPoint(uint32_t id)
{
	this->id=id;
}



void WayPoint::addConnection(const Connection &conn)
{
	if (connection_map.size()<255)
		connection_map.insert(std::pair<uint32_t,Connection>(conn.target,conn));
}

void WayPoint::deleteConnection(uint32_t target)
{
	connection_map.erase(target);
}

Connection::Connection(ConnectionType type, uint32_t target, uint8_t cost)
{
	this->type=type;
	this->target=target;
	this->cost=cost;
}

Waynet::Waynet()
{
	selection=0;
}

Waynet::~Waynet()
{
	clear();
}

void Waynet::clear()
{
	waypoints.clear();
	selection=0;
}

void Waynet::draw(SDL_Renderer *renderer, const ppl7::grafix::Rect &viewport, const ppl7::grafix::Point &worldcoords) const
{
	std::map<uint32_t,WayPoint>::const_iterator it;
	int width=viewport.width();
	int height=viewport.height();
	ppl7::grafix::Point coords(viewport.x1-worldcoords.x, viewport.y1-worldcoords.y);
	SDL_Rect rect;
	rect.w=TILE_WIDTH;
	rect.h=TILE_HEIGHT;
	for (it=waypoints.begin();it!=waypoints.end();++it) {
		const WayPoint &wp=it->second;
		int x=wp.x*TILE_WIDTH-worldcoords.x;
		int y=wp.y*TILE_HEIGHT-worldcoords.y;
		if (x>0 && y>0 && x<width && y<height) {
			std::map<uint32_t,Connection>::const_iterator cit;
			SDL_SetRenderDrawColor(renderer,255,255,0,255);	// yellow
			for (cit=wp.connection_map.begin();cit!=wp.connection_map.end();++cit) {
				WayPoint p2(cit->second.target);
				SDL_RenderDrawLine(renderer, coords.x+wp.x*TILE_WIDTH,
						coords.y+wp.y*TILE_HEIGHT,
						coords.x+p2.x*TILE_WIDTH+TILE_WIDTH-1,
						coords.y+p2.y*TILE_HEIGHT+TILE_HEIGHT-1);

			}


			rect.x=coords.x+wp.x*TILE_WIDTH;
			rect.y=coords.y+wp.y*TILE_HEIGHT;
			if (wp.id==selection) {
				SDL_SetRenderDrawColor(renderer,128,255,0,255);	// green
			} else {
				SDL_SetRenderDrawColor(renderer,0,160,0,255);	// green
			}
			SDL_RenderFillRect(renderer, &rect);
			SDL_SetRenderDrawColor(renderer,0,0,0,255);	// black
			SDL_RenderDrawRect(renderer, &rect);
		}
	}
}

void Waynet::save(ppl7::FileObject &file, unsigned char id) const
{
	if (waypoints.size()==0) return;
	std::map<uint32_t,WayPoint>::const_iterator it;
	size_t buffersize=5;
	for (it=waypoints.begin();it!=waypoints.end();++it) {
		buffersize+=5+it->second.connection_map.size()*6;
	}
	unsigned char *buffer=(unsigned char*)malloc(buffersize);
	ppl7::Poke32(buffer+0,0);
	ppl7::Poke8(buffer+4,id);
	size_t p=5;
	for (it=waypoints.begin();it!=waypoints.end();++it) {
		const WayPoint &wp=it->second;
		ppl7::Poke32(buffer+p,it->first);
		ppl7::Poke8(buffer+p+4,wp.connection_map.size());
		p+=5;
		std::map<uint32_t,Connection>::const_iterator cit;
		for (cit=wp.connection_map.begin();cit!=wp.connection_map.end();++cit) {
			const Connection &conn=cit->second;
			ppl7::Poke32(buffer+p,conn.target);
			ppl7::Poke8(buffer+p+4,conn.type);
			ppl7::Poke8(buffer+p+5,conn.cost);
			p+=6;
		}
	}
	ppl7::Poke32(buffer+0,p);
	file.write(buffer,p);
	free(buffer);
}

void Waynet::load(const ppl7::ByteArrayPtr &ba)
{
	clear();
	size_t p=0;
	const char *buffer=ba.toCharPtr();
	while (p<ba.size()) {
		WayPoint wp(ppl7::Peek32(buffer+p));
		int count=ppl7::Peek8(buffer+p+4);
		p+=5;
		for (int i=0;i<count;i++) {
			wp.addConnection(Connection((Connection::ConnectionType)ppl7::Peek8(buffer+p+4),
					ppl7::Peek32(buffer+p),
					ppl7::Peek8(buffer+p+5)));
			p+=6;
		}
		addPoint(wp);
	}
}

void Waynet::addPoint(const WayPoint &p1)
{
	waypoints.insert(std::pair<uint32_t,WayPoint>(p1.id,p1));
}

bool Waynet::hasPoint(const WayPoint &p1) const
{
	std::map<uint32_t,WayPoint>::const_iterator it;
	it=waypoints.find(p1.id);
	if (it!=waypoints.end()) return true;
	return false;
}

void Waynet::deletePoint(const WayPoint &p1)
{
	// first: erase any connection targeting the point to delete
	std::map<uint32_t,WayPoint>::iterator it;
	for (it=waypoints.begin();it!=waypoints.end();++it) {
		WayPoint &wp=it->second;
		wp.connection_map.erase(p1.id);
	}
	// second: delete the point itself
	waypoints.erase(p1.id);
	if (selection==p1.id) selection=0;
}

void Waynet::addConnection(const WayPoint &source, const Connection &conn)
{
	std::map<uint32_t,WayPoint>::iterator it;
	it=waypoints.find(source.id);
	if (it!=waypoints.end()) {
		it->second.addConnection(conn);
	}
}

void Waynet::deleteConnection(const WayPoint &source, const WayPoint &target)
{
	std::map<uint32_t,WayPoint>::iterator it;
	it=waypoints.find(source.id);
	if (it!=waypoints.end()) {
		it->second.deleteConnection(target.id);
	}
}

void Waynet::clearSelection()
{
	selection=0;
}

bool Waynet::hasSelection() const
{
	if (selection!=0) return true;
	return false;
}
WayPoint Waynet::getSelection() const
{
	return WayPoint(selection);
}

void Waynet::setSelection(const WayPoint &source)
{
	selection=source.id;
}

bool Waynet::hasConnection(const WayPoint &source, const WayPoint &target) const
{
	std::map<uint32_t,WayPoint>::const_iterator it;
	it=waypoints.find(source.id);
	if (it==waypoints.end()) return false;
	std::map<uint32_t,Connection>::const_iterator cit;
	cit=it->second.connection_map.find(target.id);
	if (cit==it->second.connection_map.end()) return false;
	return true;
}



void Waynet::findWay()
{

}
