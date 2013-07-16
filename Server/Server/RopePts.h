#pragma once
#include "Vector2f.h"
#include "Circle.h"
class RopePts
{
private:
	Vector2f Pos;
	Vector2f Vel;
	Vector2f NewVel;
	Vector2f NewPos;
	Vector2f ExternForce;
	Vector2f oriPos;
	float mass;
	Circle cir;
public:
	void set_ex_force(Vector2f ExternForce);
	void calculate_force(float dt);
	float Getradius();
	Circle* get_circle();
	Vector2f get_old_velocity();
	void add_force(Vector2f input);
	Vector2f calculate_force(float dt,float l_difference);
	void calculate_velocity(float dt);
	void calculate_force(Vector2f V1,float dt);
	void reset();
	Vector2f GetPos();
	void update(float dt);
	RopePts(void);
	RopePts(Vector2f _pos, float m,float radius,bool isfixed );
	~RopePts(void);
};

