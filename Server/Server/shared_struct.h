#pragma once
#include "Vector2f.h"

struct AABB{
	Vector2f min;
	Vector2f max;
};

struct balls_packet{
	float mass;
	float x;
	float y;
	float angle;
	float stationary;
};

struct config{//more to be added
	float friction;
	float e_ball;
	float e_line;
};

struct rope_packet{
	float x;
	float y;
	float radius;
};

struct qt_packet{
	float minx;
	float miny;
	float maxx;
	float maxy;
};

struct view{
	float camx;
	float camy;
	float sizex;
	float sizey;
};

struct status{
	int p_per_s; //physics per seconds
	int dp_per_s;//desiered physics per seconds
	int net_per_s;//network per seconds
	int dnet_pers;//desired network per seconds
	int total_b;//amount of balls
	float last_air;//last airjet force
	float fric;
	float elas_l;
	float elas_b;
};


struct grid_type{
	float x;
	float y;
	float size;
};

struct line_packet{
	float x1;
	float y1;
	float x2;
	float y2;
};


