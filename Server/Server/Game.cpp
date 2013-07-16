#include "Game.h"
                             // Header File For The OpenGL32 Library

Game::Game(void) : m_previousTime(0)
{
	//Default values
	friction = 0.05;
	line_elasticity = 0.5;
	circle_elasticity = 0.8;
	last_airjet = 0;
	willreset = false;
	last_loop = 0;
	running = true;
	current_time = 0;
	//accumlator = 0;
	dt = 0.0166;
	userope = false;
	Circlelist = new concurrent_vector<Circle>();
	paused = false;
	a_pps = 0;
	pps = 1000;
	counter = 0;
	//game_grid[0].setup(-5,-5,2);
	//add_circle();
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
	Q_tree.setused(true);
	qt_size = 0;



	//rope.debug_setup();

	rope.setup(Vector2f(-1,3),Vector2f(1,3),15,0.05,0.15);


	setup_world();

	/*for(int i = 0; i < 12; i++)
	{
		Q_tree.insert(lines[i]);
	}*/

	m_manifold = new ContactManifold();
}

Game::Game(CRITICAL_SECTION *c_lock) : m_previousTime(0)
{
//Default values
	counter = 1000;
	test_size = 0;
	cs = c_lock;
	friction = 0.05;
	line_elasticity = 0.5;
	circle_elasticity = 0.8;
	last_airjet = 0;
	last_loop = 0;
	running = true;
	current_time = 0;
	//accumlator = 0;
	dt = 0.0166;
	userope = false;
	Circlelist = new concurrent_vector<Circle>();
	paused = false;
	a_pps = 0;
	pps = 1000;
	counter = 0;
	//game_grid[0].setup(-5,-5,2);
	//add_circle();
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
	Q_tree.setused(true);
	qt_size = 0;



	//rope.debug_setup();

	rope.setup(Vector2f(-1,3),Vector2f(1,3),15,0.05,0.15);


	setup_world();

	//for(int i = 0; i < 12; i++)
	//{
	//	Q_tree.insert(lines[i]);
	//}

	m_manifold = new ContactManifold();
}

void Game::reset()
{
	EnterCriticalSection(cs);
	Q_tree.remove_objects();
	m_manifold->reset();
	Circlelist->clear();
	rope.reset();
	willreset = false;
	LeaveCriticalSection(cs);
}

void Game::set_reset()
{
	willreset = true;
}

void Game::add_circle()
{
	Circle c;
	c.SetPos(-1.5,2);
	c.SetVel(0,0);
	//c.add_force(0,0);
	c.SetMass(1.0f);
	c.SetFric(friction);
	c.SetElas(line_elasticity);
	c.SetCirElas(circle_elasticity);
	Circlelist->push_back(c);

	//c.SetPos(0.5,2);
	//c.SetVel(2,0);
	//c.SetMass(1.0f);
	//Circlelist.push_back(c);
}

void Game::toggle_rope()
{
	if(!userope)
	{
		rope.reset();
		userope = true;
	}
	else
	{
		userope = false;
	}
}

void Game::airjet(float x, float y)
{
	float radius = 2;
	Vector2f pt1 = Vector2f(0,-5.0);
	Vector2f pt2 = Vector2f(x,y);
	float force = pt1.distance(pt2);
	Vector2f dir = pt1 - pt2;
	dir.normalise();
	dir = dir*force*1000; //should be dynamic, as modifiyer for force
	last_airjet = dir.length();
	for(int i = 0; i < Circlelist->size();i++)
	{

		Vector2f pos = Circlelist->at(i).GetPos();
		float dist = pos.distance(pt1) - (Circlelist->at(i).GetRadius() + radius);
		if(dist <= 0.0f)
		{
			Circlelist->at(i).SetAirforce(dir);
		}
	}
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

	//lines.push_back(line());
	//lines.back().setup(-5,5,5,5);

	lines[1] = new line();
	lines[1]->setup(-5,5,-5,-3);

	//lines.push_back(line());
	//lines.back().setup(-5,5,-5,-3);

	lines[2] = new line();
	lines[2]->setup(-5,-3,-3,-3);

	//lines.push_back(line());
	//lines.back().setup(-5,-3,-3,-3);

	lines[3] = new line();
	lines[3]->setup(-3,-3,-1,-5);

	//lines.push_back(line());
	//lines.back().setup(-3,-3,-1,-5);

	lines[4] = new line();
	lines[4]->setup(-1,-5,1,-5);

	//lines.push_back(line());
	//lines.back().setup(-1,-5,1,-5);

	lines[5] = new line();
	lines[5]->setup(1,-5,3,-3);

	//lines.push_back(line());
	//lines.back().setup(1,-5,3,-3);

	lines[6] = new line();
	lines[6]->setup(3,-3,5,-3);

	//lines.push_back(line());
	//lines.back().setup(3,-3,5,-3);

	lines[7] = new line();
	lines[7]->setup(5,-3,5,5);

	//lines.push_back(line());
	//lines.back().setup(5,-3,5,5);

	lines[8] = new line();
	lines[8]->setup(-5,-1,-3,-1);

	//lines.push_back(line());
	//lines.back().setup(-5,-1,-3,-1);

	lines[9] = new line();
	lines[9]->setup(-3,-1,-3,1);

	//lines.push_back(line());
	//lines.back().setup(-3,-1,-3,1);

	lines[10] = new line();
	lines[10]->setup(1,1,3,-1);

	//lines.push_back(line());
	//lines.back().setup(1,1,3,-1);

	lines[11] = new line(); //set last line to always be the one line that get replace with rope, so its easier to disable it
	lines[11]->setup(-1,3,1,3);

	//lines.push_back(line());
	//lines.back().setup(-1,3,1,3);

	line_size = 12;
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


	EnterCriticalSection(cs);

	// Handle static collisions
	StaticCollisionDetection();
	string output;
	qt_size = Q_tree.output(&output);
	QT_data = output;
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
	/*if(counter < 0)
	{
		if(test_size != 150)
		{
			add_circle();
			cout<<"added test ball "<<Circlelist->size()<<endl;
			test_size++;
			counter = 10;
		}
	}
	else
	{
		counter--;
	}*/

	LeaveCriticalSection(cs);

}

