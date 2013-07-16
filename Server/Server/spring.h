#pragma once
#include "Vector2f.h"
#include "RopePts.h"

class spring
{
private:
	RopePts* Pt1; //each spring have 2 points
	RopePts* Pt2;
	Vector2f total_force;
	float r_l;
	float strength;
	float dampening;
	float Fs;
	Vector2f Fd;
	void get_current_velocity();
public:
	void set_l(float input);
	void set_strength(float input);
	void set_damening(float input);
	void calculate_force(float dt);
	spring(float _length,float _dampening, float _strength, RopePts* r1, RopePts* r2);
	spring(void);
	~spring(void);
};

