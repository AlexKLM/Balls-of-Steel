#include "Game.h"
#include <gl\gl.h>                                // Header File For The OpenGL32 Library

Game::Game(HDC hdc,concurrent_vector<balls_packet>*_ball) : m_hdc(hdc), m_previousTime(0)
{
	running = true;
	current_time = 0;
	accumlator = 0;
	dt = 0.0166;
	Circlelist = new std::vector<Circle>();
	ball = _ball;

	counter = 0;
	//game_grid[0].setup(-5,-5,2);
	add_circle();
	int ix = 0;
	int iy = 0;
	for(int x = -5; x < 5; x+=2)
	{
		for(int y = -5; y < 5;y+=2)
		{
			//game_grid[ix][iy] = new Grid();
			game_grid[ix][iy].setup(x,y,2);
			iy++;
		}
		iy = 0;
		ix++;
	}
	ix = 0;
	iy = 0;
	AABB QT_box;
	QT_box.min = Vector2f(-5,-5);
	QT_box.max = Vector2f(5,5);
	Q_tree = QT_square(0,QT_box);


	setup_world();

	for(int i = 0; i < 12; i++)
	{
		Q_tree.insert(lines[i]);
	}

	m_manifold = new ContactManifold();
}

void Game::add_circle()
{
	Circle c;
	c.SetPos(-3.5,2);
	c.SetVel(0,0);
	//c.add_force(0,0);
	c.SetMass(1.0f);
	Circlelist->push_back(c);

	//c.SetPos(0.5,2);
	//c.SetVel(2,0);
	//c.SetMass(1.0f);
	//Circlelist.push_back(c);
}

Game::~Game(void)
{
	//Circlelist.erase();
	/*delete m_Circle1;
	delete m_Circle2;
	delete m_Circle3;*/
	delete m_manifold;
}

void Game::setup_world()
{
	lines[0] = new line();

	lines[0]->setup(-5,5,5,5);

	lines[1] = new line();

	lines[1]->setup(-5,5,-5,-3);

	lines[2] = new line();
	lines[2]->setup(-5,-3,-3,-3);

	lines[3] = new line();
	lines[3]->setup(-3,-3,-1,-5);

	lines[4] = new line();
	lines[4]->setup(-1,-5,1,-5);

	lines[5] = new line();
	lines[5]->setup(1,-5,3,-3);

	lines[6] = new line();
	lines[6]->setup(3,-3,5,-3);

	lines[7] = new line();
	lines[7]->setup(5,-3,5,5);

	lines[8] = new line();
	lines[8]->setup(-5,-1,-3,-1);

	lines[9] = new line();
	lines[9]->setup(-3,-1,-3,1);

	lines[10] = new line();
	lines[10]->setup(1,1,3,-1);

	lines[11] = new line();
	lines[11]->setup(-1,3,1,3);
}

void Game::Update()
{
	// **************************************************
	// The simulation loop should be on its own thread(s)
	// **************************************************
	//SimulationLoop();
	//Render();
}

void Game::SimulationLoop(float dt)
{
	//Vector2f pos1 = Vector2f(2,0);
	//Vector2f pos2 = Vector2f(4,0);
	//float dist = pos1.distance(pos2);
	//Vector2f colNormal = (pos1 - pos2).normalise();
	
	//DEBUG STUFF DELETE THIS
	

	Q_tree.remove_objects();

	update_QT();



	// Handle static collisions
	StaticCollisionDetection();

	// Should actually calculate dt based on the simulation loop rate using a timer
	//float currentTime = m_previousTime + 0.01f;
	//m_dt = currentTime - m_previousTime;
	m_dt = dt;
	// Calculate the physic calculations on all objects (e.g. new position, velocity, etc)
	CalculateObjectPhysics();

	// Assess and remove any collisions that no longer take place
	m_manifold->Assess();

	// Handle dynamic collisions and update contact manifold for all objects
	DynamicCollisionDetection();

	//circle that have collided will add their ealiest contact point to manifold
	Update_manifold();

	// Handle dynamic collision responses using the contact manifold for all objects
	DynamicCollisionResponse();

	// Update the physics calculations on all objects (e.g. new position, velocity, etc)
	UpdateObjectPhysics();
	/*if(Circlelist.size() <1)
	{
		if(counter< 8*m_dt)
		{
			counter+= m_dt;
		}
		else
		{
			add_circle();
			counter =0;
		}
	}*/
	//m_previousTime = currentTime + m_dt;
}

