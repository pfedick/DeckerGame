#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "decker.h"
#include "waynet.h"

Position::Position()
{
	id=0;
}
Position::Position(const Position& other)
{
	this->id=other.id;
}

Position::Position(uint32_t id)
{
	this->id=id;
}

Position::Position(uint16_t x, uint16_t y)
{
	this->x=x;
	this->y=y;
}

Position::operator uint32_t() const
{
	return id;
}

WayPoint::WayPoint(uint16_t x, uint16_t y)
{
	this->x=x;
	this->y=y;
}

WayPoint::WayPoint(uint32_t id)
{
	this->id=id;
}

WayPoint::WayPoint()
{
	this->id=0;
}


void WayPoint::addConnection(const Connection& conn)
{
	if (connection_map.size() < 255)
		connection_map.insert(std::pair<uint32_t, Connection>(conn.target, conn));
}

void WayPoint::deleteConnection(uint32_t target)
{
	connection_map.erase(target);
}

Connection::Connection()
{
	source.id=0;
	target.id=0;
	type=Connection::Invalid;
	cost=0;
}

Connection::Connection(const Position& source, const Position& target, ConnectionType type, uint8_t cost)
{
	this->source=source;
	this->target=target;
	this->type=type;
	this->cost=cost;
}

void Connection::clear()
{
	source.id=0;
	target.id=0;
	type=Connection::Invalid;
	cost=0;
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


void drawConnections(SDL_Renderer* renderer, ppl7::grafix::Point coords, const std::list<Connection>& connection_list)
{
	std::list<Connection>::const_iterator it;
	for (it=connection_list.begin();it != connection_list.end();++it) {
		const Position& p1=(*it).source;
		const Position& p2=(*it).target;
		switch ((*it).type) {
		case Connection::Walk:
			SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
			break;
		case Connection::JumpUp:
			SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
			break;
		case Connection::JumpLeft:
			SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
			break;
		case Connection::JumpRight:
			SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
			break;
		case Connection::Climb:
			SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
			break;

		default:
			SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
			break;

		}


		SDL_RenderDrawLine(renderer, coords.x + p1.x * TILE_WIDTH,
			coords.y + p1.y * TILE_HEIGHT,
			coords.x + p2.x * TILE_WIDTH + TILE_WIDTH - 1,
			coords.y + p2.y * TILE_HEIGHT + TILE_HEIGHT - 1);

		SDL_Rect rect;
		rect.x=coords.x + p1.x * TILE_WIDTH;
		rect.y=coords.y + p1.y * TILE_HEIGHT;
		rect.w=10;
		rect.h=10;
		SDL_RenderFillRect(renderer, &rect);
		rect.x=coords.x + p2.x * TILE_WIDTH + TILE_WIDTH - 1;
		rect.y=coords.y + p2.y * TILE_HEIGHT + TILE_HEIGHT - 1;
		rect.w=4;
		rect.h=4;
		SDL_RenderFillRect(renderer, &rect);
	}
}

void Waynet::draw(SDL_Renderer* renderer, const ppl7::grafix::Rect& viewport, const ppl7::grafix::Point& worldcoords) const
{
	std::map<uint32_t, WayPoint>::const_iterator it;
	int width=viewport.width();
	int height=viewport.height();
	ppl7::grafix::Point coords(viewport.x1 - worldcoords.x, viewport.y1 - worldcoords.y);
	std::list<Connection> connection_list;
	SDL_Rect rect;
	rect.w=TILE_WIDTH;
	rect.h=TILE_HEIGHT;
	for (it=waypoints.begin();it != waypoints.end();++it) {
		const WayPoint& wp=it->second;
		int x=wp.x * TILE_WIDTH - worldcoords.x;
		int y=wp.y * TILE_HEIGHT - worldcoords.y;
		if (x > 0 && y > 0 && x < width && y < height) {
			std::map<uint32_t, Connection>::const_iterator cit;
			for (cit=wp.connection_map.begin();cit != wp.connection_map.end();++cit) {
				connection_list.push_back(cit->second);
			}
			rect.x=coords.x + wp.x * TILE_WIDTH;
			rect.y=coords.y + wp.y * TILE_HEIGHT;
			if (wp.id == selection) {
				SDL_SetRenderDrawColor(renderer, 128, 255, 0, 255);	// green
			} else {
				SDL_SetRenderDrawColor(renderer, 0, 160, 0, 255);	// green
			}
			SDL_RenderFillRect(renderer, &rect);
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);	// black
			SDL_RenderDrawRect(renderer, &rect);
		}
	}
	drawConnections(renderer, coords, connection_list);
}

