#pragma once
#include "ContactManifold.h"
#include "Vector2f.h"
//#include "QT_square.h"
#include "shared_struct.h"
class line
{
public:
	line(void);
	~line(void);
	
	void setup(float x1,float y1,float x2,float y2);
	//void Render();

	Vector2f GetP1();
	Vector2f GetP2();
	Vector2f GetNormal();
	bool is_in_line(float x, float y);
	AABB get_Bbox();
private:
	Vector2f P1;
	Vector2f P2;
	Vector2f Normal;
};