//**************************Handle static collisions***********************
void Game::StaticCollisionDetection()
{
	//http://gamedev.tutsplus.com/tutorials/implementation/quick-tip-use-quadtrees-to-detect-likely-collisions-in-2d-space/
	bool loop = true;
	int max_loop = 10;
	int loop_no = 0;
	std::vector<Circle*> possible_Cs;
	std::vector<line*> possible_Ls;
	while(loop && loop_no < max_loop)
	{
		loop = false;
		for each(line *l in lines)
		{
			for(int i = 0; i < Circlelist->size();i++)
			{
				loop |=StaticSphereLineCollisionDetection(&Circlelist->at(i),l);
			}
		
		}

		for (int i = 0; i < Circlelist->size(); i++)
		{
		
			possible_Cs.clear();
			Q_tree.generate_possible_list(&possible_Cs,&Circlelist->at(i));
			
			for(int c = 0; c<possible_Cs.size(); c++)
			{
				if(possible_Cs[c]->getID() != Circlelist->at(i).getID())
				{
					loop |= StaticSphereCollisionDetection(&Circlelist->at(i), possible_Cs[c]);
				}
			}
		}
		/*loop |= StaticSphereCollisionDetection(m_Circle1, m_Circle2);
		loop |= StaticSphereCollisionDetection(m_Circle1, m_Circle3);
		loop |= StaticSphereCollisionDetection(m_Circle2, m_Circle3);*/
		Q_tree.remove_objects();
		update_QT();
		loop_no++;
	}
}

void Game::Update_manifold()
{
	for(int i = 0; i < Circlelist->size();i++)
	{
		if(Circlelist->at(i).have_collided())
		{
			Circlelist->at(i).add_to_manifold(m_manifold);
		}
	}
}

bool Game::StaticSphereCollisionDetection(Circle *circle1, Circle *circle2)
{
	bool loop = false;
	//if(circle1->is_stationary() && circle2->is_stationary())
	//{
	//	return false;
	//}
		if(circle1->is_stationary() && circle2->is_stationary())
	{
		return loop;
	}
	Vector2f pos1 = circle1->GetPos();
	Vector2f pos2 = circle2->GetPos();
	float dist = pos1.distance(pos2) - (circle1->GetRadius() + circle2->GetRadius());
	if(dist < 0.0f)
	{
		Vector2f colNormal = (pos1 - pos2).normalise();

		pos1 = pos1 - (colNormal * dist * 1.1f);
		circle1->SetPos(pos1.GetX(), pos1.GetY());
		circle1->set_stationary(false);
		pos2 = pos2 + (colNormal * dist* 1.1f);
		circle2->SetPos(pos2.GetX(), pos2.GetY());
		circle2->set_stationary(false);
		loop = true;
	}

	return loop;
}

bool Game::StaticSphereLineCollisionDetection(Circle *circle1, line *line1)
{
	bool loop = false;
	/*if(circle1->is_stationary())
	{
		return false;
	}*/
	if(circle1->is_stationary())
	{
		return loop;
	}
	Vector2f center = circle1->GetPos();
	float radius = circle1->GetRadius();
	Vector2f p1 = line1->GetP1(); //end point 1 of line
	Vector2f p2 = line1->GetP2();
	//using c2 = a2+b2 method to find length of circle pos to closest line point
	Vector2f dir = p2 - p1;
	Vector2f diff = center - p1;
	float t = diff.dot(dir)/dir.dot(dir);
	if(t<0.0f)
	{
		t = 0.0f;
	}
	else if(t > 1.0f)
	{
		t = 1.0f;
	}
	Vector2f closest_p = p1+t*dir;
	Vector2f dist = center - closest_p;
	float distsqr = dist.dot(dist);
	if(distsqr <= (radius*radius))
	{

		//m_manifold->Add_debug(closest_p);
		float dist = center.distance(closest_p) - (circle1->GetRadius());//1 is fake circle radius
		
		Vector2f colNormal = (center - closest_p).normalise();

		float resolve_dir = colNormal.dot(circle1->GetVel().normalise());
		if(resolve_dir >=-1.0f && resolve_dir <= 0.0f)
		{
			colNormal.mult(-1); //makes sure the resolve direction is always against the velocity direction(otherwise may push circle to otherside of line)
			
		}
		center = center -(colNormal * dist *1.1f);
		/*else
		{
			center = center + (colNormal * dist * 1.1f);
		}*/
		circle1->SetPos(center.GetX(), center.GetY());
		circle1->set_stationary(false);
		loop = true;
	}
	return loop;
}

void Game::add_circle(float x, float y)
{
	Circle c;
	c.SetPos(x,y);
	c.SetVel(0,0);
	//c.add_force(0,0);
	c.SetMass(1.0f);
	Circlelist->push_back(c);
}

//**************************Update the physics calculations on each object***********************
void Game::CalculateObjectPhysics()
{
	for(int i = 0; i < Circlelist->size();i++)
	{
		if(!Circlelist->at(i).is_stationary())
		{
			Circlelist->at(i).CalculatePhysicsImE(m_dt);
		}
	}
	/*m_Circle1->CalculatePhysicsImE(m_dt);
	m_Circle2->CalculatePhysicsImE(m_dt);
	m_Circle3->CalculatePhysicsImE(m_dt);*/
}