void Waynet::save(ppl7::FileObject& file, unsigned char id) const
{
	if (waypoints.size() == 0) return;
	std::map<uint32_t, WayPoint>::const_iterator it;
	size_t buffersize=6;
	for (it=waypoints.begin();it != waypoints.end();++it) {
		buffersize+=5 + it->second.connection_map.size() * 6;
	}
	unsigned char* buffer=(unsigned char*)malloc(buffersize);
	ppl7::Poke32(buffer + 0, 0);
	ppl7::Poke8(buffer + 4, id);
	ppl7::Poke8(buffer + 5, 1);		// Version
	size_t p=6;
	for (it=waypoints.begin();it != waypoints.end();++it) {
		const WayPoint& wp=it->second;
		ppl7::Poke32(buffer + p, it->first);
		ppl7::Poke8(buffer + p + 4, wp.connection_map.size());
		p+=5;
		std::map<uint32_t, Connection>::const_iterator cit;
		for (cit=wp.connection_map.begin();cit != wp.connection_map.end();++cit) {
			const Connection& conn=cit->second;
			ppl7::Poke32(buffer + p, conn.target.id);
			ppl7::Poke8(buffer + p + 4, conn.type);
			ppl7::Poke8(buffer + p + 5, conn.cost);
			p+=6;
		}
	}
	ppl7::Poke32(buffer + 0, p);
	file.write(buffer, p);
	free(buffer);
}

void Waynet::load(const ppl7::ByteArrayPtr& ba)
{
	clear();
	const char* buffer=ba.toCharPtr();
	int version=ppl7::Peek8(buffer);
	size_t p=1;
	if (version == 1) {
		while (p < ba.size()) {
			WayPoint wp(ppl7::Peek32(buffer + p));
			int count=ppl7::Peek8(buffer + p + 4);
			p+=5;
			for (int i=0;i < count;i++) {
				wp.addConnection(Connection(wp.id, ppl7::Peek32(buffer + p),
					(Connection::ConnectionType)ppl7::Peek8(buffer + p + 4),
					ppl7::Peek8(buffer + p + 5)));
				p+=6;
			}
			addPoint(wp);
		}
	} else {
		printf("Can't load Waynet, unknown version! [%d]\n", version);
	}
}

void Waynet::addPoint(const WayPoint& p1)
{
	waypoints.insert(std::pair<uint32_t, WayPoint>(p1.id, p1));
}

bool Waynet::hasPoint(const WayPoint& p1) const
{
	std::map<uint32_t, WayPoint>::const_iterator it;
	it=waypoints.find(p1.id);
	if (it != waypoints.end()) return true;
	return false;
}

void Waynet::deletePoint(const WayPoint& p1)
{
	// first: erase any connection targeting the point to delete
	std::map<uint32_t, WayPoint>::iterator it;
	for (it=waypoints.begin();it != waypoints.end();++it) {
		WayPoint& wp=it->second;
		wp.connection_map.erase(p1.id);
	}
	// second: delete the point itself
	waypoints.erase(p1.id);
	if (selection == p1.id) selection=0;
}

void Waynet::addConnection(const WayPoint& source, const Connection& conn)
{
	std::map<uint32_t, WayPoint>::iterator it;
	it=waypoints.find(source.id);
	if (it != waypoints.end()) {
		it->second.addConnection(conn);
	}
}

