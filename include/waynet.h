#ifndef INCLUDE_WAYNET_H_
#define INCLUDE_WAYNET_H_
#include <SDL.h>
#include <ppl7.h>
#include <ppl7-grafix.h>
#include <list>
#include <map>


class Position
{
public:
	union {
		struct {
			uint16_t x, y;
		};
		uint32_t id;
	};
	Position();
	Position(const Position& other);
	Position(uint32_t id);
	Position(uint16_t x, uint16_t y);
	operator uint32_t() const;
	bool operator==(const Position& other) const;
	bool isNear(const Position& other) const;
};

class Connection
{
public:
	enum ConnectionType {
		Invalid=0,
		Walk=1,
		JumpUp,
		JumpLeft,
		JumpRight,
		Climb,
		Go,
	};
	ConnectionType type;
	Position source;
	Position target;
	uint8_t cost;
	Connection();
	Connection(const Position& source, const Position& target, ConnectionType type, uint8_t cost=1);
	void clear();
	const char* name() const;
};

class WayPoint : public Position
{
public:
	std::map<uint32_t, Connection> connection_map;
	uint32_t as;

	WayPoint(uint16_t x, uint16_t y);
	WayPoint(uint32_t id);
	WayPoint();
	void addConnection(const Connection& conn);
	void deleteConnection(uint32_t target);
};

double Distance(const Position& p1, const Position& p2);


class Waynet
{
private:
	std::map<uint32_t, WayPoint> waypoints;
	uint32_t next_as;

	uint32_t selection;
	WayPoint invalid_waypoint;

	bool findBestWay(std::set<uint32_t>& visited_nodes, std::list<Connection>& way_list, const WayPoint& previous, const WayPoint& start, const WayPoint& target, int maxNodes);

public:
	Waynet();
	~Waynet();
	void clear();
	void draw(SDL_Renderer* renderer, const ppl7::grafix::Rect& viewport, const ppl7::grafix::Point& worldcoords) const;
	void save(ppl7::FileObject& file, unsigned char id) const;
	void load(const ppl7::ByteArrayPtr& ba);

	void addPoint(const WayPoint& p1);
	bool hasPoint(const WayPoint& p1) const;
	void deletePoint(const WayPoint& p1);
	void addConnection(const WayPoint& source, const Connection& conn);
	void deleteConnection(const WayPoint& source, const WayPoint& target);

	const WayPoint& findNearestWaypoint(const Position& p);
	bool findWay(std::list<Connection>& waypoints, const Position& source, const Position& target);


	void clearSelection();
	bool hasSelection() const;
	WayPoint getSelection() const;
	void setSelection(const WayPoint& source);
	bool hasConnection(const WayPoint& source, const WayPoint& target) const;
};

#endif	// INCLUDE_WAYNET_H_