//**************************Handle static collisions***********************
void Game::StaticCollisionDetection()
{
	bool loop = true;
	int max_loop = 50;
	int loop_no = 0;
	std::vector<Circle*> possible_Cs = std::vector<Circle*>();
	//std::vector<line*> possible_Ls;
	int size = line_size;
	if(userope)
	{
		size-=1;
	}
	concurrent_vector<RopePts>* ropepts = rope.get_RopePts();

	while(loop && loop_no < max_loop)
	{
		loop = false;
		//for(int l = 0; l < size; l++)
		//{
		//	for(int i = 0; i < Circlelist->size();i++)
		//	{
		//		loop |=StaticSphereLineCollisionDetection(&Circlelist->at(i),lines[l]);
		//	}
		//}

		for (int i = 0; i < Circlelist->size(); i++)
		{
		
			possible_Cs.clear();
			Q_tree.generate_possible_list(&possible_Cs,&Circlelist->at(i));

			/*for(int c = i+1; c< Circlelist->size();c++)
			{
				loop |= StaticSphereCollisionDetection(&Circlelist->at(i), &Circlelist->at(c));
			}*/
			for(int c = 0; c<possible_Cs.size(); c++)
			{
				
				if(possible_Cs[c]->getID() != Circlelist->at(i).getID() )
				{
					loop |= StaticSphereCollisionDetection(&Circlelist->at(i), possible_Cs[c]);
				}
			}

			for(int l = 0; l < size; l++)
			{
				loop |=StaticSphereLineCollisionDetection(&Circlelist->at(i),lines[l]);
			}
			if(userope)
			{
				//for( int i = 0; i < Circlelist->size(); i++)
				//{
					for(int r = 0; r<ropepts->size();r++)
					{
						loop |= StaticSphereCollisionDetection(&Circlelist->at(i),ropepts->at(r).get_circle());
					}
				//}
			}
		}

		

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

void Game::toggle_paused()
{
	if(!paused)
	{
		paused = true;
	}
	else
	{
		paused = false;
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
		if(!circle1->is_fixed())
		{
		pos1 = pos1 - (colNormal * dist * 1.1f);
		circle1->SetPos(pos1.GetX(), pos1.GetY());
		circle1->set_stationary(false);
		}
		if(!circle2->is_fixed())
		{
		pos2 = pos2 + (colNormal * dist* 1.1f);
		circle2->SetPos(pos2.GetX(), pos2.GetY());
		circle2->set_stationary(false);
		}
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
		float dist = center.distance(closest_p) - (circle1->GetRadius());
		
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
	
	srand (time(NULL));
	int mass = rand() % 8 + 3;
	c.SetMass(mass);
	c.SetFric(friction);
	c.SetElas(line_elasticity);
	c.SetCirElas(circle_elasticity);

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
	if(userope)
	{
		rope.calculate_physics(m_dt);
	}
}

//**************************Handle dynamic collisions***********************
void Game::DynamicCollisionDetection()
{

		concurrent_vector<RopePts>* ropepts = rope.get_RopePts();
		int size = line_size;
		if(userope)
		{
			size-=1;
		}
	for (int i = 0; i < Circlelist->size(); i++)
	{
		for (int next = i+1;  next<Circlelist->size();next++)
		{
			Circlelist->at(i).CollisionWithCircle(&Circlelist->at(next),m_dt);
			
		}
		if(userope)
		{
			for(int r = 0; r<ropepts->size();r++)
			{
				Circlelist->at(i).CollisionWithCircle(ropepts->at(r).get_circle(),m_dt);
			}
		}
		for(int l = 0; l<size;l++)
		{
			Circlelist->at(i).CollisionWithLine(lines[l],m_dt);
		}
	}

	/*for each(line *l in lines)
	{
		for(int i = 0; i<Circlelist->size();i++)
		{
			
		}
	}*/
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

}

void Game::set_pps(int input)
{
	pps+=input;
	if(pps <=0)
	{
		pps = 1;
	}
}

void Game::set_fric(float input)
{
	friction = input;
}

void Game::set_e_ball(float input)
{
	circle_elasticity = input;
}

void Game::set_e_line(float input)
{
	line_elasticity = input;
}

string Game::get_level_data()
{
	
	string output = "";
	stringstream ss;
	for each(line *l in lines)
	{
		ss.str("");
		ss<<l->GetP1().GetX();
		output.append(ss.str());
		ss.str("");
		output.append(" ");
		ss<<l->GetP1().GetY();
		output.append(ss.str());
		ss.str("");
		output.append(" ");
		ss<<l->GetP2().GetX();
		output.append(ss.str());
		ss.str("");
		output.append(" ");
		ss<<l->GetP2().GetY();
		output.append(ss.str());
		output.append(" ");
	}
	return output;
}

string Game::get_grid()
{
	string output = "";
	stringstream ss;
	for each(Grid g in game_grid)
	{
		ss.str("");
		ss<<g.getX();
		output.append(ss.str());
		output.append(" ");
		ss.str("");
		ss<<g.getY();
		output.append(ss.str());
		output.append(" ");
		ss.str("");
		ss<<g.getSize();
		output.append(ss.str());
		output.append(" ");
		ss.str("");
	}
	return output;
}

int Game::get_level_size()
{
	int result =  line_size;
	return result;
}

int Game::get_grid_size()
{
	return (sizeof(game_grid)/sizeof(*game_grid));
}

void Game::update_QT()
{
	for(int i = 0; i < Circlelist->size(); i++)
	{
		Q_tree.insert(&Circlelist->at(i));
	}
}

int Game::get_apps()
{
	return a_pps;
}

int Game::get_pps()
{
	return pps;
}

int Game::get_qt_size()
{
	return qt_size;
}

float Game::get_fric()
{
	return friction;
}

float Game::get_elasticity_l()
{
	return line_elasticity;
}

float Game::get_elasticity_b()
{
	return circle_elasticity;
}

float Game::get_lastairjet()
{
	return last_airjet;
}

string Game::get_q_T()
{
	return QT_data;
}

bool Game::hasrope()
{
	return userope;
}

//**************************Update the physics calculations on each object***********************
void Game::UpdateObjectPhysics()
{
	for(int i = 0; i < Circlelist->size();i++)
	{
		Circlelist->at(i).Update(m_dt);
	}
	if(userope)
	{
		rope.update(m_dt);
	}
	/*m_Circle1->Update();
	m_Circle2->Update();
	m_Circle3->Update();*/
}

//**************************Render and display the scene in OpenGL***********************

concurrent_vector<Circle>* Game::get_balls()
{
	return Circlelist;
}

concurrent_vector<RopePts>* Game::get_rope()
{
	return rope.get_RopePts();
}

//*************************Run at every end of frame************************************

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

bool Game::check_dt()
{
	const float time = get_time();
	float delt = (time - current_time)*1000;
	float value = 1000/(float)pps;
	if(delt >value) //
	{
		dt = (delt/1000);
		current_time = time;
		return true;
	}
	else
	{
		return false;
	}
}


//*******************************Threading************************************

void Game::start()
{
	thread = (HANDLE)_beginthreadex(NULL,0,threadFunc,this,0,NULL);
	int result = SetThreadAffinityMask(thread,4); //core 2 = 2, core 3 = 4????
}

int Game::run()
{
	while(running)
	{
		//float dtime = calculate_dt();
		//accumlator += dtime;
		if(check_dt())
		{
			float current = get_time();
			a_pps = 1000/((current - last_loop)*1000);
			last_loop = current;
			if(!paused)
			{
				SimulationLoop(dt);
			}
		}
		if(willreset)
		{
			reset();
		}
		//while (accumlator >= dt)
		//{
		//	
		//	//write_b_to_packet();
		//	//string output;
		//	//qt_size = Q_tree.output(&QT_data);
		//	//QT_data = output;
		//	accumlator -=dt;
		//}

	}
	return 0;
}