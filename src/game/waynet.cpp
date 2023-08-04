#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "decker.h"
#include "waynet.h"

//#define DEBUGWAYNET

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

bool Position::isNear(const Position& other) const
{
#ifdef DEBUGWAYNET
	ppl7::PrintDebug("Position::isNear, distance x=%d, distance y=%d\n", abs(x - other.x), abs(y - other.y));
#endif
	if (abs(x - other.x) <= 32 && abs(y - other.y) <= 32) return true;

	return false;
}

bool Position::operator==(const Position& other) const
{
	return (id == other.id);
}

WayPoint::WayPoint(uint16_t x, uint16_t y)
{
	this->x=x;
	this->y=y;
	as=0;
}

WayPoint::WayPoint(uint32_t id)
{
	this->id=id;
	as=0;
}

WayPoint::WayPoint()
{
	this->id=0;
	as=0;
}

bool WayPoint::operator==(const WayPoint& other) const
{
	return (id == other.id);
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
	source_as=0;
	target_as=0;
	type=Connection::Invalid;
	cost=1.0f;
	total_costs=0.0f;
}

Connection::Connection(const WayPoint& source, const WayPoint& target, ConnectionType type, float cost)
{
	this->source=source;
	this->target=target;
	source_as=source.as;
	target_as=target.as;
	this->type=type;
	this->cost=cost;
	updateTotalCosts();
}

void Connection::updateTotalCosts()
{
	total_costs=Distance(source, target) * cost;
	if (type == ConnectionType::Go) total_costs*=2.0f;
	if (type == ConnectionType::Climb) total_costs*=1.5;

}

void Connection::clear()
{
	source.id=0;
	target.id=0;
	type=Connection::Invalid;
	cost=1.0f;
	total_costs=0.0f;
}

const char* Connection::name() const
{
	switch (type) {
	case ConnectionType::Invalid: return "invalid";
	case ConnectionType::Walk: return "Walk";
	case ConnectionType::JumpUp: return "JumpUp";
	case ConnectionType::JumpLeft: return "JumpLeft";
	case ConnectionType::JumpRight: return "JumpRight";
	case ConnectionType::Climb: return "Climb";
	case ConnectionType::Go: return "Go";
	}
	return "unknown";
}

Waynet::Waynet()
{
	selection=0;
	next_as=1;
	spriteset=NULL;
	invalid_waypoint.id=0xffffffff;
	debug_enabled=false;
}

Waynet::~Waynet()
{
	clear();
}

void Waynet::clear()
{
	waypoints.clear();
	selection=0;
	next_as=1;
}

void Waynet::setSpriteset(SpriteTexture* spriteset)
{
	this->spriteset=spriteset;
}

void Waynet::drawConnections(SDL_Renderer* renderer, ppl7::grafix::Point coords, const std::list<Connection>& connection_list, bool debug) const
{
	std::list<Connection>::const_iterator it;
	for (it=connection_list.begin();it != connection_list.end();++it) {
		const Position& p1=(*it).source;
		const Position& p2=(*it).target;

		int x1, y1, x2, y2;
		if (p2.x < p1.x || p2.y < p1.y) {
			x1=coords.x + p1.x - 22;
			y1=coords.y + p1.y - 22;
		} else {
			x1=coords.x + p1.x + 22;
			y1=coords.y + p1.y + 22;
		}
		if (p1.x > p2.x || p1.y < p2.y) {
			x2=coords.x + p2.x + 22;
			y2=coords.y + p2.y - 22;
		} else {
			x2=coords.x + p2.x - 22;
			y2=coords.y + p2.y + 22;

		}
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		int w=1;
		if (debug) {
			if (debug_as_part_of_way.find((*it).source_as) != debug_as_part_of_way.end() &&
				debug_as_part_of_way.find((*it).target_as) != debug_as_part_of_way.end()) {
				w=4;
			}
		}


		SDL_RenderDrawLine(renderer, x1 + w, y1 + w, x2 + w, y2 + w);
		SDL_RenderDrawLine(renderer, x1 - 1, y1 - 1, x2 - 1, y2 - 1);

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
		for (int i=0;i < w;i++) {
			SDL_RenderDrawLine(renderer, x1 + i, y1 + i, x2 + i, y2 + i);
		}

	}
}