//**************************Handle dynamic collisions***********************
void Game::DynamicCollisionDetection()
{
	/*m_Circle1->CollisionWithCircle(m_Circle2, m_manifold,m_dt);
	m_Circle1->CollisionWithCircle(m_Circle3, m_manifold,m_dt);
	m_Circle2->CollisionWithCircle(m_Circle3, m_manifold,m_dt);

	for each(line *l in lines)
	{
		m_Circle1->CollisionWithLine(l,m_manifold,m_dt);
		m_Circle2->CollisionWithLine(l,m_manifold,m_dt);
		m_Circle3->CollisionWithLine(l,m_manifold,m_dt);
	}*/
	for (int i = 0; i < Circlelist->size(); i++)
	{
		for (int next = i+1;  next<Circlelist->size();next++)
		{
			Circlelist->at(i).CollisionWithCircle(&Circlelist->at(next),m_dt);
		}
			
	}

	for each(line *l in lines)
	{
		for(int i = 0; i<Circlelist->size();i++)
		{
			Circlelist->at(i).CollisionWithLine(l,m_dt);
		}
	}
}

//**************************Handle dynamic collision responses***********************
void Game::DynamicCollisionResponse()
{
	for(int collision = 0; collision < m_manifold->GetNumPoints(); ++collision)
	{
		ManifoldPoint &point = m_manifold->GetPoint(collision);
		if(!point.responded)
		{
			if(point.type == with_circle) 
			{
				point.contactID1->CollisionResponseWithCircle(point,m_dt);
			}
			else if(point.type == with_line)
			{
				point.contactID1->CollisionResponseWithLine(point,m_dt);
			}
			else if(point.type == with_line_end)
			{
				point.contactID1->CollisionResponseWithLineEnd(point,m_dt);
			}
		}
	}

	/*for(int collision = 0;collision < m_manifold->GetNumLineContact();++collision)
	{
		line_ManifoldPoint &point = m_manifold->Get_linePoint(collision);
		if(!point.responded) point.contactID1->CollisionResponseWithLine(point,m_dt);
	}*/
}

//void Game::continuous_collisiton(float dt)
//{
//	bool have_collision = true;
//	float remaining_time = dt;
//	while(dt >0)
//	{
//
//	}
//}




void Game::update_QT()
{
	for(int i = 0; i < Circlelist->size(); i++)
	{
		Q_tree.insert(&Circlelist->at(i));
	}
}

//**************************Update the physics calculations on each object***********************
void Game::UpdateObjectPhysics()
{
	for(int i = 0; i < Circlelist->size();i++)
	{
		Circlelist->at(i).Update();
	}
	/*m_Circle1->Update();
	m_Circle2->Update();
	m_Circle3->Update();*/
}

//**************************Render and display the scene in OpenGL***********************
void Game::Render()									// Here's Where We Do All The Drawing
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer
	glLoadIdentity();									// Reset The Current Modelview Matrix
	m_manifold->debug_render();
	
	for each(Grid g in game_grid)
	{
		g.testRender();
	}

	for each(line *l in lines)
	{
		l->Render();
	}

	for(int i = 0; i < Circlelist->size();i++)
	{
		Circlelist->at(i).Render();
	}
	Q_tree.Render();
	/*m_Circle1->Render();
	m_Circle2->Render();
	m_Circle3->Render();*/

	SwapBuffers(m_hdc);				// Swap Buffers (Double Buffering)
}

//*************************Run at every end of frame************************************
void Game::write_b_to_packet()
{
	//ball->clear();
	
	for(int i = 0; i < Circlelist->size();i++)
	{
		if(i >= ball->size())
		{
			balls_packet b;
			ball->push_back(b);
			ball->back().x = Circlelist->at(i).GetPos().GetX();
			ball->back().y = Circlelist->at(i).GetPos().GetY();
			ball->back().angle = Circlelist->at(i).GetAngle();
			ball->back().stationary = Circlelist->at(i).is_stationary();
		}
		else
		{
			ball->at(i).x = Circlelist->at(i).GetPos().GetX();
			ball->at(i).y = Circlelist->at(i).GetPos().GetY();
			ball->at(i).angle = Circlelist->at(i).GetAngle();
			ball->at(i).stationary = Circlelist->at(i).is_stationary();
		}

	}
}


float Game::get_time()
{
	static __int64 start = 0;
    static __int64 frequency = 0;

    if (start==0)
    {
        QueryPerformanceCounter((LARGE_INTEGER*)&start);
        QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);
        return 0.0f;
    }

    __int64 counter = 0;
    QueryPerformanceCounter((LARGE_INTEGER*)&counter);
    return (float) ((counter - start) / double(frequency));
}

float Game::calculate_dt()
{
	const float time = get_time();
	float dt = time - current_time;
	current_time = time;

	if(dt > 0.25)
	{
		dt = 0.25;
	}
	return dt;
}



//*******************************Threading************************************

void Game::start()
{
	thread = (HANDLE)_beginthreadex(NULL,0,threadFunc,this,0,NULL);
}

int Game::run()
{
	while(running)
	{
		float dtime = calculate_dt();
		accumlator += dtime;
		while (accumlator >= dt)
		{
			SimulationLoop(dt);
			write_b_to_packet();
			accumlator -=dt;
		}
		Render();
	}
	return 0;
}