#pragma once
#include <vector>
#include "Vector2f.h"
using namespace std;

class Circle;
enum collision_type
{
	with_circle,
	with_line,
	with_line_end,
};

struct ManifoldPoint {
	collision_type type;
	Vector2f contactPoint;
	Circle *contactID1;
	Circle *contactID2;
	Vector2f contactNormal;
	float contact_time;
	float remaining_time;
	bool responded;
};

struct line_ManifoldPoint
{
	Circle *contactID1;
	Vector2f contactPoint;
	Vector2f lineNormal;
	float contact_time;
	float remaining_time;
	bool responded;
};

class ContactManifold
{
public:
	ContactManifold(void);
	~ContactManifold(void);
	bool all_sorted();
	void Add(ManifoldPoint point);
	void Add(line_ManifoldPoint point);
	void Add_debug(Vector2f point);
	void Assess();
	void reaccess();
	void debug_render();
	int GetNumPoints() const;
	int GetNumLineContact() const;
	ManifoldPoint& GetPoint(int index);
	line_ManifoldPoint& Get_linePoint(int index);

private:
	// ******************************************************************************************
	// The max number of points in the manifold is not verified in the code, so you need to test!
	// Maybe it would be safer to use a dyamic data sturcture (although this may be too slow)?
	// ******************************************************************************************
	ManifoldPoint m_points[1000];
	std::vector<line_ManifoldPoint> points_line;
	std::vector<Vector2f> debug_points;
	line_ManifoldPoint m_points_line[1000];//1000 for now
	Vector2f m_debug_points[1000]; //for debugging line collision
	int m_numOfPoints_line;
	int m_numOfPoints;
	int m_numOfDebugPoints;
};