void drawAs(SDL_Renderer* renderer, SpriteTexture* spriteset, int x, int y, int as)
{
	ppl7::String s;
	s.setf("%d", as);
	int w=(int)s.size() * 10;
	x-=w / 2;
	for (size_t p=0;p < s.size();p++) {
		int num=s[p] - 48 + 3;
		spriteset->draw(renderer, x, y, num);
		x+=10;
	}


}

void Waynet::draw(SDL_Renderer* renderer, const ppl7::grafix::Rect& viewport, const ppl7::grafix::Point& worldcoords) const
{
	std::map<uint32_t, WayPoint>::const_iterator it;
	int width=viewport.width();
	int height=viewport.height();
	ppl7::grafix::Point coords(viewport.x1 - worldcoords.x, viewport.y1 - worldcoords.y);
	std::list<Connection> connection_list;
	/*
	SDL_Rect rect;
	rect.w=TILE_WIDTH;
	rect.h=TILE_HEIGHT;
	*/
	for (it=waypoints.begin();it != waypoints.end();++it) {
		const WayPoint& wp=it->second;
		int x=wp.x - worldcoords.x;
		int y=wp.y - worldcoords.y;
		if (x > -1000 && y > -1000 && x < width + 1000 && y < height + 1000) {
			std::map<uint32_t, Connection>::const_iterator cit;
			for (cit=wp.connection_map.begin();cit != wp.connection_map.end();++cit) {
				connection_list.push_back(cit->second);
			}
		}
	}
	drawConnections(renderer, coords, connection_list);
	if (debug_enabled) {
		drawConnections(renderer, coords, debug_waypoints, true);
	}
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);	// white
	if (spriteset) {
		for (it=waypoints.begin();it != waypoints.end();++it) {
			const WayPoint& wp=it->second;
			int x=wp.x - worldcoords.x;
			int y=wp.y - worldcoords.y;
			if (x > -48 && y > -48 && x < width + 24 && y < height + 24) {
				int sprite_no=0;
				if (debug_enabled) {
					if (debug_as_part_of_way.find(wp.as) != debug_as_part_of_way.end()) sprite_no=2;

				} else if (wp.id == selection) sprite_no=1;
				x=coords.x + wp.x;
				y=coords.y + wp.y;
				spriteset->draw(renderer, x, y, sprite_no);
				drawAs(renderer, spriteset, x, y, wp.as);
			}
		}
	}
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
	ppl7::Poke8(buffer + 5, 2);		// Version
	size_t p=6;
	for (it=waypoints.begin();it != waypoints.end();++it) {
		const WayPoint& wp=it->second;
		ppl7::Poke32(buffer + p, it->first);
		ppl7::Poke32(buffer + p + 4, wp.as);
		ppl7::Poke8(buffer + p + 8, wp.connection_map.size());
		p+=9;
		std::map<uint32_t, Connection>::const_iterator cit;
		for (cit=wp.connection_map.begin();cit != wp.connection_map.end();++cit) {
			const Connection& conn=cit->second;
			ppl7::Poke32(buffer + p, conn.target.id);
			ppl7::Poke8(buffer + p + 4, conn.type);
			ppl7::PokeFloat(buffer + p + 5, conn.cost);
			p+=9;
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
			wp.x=wp.x * TILE_WIDTH + 16;
			wp.y=wp.y * TILE_HEIGHT + 19;
			int count=ppl7::Peek8(buffer + p + 4);
			p+=5;
			for (int i=0;i < count;i++) {
				WayPoint destination(ppl7::Peek32(buffer + p));
				destination.x=destination.x * TILE_WIDTH + 16;
				destination.y=destination.y * TILE_HEIGHT + 19;
				wp.addConnection(Connection(wp.id, destination.id,
					(Connection::ConnectionType)ppl7::Peek8(buffer + p + 4),
					1.0f));
				p+=6;
			}
			addPoint(wp);
		}
	} else if (version == 2) {
		while (p < ba.size()) {
			WayPoint wp(ppl7::Peek32(buffer + p));
			wp.as=ppl7::Peek32(buffer + p + 4);
			if (wp.as >= next_as) next_as=wp.as + 1;
			int count=ppl7::Peek8(buffer + p + 8);
			p+=9;
			for (int i=0;i < count;i++) {
				wp.addConnection(Connection(wp.id, ppl7::Peek32(buffer + p),
					(Connection::ConnectionType)ppl7::Peek8(buffer + p + 4),
					ppl7::PeekFloat(buffer + p + 5)));
				p+=9;
			}
			addPoint(wp);
		}
	} else {
		printf("Can't load Waynet, unknown version! [%d]\n", version);
	}
	{
		std::map<uint32_t, WayPoint>::iterator itw;
		for (itw=waypoints.begin();itw != waypoints.end();++itw) {
			std::map<uint32_t, Connection>::iterator itc;
			for (itc=itw->second.connection_map.begin();itc != itw->second.connection_map.end();++itc) {
				itc->second.source_as=getAs(itc->second.source);
				itc->second.target_as=getAs(itc->second.target);
			}
		}
	}
}