void Waynet::deleteConnection(const WayPoint& source, const WayPoint& target)
{
	std::map<uint32_t, WayPoint>::iterator it;
	it=waypoints.find(source.id);
	if (it != waypoints.end()) {
		it->second.deleteConnection(target.id);
	}
}

void Waynet::clearSelection()
{
	selection=0;
}

bool Waynet::hasSelection() const
{
	if (selection != 0) return true;
	return false;
}
WayPoint Waynet::getSelection() const
{
	return WayPoint(selection);
}

void Waynet::setSelection(const WayPoint& source)
{
	selection=source.id;
}

bool Waynet::hasConnection(const WayPoint& source, const WayPoint& target) const
{
	std::map<uint32_t, WayPoint>::const_iterator it;
	it=waypoints.find(source.id);
	if (it == waypoints.end()) return false;
	std::map<uint32_t, Connection>::const_iterator cit;
	cit=it->second.connection_map.find(target.id);
	if (cit == it->second.connection_map.end()) return false;
	return true;
}

double Distance(const Position& p1, const Position& p2)
{
	double a=abs(p2.x - p1.x);
	double b=abs(p2.y - p1.y);
	return sqrt((a * a) + (b * b));
}

const WayPoint& Waynet::findNearestWaypoint(const Position& p)
{
	std::map<uint32_t, WayPoint>::const_iterator it;
	const WayPoint* best=NULL;
	double best_distance=999999.0f;
	for (it=waypoints.begin();it != waypoints.end();++it) {
		double d=Distance(p, it->second);
		if (d < best_distance) {
			best_distance=d;
			best=&it->second;
		}
	}
	if (best) return *best;
	return invalid_waypoint;
}

static int calcCosts(const std::list<Connection>& conns)
{
	std::list<Connection>::const_iterator it;
	int c=0;
	for (it=conns.begin();it != conns.end();++it) {
		int d=(int)Distance((*it).source, (*it).target);
		c+=d;
	}
	return c;
}

bool Waynet::findBestWay(std::list<Connection>& way_list, const WayPoint& previous, const WayPoint& start, const WayPoint& target, int maxNodes)
{
	//printf ("Waynet::findBestWay %d, we have %d choices\n", maxNodes, (int)start.connection_map.size());
	if (maxNodes <= 0) return false;
	std::list<Connection>best;
	int best_cost=9999999;
	std::map<uint32_t, Connection>::const_iterator it;
	for (it=start.connection_map.begin();it != start.connection_map.end();++it) {
		if (it->second.target.id == previous.id) continue;
		std::list<Connection>current;
		current.push_back(it->second);
		if (it->second.target.id == target.id) {
			//printf ("found target %d\n", maxNodes);
			int cost=calcCosts(current);
			if (cost < best_cost) {
				best_cost=cost;
				best=current;
			}
		} else {
			if (findBestWay(current, start, waypoints[it->second.target.id], target, maxNodes - 1)) {
				int cost=calcCosts(current);
				if (cost < best_cost) {
					best_cost=cost;
					best=current;
				}
			}
		}
	}
	if (!best.size()) return false;
	std::list<Connection>::const_iterator i2;
	for (i2=best.begin();i2 != best.end();++i2) {
		way_list.push_back((*i2));
	}
	return true;
}

bool Waynet::findWay(std::list<Connection>& way_list, const Position& source, const Position& target)
{
	way_list.clear();
	if (waypoints.size() == 0) {
		printf("we don't have any waypoints\n");
		return false;
	}
	Position pt_source(source.x / TILE_WIDTH, source.y / TILE_HEIGHT);
	Position pt_target(target.x / TILE_WIDTH, target.y / TILE_HEIGHT);
	// find nearest waypoint of source
	const WayPoint& wp_source=findNearestWaypoint(pt_source);
	const WayPoint& wp_target=findNearestWaypoint(pt_target);
	if (wp_source == invalid_waypoint || wp_target == invalid_waypoint) return false;
	return findBestWay(way_list, WayPoint(), wp_source, wp_target, 15);

}
