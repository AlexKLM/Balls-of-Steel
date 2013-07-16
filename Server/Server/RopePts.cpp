#include "RopePts.h"


RopePts::RopePts(void)
{
}


RopePts::~RopePts(void)
{
}


RopePts::RopePts(Vector2f _pos, float m,float radius,bool isfixed )
{
	Vel.Set(0,0);
	NewVel.Set(0,0);	
	ExternForce.Set(0,0);
	Pos = _pos;
	oriPos = Pos;
	NewPos = _pos;
	mass = m;
	cir = Circle(Pos,radius,mass);
	cir.SetPos(Pos.GetX(),Pos.GetY());
	cir.SetVel(0,0);
	cir.set_fixed(isfixed);
}

void RopePts::update(float dt)
{
	if(!cir.is_fixed())
	{
		cir.Update(dt);
		Vel = cir.GetVel();
		NewVel = cir.GetNewVel();
		Pos = cir.GetPos();
	}
	else
	{
		Vel.Set(0,0);
		NewVel.Set(0,0);
	}
}

Circle* RopePts::get_circle()
{
	return &cir;
}

Vector2f RopePts::get_old_velocity()
{
	return Vel;
}

void RopePts::calculate_velocity(float dt)
{
	if(!cir.is_fixed())
	{
		Vector2f force(0.0f, -9.81f * mass);
		force = ExternForce+force;
		Vector2f drag(0.1f,0.1f); //Cf
		drag = (drag*Vel)*-1;
		Vector2f accel = (force / mass)+drag;


		NewVel = Vel + (accel * dt);
		cir.SetNewVel(NewVel);
		cir.move(dt);
		ExternForce.Set(0,0);
	}
	else
	{
		Vel.Set(0,0);
		NewVel.Set(0,0);
	}
}

float RopePts::Getradius()
{
	return cir.GetRadius();
}
Vector2f RopePts::GetPos()
{
	return Pos;
}

void RopePts::add_force(Vector2f input)
{
	ExternForce = ExternForce+input;
}

void RopePts::reset()
{
	Pos = oriPos;
	Vel.Set(0,0);
	NewVel.Set(0,0);	
	ExternForce.Set(0,0);
	cir.SetPos(Pos.GetX(),Pos.GetY());
	cir.SetVel(0,0);
	cir.SetNewVel(Vector2f(0,0));
	ExternForce.Set(0,0);
}
Vector2f RopePts::calculate_force(float dt,float l_difference)
{
	//Fs = strength*l_difference;
	return Vector2f(0,0);
}

