#pragma once
#include "Vector2f.h"
#include <vector>
#include "line.h"
#include "Grid.h"
using namespace std;
//THE GRID
class Grid
{
public:
	Grid(void);
	~Grid(void);
	void setup(float _x, float _y, float _size);
	/*bool is_inGrid(Circle &c);
	bool is_inGrid(line &l);*/
	void testRender();
	/*void setGrid(Grid* g, Grid_pos pos);*/
	/*Grid* returnGrid(Grid_pos pos);*/

private:
	vector<Circle*> circles;
	vector<line*> lines;
	//Vector2f pos; //not nessary?
	float x,y;
	float size;

	////nearby grid
	//Grid* N_G;
	//Grid* NE_G;
	//Grid* E_G;
	//Grid* SE_G;
	//Grid* S_G;
	//Grid* SW_G;
	//Grid* W_G;
	//Grid* NW_G;
};