void Waynet::addPoint(const WayPoint& p1)
{
	std::pair<std::map<uint32_t, WayPoint>::iterator, bool>result=waypoints.insert(std::pair<uint32_t, WayPoint>(p1.id, p1));
	if (result.first->second.as == 0) {
		result.first->second.as=next_as;
		next_as++;
	} else if (result.first->second.as >= next_as) next_as=result.first->second.as + 1;
}

const WayPoint& Waynet::findPoint(const WayPoint& p1) const
{
	std::map<uint32_t, WayPoint>::const_iterator it;
	for (it=waypoints.begin();it != waypoints.end();++it) {
		const WayPoint& wp=it->second;
		if (wp.isNear(p1)) return wp;
	}
	return invalid_waypoint;
}

const WayPoint& Waynet::getPoint(const Position& p1) const
{
	std::map<uint32_t, WayPoint>::const_iterator it;
	it=waypoints.find(p1);
	if (it != waypoints.end()) return (*it).second;
	return invalid_waypoint;
}

uint32_t Waynet::getAs(const Position& wp)
{
	std::map<uint32_t, WayPoint>::const_iterator it;
	it=waypoints.find(wp);
	if (it != waypoints.end()) return (*it).second.as;
	return 0;
}



const WayPoint& Waynet::invalidPoint() const
{
	return invalid_waypoint;
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
#ifdef DEBUGWAYNET
	ppl7::PrintDebugTime("Waynet::findNearestWaypoint (%d, %d) => invalid position", p.x, p.y);
#endif
	return invalid_waypoint;
}

static int calcCosts(const std::list<Connection>& conns)
{
	std::list<Connection>::const_iterator it;
	int c=0;
	for (it=conns.begin();it != conns.end();++it) {
		int d=(int)(Distance((*it).source, (*it).target) * (*it).cost);
		c+=d;
	}
	return c;
}

void Waynet::enableDebug(bool enable)
{
	debug_enabled=enable;
	debug_start=invalid_waypoint;
	debug_end=invalid_waypoint;
	debug_as_part_of_way.clear();
	debug_waypoints.clear();
}

