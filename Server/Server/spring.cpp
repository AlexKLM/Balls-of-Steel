#include "spring.h"


spring::spring(void)
{
}


spring::spring(float _length,float _dampening, float _strength, RopePts* r1, RopePts* r2)
{
	Pt1 = r1;
	Pt2 = r2;
	r_l = _length;
	strength = _strength;
	dampening = _dampening;
	total_force.Set(0,0);
	Fs = 0;
	Fd.Set(0,0);
}

spring::~spring(void)
{
}


void spring::set_damening(float input)
{
	dampening = input;
}

void spring::set_l(float input)
{
	r_l = input;
}

void spring::set_strength(float input)
{
	strength = input;
}

void spring::calculate_force(float dt)
{
	Vector2f L = Pt2->GetPos() -Pt1->GetPos();
	float length = Pt1->GetPos().distance(Pt2->GetPos());
	Fs = strength*(length - r_l);  //force produce by spring = strength * length difference

	Vector2f velocity_diff = Pt1->get_old_velocity() - Pt2->get_old_velocity();

	total_force = (Fs - (dampening*(velocity_diff).dot(L)/length))*L/length;
	Pt1->add_force(total_force);
	Pt2->add_force(total_force*-1);
}


