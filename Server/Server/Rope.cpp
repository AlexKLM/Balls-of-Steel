#include "Rope.h"


Rope::Rope(void)
{
	rPts = new concurrent_vector<RopePts>();
	springs = std::vector<spring>();
}


Rope::~Rope(void)
{
}

void Rope::debug_setup()
{
	float flex_s = 100;
	float flex_damp= 20;
	float s_s = 150;
	float s_damp = 30;

	rPts->push_back(RopePts(Vector2f(-1,0),1,0.05,true));

	rPts->push_back(RopePts(Vector2f(-1.8,0),1,0.05,false));

	rPts->push_back(RopePts(Vector2f(-1.6,0),1,0.05,false));

	rPts->push_back(RopePts(Vector2f(-1.4,0),1,0.05,false));

	rPts->push_back(RopePts(Vector2f(-1.2,0),1,0.05,false));

	rPts->push_back(RopePts(Vector2f(1,0),1,0.05,true));



	//rPts->push_back(RopePts(Vector2f(-0.8,0),0.05,0.05,false));

	//rPts->push_back(RopePts(Vector2f(0,0),0.05,0.05,false));

	//rPts->push_back(RopePts(Vector2f(0.5,0),0.05,0.05,true));

	spring s = spring(rPts->at(0).GetPos().distance(rPts->at(1).GetPos()),s_damp,s_s,&rPts->at(0),&rPts->at(1));
	springs.push_back(s);

	s = spring(rPts->at(1).GetPos().distance(rPts->at(2).GetPos()),s_damp,s_s,&rPts->at(1),&rPts->at(2));
	springs.push_back(s);
	
	s = spring(rPts->at(2).GetPos().distance(rPts->at(3).GetPos()),s_damp,s_s,&rPts->at(2),&rPts->at(3));
	springs.push_back(s);

	s = spring(rPts->at(3).GetPos().distance(rPts->at(4).GetPos()),s_damp,s_s,&rPts->at(3),&rPts->at(4));
	springs.push_back(s);

	s = spring(rPts->at(4).GetPos().distance(rPts->at(5).GetPos()),s_damp,s_s,&rPts->at(4),&rPts->at(5));
	springs.push_back(s);

	//flex springs
	s = spring(rPts->at(0).GetPos().distance(rPts->at(2).GetPos()),flex_damp,flex_s,&rPts->at(0),&rPts->at(2));
	springs.push_back(s);

	s = spring(rPts->at(1).GetPos().distance(rPts->at(3).GetPos()),flex_damp,flex_s,&rPts->at(1),&rPts->at(3));
	springs.push_back(s);

	s = spring(rPts->at(2).GetPos().distance(rPts->at(4).GetPos()),flex_damp,flex_s,&rPts->at(2),&rPts->at(4));
	springs.push_back(s);

	s = spring(rPts->at(3).GetPos().distance(rPts->at(5).GetPos()),flex_damp,flex_s,&rPts->at(3),&rPts->at(5));
	springs.push_back(s);

}

void Rope::setup(Vector2f EndPt1, Vector2f EndPt2, int num_points, float desired_size, float gap) //-1,3, to 1,3
{
	float flex_s = 2000;
	float flex_damp= 80;
	float s_s = 2000;
	float s_damp = 100;
	float mass = 5;

	//float gap = EndPt1.distance(EndPt2)/num_points;

	rPts->push_back(RopePts(EndPt1,mass,desired_size,true));
	Vector2f pos;
	for(int i = 1; i < num_points-1; i++)
	{
		 pos.Set(EndPt1.GetX() + (gap*i),EndPt1.GetY());
		rPts->push_back(RopePts(pos,mass,desired_size,false));
	}
	rPts->push_back(RopePts(EndPt2,mass,desired_size,true));
	//setup for normal springs
	spring s;
	for(int i = 0; i < num_points ; i++)
	{
		if(i +1 < num_points)
		{
			s = spring(rPts->at(i).GetPos().distance(rPts->at(i+1).GetPos()),s_damp,s_s,&rPts->at(i),&rPts->at(i+1));
			springs.push_back(s);
		}
		
	}
	////setup for flex springs
	for(int i = 0; i < num_points-2; i++)
	{
		s = spring(rPts->at(i).GetPos().distance(rPts->at(i+2).GetPos()),flex_damp,flex_s,&rPts->at(i),&rPts->at(i+2));
		springs.push_back(s);
	}



}

void Rope::update(float dt)
{
	for(int i = 0; i < rPts->size(); i++)
	{
		rPts->at(i).update(dt);
	}
}

void Rope::calculate_physics(float dt)
{
	for(int i = 0; i < springs.size(); i++)
	{
		springs[i].calculate_force(dt);
	}
	for(int i = 0; i < rPts->size();i++)
	{
		rPts->at(i).calculate_velocity(dt);
	}
}

concurrent_vector<RopePts>* Rope::get_RopePts()
{
	return rPts;
}

void Rope::reset()
{
	for(int i = 0; i < rPts->size();i++)
	{
		rPts->at(i).reset();
	}
}
