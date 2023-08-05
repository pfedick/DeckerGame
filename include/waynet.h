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

class WayPoint;

class Connection
{
private:
	void updateTotalCosts();
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
	uint32_t source_as;
	Position target;
	uint32_t target_as;
	float cost;
	float total_costs;
	Connection();
	Connection(const WayPoint& source, const WayPoint& target, ConnectionType type, float cost=1.0f);
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
	bool operator==(const WayPoint& other) const;
};

double Distance(const Position& p1, const Position& p2);


class Waynet
{
public:
	typedef std::list<Connection> WayList;

private:
	std::map<uint32_t, WayPoint> waypoints;
	uint32_t next_as;

	uint32_t selection;
	WayPoint invalid_waypoint;
	SpriteTexture* spriteset;

	Position debug_start, debug_end;
	std::list<Connection> debug_waypoints;
	std::set<int> debug_as_part_of_way;
	bool debug_enabled;

	bool findBestWay(std::set<uint32_t>& visited_nodes, std::list<Connection>& way_list, const WayPoint& previous, const WayPoint& start, const WayPoint& target, int maxNodes) const;
	uint32_t getAs(const Position& wp);
	void drawConnections(SDL_Renderer* renderer, ppl7::grafix::Point coords, const std::list<Connection>& connection_list, bool debug=false) const;


	class WaySoFar
	{
	public:
		WayList waylist;
		float cost;
	};

	class BestWay
	{
	public:
		WayList waylist;
		float total_costs=0.0f;
		int depth=0;
	};

	class Way
	{
	public:
		const WayPoint& start;
		const WayPoint& end;
		std::list<Connection>& way_list;
		std::map<uint32_t, float> analyzed_nodes;
		BestWay best;
		int depth_limit;

		std::map<uint32_t, WaySoFar> check_next;

		Way(const WayPoint& start, const WayPoint& end, std::list<Connection>& way_list);

	};

	bool findBestWay(Way& way) const;

public:

	Waynet();
	~Waynet();
	void clear();
	void draw(SDL_Renderer* renderer, const ppl7::grafix::Rect& viewport, const ppl7::grafix::Point& worldcoords) const;
	void save(ppl7::FileObject& file, unsigned char id) const;
	void load(const ppl7::ByteArrayPtr& ba);
	void setSpriteset(SpriteTexture* spriteset);

	void addPoint(const WayPoint& p1);
	const WayPoint& findPoint(const WayPoint& p1) const;
	const WayPoint& invalidPoint() const;
	const WayPoint& getPoint(const Position& p1) const;
	//bool hasPoint(const WayPoint& p1) const;
	void deletePoint(const WayPoint& p1);
	void addConnection(const WayPoint& source, const Connection& conn);
	void deleteConnection(const WayPoint& source, const WayPoint& target);

	const WayPoint& findNearestWaypoint(const Position& p) const;
	bool findWay(std::list<Connection>& waypoints, const Position& source, const Position& target) const;

	void enableDebug(bool enable);
	void setDebugPoints(const Position& start, const Position& end);
	void setDebugStart(const Position& start);
	void setDebugEnd(const Position& end);


	void clearSelection();
	bool hasSelection() const;
	WayPoint getSelection() const;
	void setSelection(const WayPoint& source);
	bool hasConnection(const WayPoint& source, const WayPoint& target) const;
};

#endif	// INCLUDE_WAYNET_H_
