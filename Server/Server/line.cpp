#include "line.h"
#include <Windows.h>
#include <gl\gl.h>  
#define _USE_MATH_DEFINES
#include <math.h>

line::line(void)
{
}


line::~line(void)
{
}

Vector2f line::GetP1()
{
	return P1;
}

Vector2f line::GetP2()
{
	return P2;
}

Vector2f line::GetNormal()
{
	return Normal;
}

const float EPSILON = 0.001f;
bool line::is_in_line(float x, float y)
{
	float rx = floorf(x * 100) / 100;
	float ry = floorf(y * 100) /100;
	if((rx <= P1.GetX() && rx >= P2.GetX()) || (rx >= P1.GetX() && rx <= P2.GetX()))
	{
		if((ry <= P1.GetY() && ry >= P2.GetY()) || (ry >= P1.GetY() && ry <= P2.GetY()))
		{
			return true;
		}
	}
	return false;

}

void line::setup(float x1,float y1,float x2,float y2)
{
	P1 = Vector2f(x1,y1);
	P2 = Vector2f(x2,y2);

	
	Vector2f v = P2 - P1;
	Normal = Vector2f(-v.GetY(),v.GetX());
	/*float length = N.length();
	Normal = Vector2f(N.GetX()/length,N.GetY()/length);*/

	Normal = Normal.normalise();
}

//void line::Render()
//{		
//
//	glBegin(GL_LINES );
//	glColor3d(1, 1, 1);
//	glVertex2d(P1.GetX(),P1.GetY());
//	glVertex2d(P2.GetX(),P2.GetY());
//	glEnd();
//
//	glBegin(GL_LINES);
//	glColor3d(0, 1, 0);
//	Vector2f v = (P1 + P2)/2;
//	Vector2f n = v+Normal;
//	glVertex2d(v.GetX(),v.GetY());
//	glVertex2d(n.GetX(),n.GetY());
//	glEnd();
//
//	glBegin(GL_LINE_LOOP);
//	glColor3d(0.5, 0.5, 0.5);
//	glVertex2d(min(P1.GetX(),P2.GetX())-0.05f,min(P1.GetY(),P2.GetY())-0.05f);
//	glVertex2d(min(P1.GetX(),P2.GetX())-0.05f,max(P1.GetY(),P2.GetY())+0.05f);
//	glVertex2d(max(P1.GetX(),P2.GetX())+0.05f,max(P1.GetY(),P2.GetY())+0.05f);
//	glVertex2d(max(P1.GetX(),P2.GetX())+0.05f,min(P1.GetY(),P2.GetY())-0.05f);
//	glEnd();
//
//}

AABB line::get_Bbox()
{
	AABB box;
	box.min = Vector2f(min(P1.GetX(),P2.GetX())-0.05f,min(P1.GetY(),P2.GetY())-0.05f);
	box.max = Vector2f(max(P1.GetX(),P2.GetX())+0.05f,max(P1.GetY(),P2.GetY())+0.05f);
	return box;
}