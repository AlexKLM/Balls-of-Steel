#include "ContactManifold.h"
#include <Windows.h>

#include <gl\gl.h> 

#define _USE_MATH_DEFINES
#include <math.h>

#include "Circle.h"

#include <assert.h>

ContactManifold::ContactManifold(void) : m_numOfPoints(0), m_numOfDebugPoints(0), m_numOfPoints_line(0)
{
}


ContactManifold::~ContactManifold(void)
{
}

void ContactManifold::Add(ManifoldPoint point)
{
	
	point.responded=false;
	m_points[m_numOfPoints] = point;
	++m_numOfPoints;
}

void ContactManifold::reset()
{
	m_numOfPoints = 0;
	m_numOfPoints_line = 0;
}
void ContactManifold::Add(line_ManifoldPoint point)
{
	m_points_line[m_numOfPoints_line] = point;
	++m_numOfPoints_line;
}

void ContactManifold::Assess()
{
	bool loop = true;
	while(loop)
	{
		loop = false;
		for(int collision = 0; collision < m_numOfPoints; ++collision)
		{
			//Circle *circle1 = m_points[collision].contactID1;
			//Circle *circle2 = m_points[collision].contactID2;
			if(m_points[collision].responded)
			{
				for(int n = collision; n < m_numOfPoints-1; ++n)
				{
					m_points[n] = m_points[n+1];
				}
				--m_numOfPoints;
				loop = true;
			}
			/*if(circle1->GetPos().distance(circle2->GetPos()) > circle1->GetRadius() + circle2->GetRadius())
			{
				
			}*/
		}
	}
}

bool ContactManifold::all_sorted()
{
	bool sorted = true;
	for(int collision = 0; collision < m_numOfPoints_line; ++collision)
	{
		if(m_points_line[collision].remaining_time > 0)
		{
			return false;
		}
		else
		{
			for(int n = collision; n < m_numOfPoints_line-1; ++n)
			{
				m_points_line[n] = m_points_line[n+1];
			}
			--m_numOfPoints_line;
		}
	}
	return sorted;
}

void ContactManifold::reaccess()
{
	for(int collision = 0; collision < m_numOfPoints_line; ++collision)
	{

	}
}

void ContactManifold::Add_debug(Vector2f point)
{
	//m_debug_points[m_numOfDebugPoints] = point;
	//++m_numOfDebugPoints;
}

//void ContactManifold::debug_render()
//{
//	for(int i = 0; i < m_numOfDebugPoints; i++)
//	{
//		glPushMatrix();
//			glTranslatef(m_debug_points[i].GetX(), m_debug_points[i].GetY(), 0);
//			glBegin(GL_LINE_LOOP);
//			glColor3d(0, 0, 1);
//			glVertex2d(0,0);
//			for(float angle = 0.0f; angle < 2*M_PI; angle += 0.1f)
//			{
//				glVertex2d(sin(angle)*0.1f,  cos(angle)*0.10);
//			}
//			
//			glVertex2d(sin(2*M_PI)*0.1f,  cos(2*M_PI)*0.1f);
//			glEnd();
//			glPopMatrix();
//
//	}
//}

int ContactManifold::GetNumPoints() const
{
	return m_numOfPoints;
}

int ContactManifold::GetNumLineContact() const
{
	return m_numOfPoints_line;
}

ManifoldPoint& ContactManifold::GetPoint(int index)
{
	return m_points[index];
}

line_ManifoldPoint& ContactManifold::Get_linePoint(int index)
{
	return m_points_line[index];
}