void Waynet::setDebugPoints(const Position& start, const Position& end)
{
	debug_as_part_of_way.clear();
	debug_waypoints.clear();
	debug_start=start;
	debug_end=end;
	if (debug_start == invalidPoint() || debug_end == invalidPoint()) return;
	ppl7::PrintDebugTime("===========================================================================\n");
	ppl7::PrintDebugTime("searching for a way from %d:%d => %d:%d\n", debug_start.x, debug_start.y,
		debug_end.x, debug_end.y);

	if (!findWay(debug_waypoints, debug_start, debug_end)) {
		ppl7::PrintDebugTime("Found no way :-(\n");
	} else {
		std::list<Connection>::const_iterator it;
		float total=0.0f;
		for (it=debug_waypoints.begin();it != debug_waypoints.end();++it) {
			total+=(*it).total_costs;
			ppl7::PrintDebugTime("source: %3d(%5d:%5d), target: %3d(%5d:%5d), type: %-10s, cost: %0.3f\n",
				(*it).source_as,
				(*it).source.x, (*it).source.y,
				(*it).target_as,
				(*it).target.x, (*it).target.y,
				(*it).name(), (*it).total_costs);
			debug_as_part_of_way.insert((*it).source_as);
			debug_as_part_of_way.insert((*it).target_as);
		}
		ppl7::PrintDebugTime("Total Hops: %zd, Total costs: %0.3f\n", debug_waypoints.size(), total);
	}
}

void Waynet::setDebugStart(const Position& start)
{
	debug_start=start;
	setDebugPoints(debug_start, debug_end);
}

void Waynet::setDebugEnd(const Position& end)
{
	debug_end=end;
	setDebugPoints(debug_start, debug_end);
}


bool Waynet::findBestWay(std::set<uint32_t>& visited_nodes, std::list<Connection>& way_list, const WayPoint& previous, const WayPoint& start, const WayPoint& target, int maxNodes)
{
	if (visited_nodes.find(start.id) != visited_nodes.end()) return false;
#ifdef DEBUGWAYNET
	ppl7::PrintDebugTime("Waynet::findBestWay, Node=%d, depth: %d, maxnodes: %d, we have %d choices, waylist is %zd nodes long\n",
		start.as, visited_nodes.size(),
		maxNodes, (int)start.connection_map.size(), way_list.size());
#endif
	if (maxNodes <= 0) return false;
	std::list<Connection>best;
	int best_cost=9999999;
	visited_nodes.insert(start.id);

	std::map<uint32_t, Connection>::const_iterator it;
	for (it=start.connection_map.begin();it != start.connection_map.end();++it) {
		if (it->second.target.id == previous.id) continue;
		std::list<Connection>current;
		current.push_back(it->second);
		if (it->second.target.id == target.id) {
#ifdef DEBUGWAYNET
			//ppl7::PrintDebugTime("found target %d\n", maxNodes);
#endif
			int cost=calcCosts(current);
			if (cost < best_cost) {
				best_cost=cost;
				best=current;
			}
		} else {
			if (findBestWay(visited_nodes, current, start, waypoints[it->second.target.id], target, maxNodes - 1)) {
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
	Position pt_source(source.x, source.y);
	Position pt_target(target.x, target.y);
	// find nearest waypoint of source
	const WayPoint& wp_source=findNearestWaypoint(pt_source);
	const WayPoint& wp_target=findNearestWaypoint(pt_target);
	if (wp_source == invalid_waypoint || wp_target == invalid_waypoint) return false;
	std::set<uint32_t> visited_nodes;
#ifdef DEBUGWAYNET
	ppl7::PrintDebugTime("\n\n\nstart to find a way form %d:%d (%u) to to %d:%d (%u) =========================================================\n",
		wp_source.x, wp_source.y, wp_source.id,
		wp_target.x, wp_target.y, wp_target.id);
#endif
	for (int maxchecks=10;maxchecks < 25;maxchecks+=5) {
		if (findBestWay(visited_nodes, way_list, WayPoint(), wp_source, wp_target, maxchecks)) return true;
	}
	return false;

}
