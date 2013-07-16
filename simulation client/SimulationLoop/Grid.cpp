#include "Grid.h"
#include <Windows.h>
#include <gl\gl.h>  
#define _USE_MATH_DEFINES
#include <math.h>

Grid::Grid(void)
{
}


Grid::~Grid(void)
{
}

void Grid::setup(float _x, float _y, float _size)
{
	x = _x;
	y = _y;
	size = _size;
}

void Grid::testRender()
{
	glPushMatrix();	
	glBegin(GL_LINE_LOOP );
	glColor3d(0.2f, 0.2f, 0.2f);
	glVertex2d(x,y);
	glVertex2d(x+size,y);
	glVertex2d(x+size,y+size);
	glVertex2d(x,y+size);
	glVertex2d(x,y);
	glEnd();

}

//
//void Grid::setGrid(Grid *g, Grid_pos pos)
//{
//	switch(pos)
//	{
//	case N:
//		N_G = g;
//		break;
//	case NE:
//		NE_G = g;
//		break;
//	case E:
//		E_G = g;
//		break;
//	case SE:
//		SE_G = g;
//		break;
//	case S:
//		S_G = g;
//		break;
//	case SW:
//		SW_G = g;
//		break;
//	case W:
//		W_G = g;
//		break;
//	case NW:
//		NW_G = g;
//		break;
//	}
//}


//Grid* Grid::returnGrid(Grid_pos pos)
//{
//	switch(pos)
//	{
//	case N:
//		return N_G;
//	case NE:
//		return NE_G;
//	case E:
//		return E_G;
//	case SE:
//		return SE_G;
//	case S:
//		return S_G;
//	case SW:
//		return SW_G;
//	case W:
//		return W_G;
//	case NW:
//        return NW_G;
//	}
//}
//
//bool Grid::is_inGrid(Circle &c)
//{
//	return false;
//}
