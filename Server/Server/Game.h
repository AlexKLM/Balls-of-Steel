#pragma once
#include <process.h>
#include <Windows.h>
#include <iostream>
#include "Circle.h"
#include "Grid.h"
#include "line.h"
#include "ContactManifold.h"
#include "Rope.h"
#include "RopePts.h"
#include <vector>;
#include "QT_square.h"
#include "shared_struct.h"
#include <concurrent_vector.h>
using namespace std;
using namespace Concurrency;
class Game
{
public:
	Game(void);
	~Game(void);
	Game(CRITICAL_SECTION *c_lock);
	
	void setlock(CRITICAL_SECTION *c_lock);
	void Render();
	void Update();	
	void toggle_paused();
	void SimulationLoop(float dt);
	void airjet(float x, float y);
	void add_circle(float x, float y);
	void toggle_rope();
	void write_b_to_packet();

	CRITICAL_SECTION *cs;


	int get_pps();
	int get_apps();
	void set_reset();

	void set_pps(int input);
	void set_fric(float input);
	void set_e_ball(float input);
	void set_e_line(float input);

	int run();
	void start();
	bool hasrope();

	concurrent_vector<Circle>* get_balls();
	concurrent_vector<RopePts>* get_rope();

	int get_level_size();
	int get_grid_size();
	int get_qt_size();

	float get_fric();
	float get_elasticity_l();
	float get_elasticity_b();
	float get_lastairjet();


	string get_level_data();
	string get_grid();
	string get_q_T();
private:
	void setup_world();
	bool running;
	bool userope;
	bool paused;
	float get_time();
	void reset();

	float friction;
	float line_elasticity;
	float circle_elasticity;

	float last_airjet;

	float calculate_dt();
	string QT_data;
	int qt_size;

	void StaticCollisionDetection();
	bool StaticSphereCollisionDetection(Circle *circle1, Circle *circle2);
	bool StaticSphereLineCollisionDetection(Circle *circle1, line *line1);
	void CalculateObjectPhysics();
	void DynamicCollisionDetection();
	void continuous_collisiton(float dt);
	void DynamicCollisionResponse();
	void Update_manifold();
	void UpdateObjectPhysics();
	void update_QT();
	bool check_dt();
	void add_circle();

	int test_size;

	static unsigned __stdcall threadFunc(void *param) {
		if (param)
			
			return ((Game*)param)->run();
		return 1;  // Return error
	}


	HANDLE thread;

	float last_loop;//last time the physics loop gets called
	float current_time;


	/*float accumlator;*/
	float dt;

	bool willreset;

	int pps;//desiered physics per second
	int a_pps;//actual physics per second

	QT_square Q_tree;
	HDC   m_hdc;
	float m_dt;
	float m_previousTime;
	Rope rope;

	int line_size;
	//line *testline;
	line *lines[12];
	//std::vector<line> lines;
	Grid game_grid[5][5];
	ContactManifold *m_manifold;
	concurrent_vector<Circle>* Circlelist;
	float counter;
};

