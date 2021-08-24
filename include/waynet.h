#ifndef INCLUDE_WAYNET_H_
#define INCLUDE_WAYNET_H_
#include <SDL.h>
#include <ppl7.h>
#include <ppl7-grafix.h>
#include <list>
#include <map>


class Connection
{
public:
	enum ConnectionType {
		Walk=1,
		JumpUp,
		JumpLeft,
		JumpRight,
		ClimbUp,
		ClimbDown
	};
	ConnectionType type;
	uint32_t target;
	uint8_t cost;
	Connection(ConnectionType type, uint32_t target, uint8_t cost=1);
};

class WayPoint
{
public:
	union {
		struct {
			uint16_t x,y;
		};
		uint32_t id;
	};
	std::map<uint32_t,Connection> connection_map;

	WayPoint(uint16_t x, uint16_t y);
	WayPoint(uint32_t id);
	void addConnection(const Connection &conn);
	void deleteConnection(uint32_t target);
};

class Waynet
{
private:
	std::map<uint32_t,WayPoint> waypoints;

	uint32_t selection;

public:
	Waynet();
	~Waynet();
	void clear();
	void draw(SDL_Renderer *renderer, const ppl7::grafix::Rect &viewport, const ppl7::grafix::Point &worldcoords) const;
	void save(ppl7::FileObject &file, unsigned char id) const;
	void load(const ppl7::ByteArrayPtr &ba);

	void addPoint(const WayPoint &p1);
	bool hasPoint(const WayPoint &p1) const;
	void deletePoint(const WayPoint &p1);
	void addConnection(const WayPoint &source, const Connection &conn);
	void deleteConnection(const WayPoint &source, const WayPoint &target);

	void findWay();


	void clearSelection();
	bool hasSelection() const;
	WayPoint getSelection() const;
	void setSelection(const WayPoint &source);
	bool hasConnection(const WayPoint &source, const WayPoint &target) const;
};

#endif	// INCLUDE_WAYNET_H_
