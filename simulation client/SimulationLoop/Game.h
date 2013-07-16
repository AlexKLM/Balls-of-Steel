#pragma once
#include <process.h>
#include <Windows.h>
#include "Circle.h"
#include "Grid.h"
#include "line.h"
#include "ContactManifold.h"
#include <vector>;
#include "QT_square.h"
#include "shared_struct.h"
#include <concurrent_vector.h>
using namespace std;
using namespace Concurrency;

class Game
{
public:
	Game(HDC hdc,concurrent_vector<balls_packet>*_ball);
	~Game(void);
	void Render();
	void Update();	
	void SimulationLoop(float dt);
	void add_circle(float x, float y);
	void write_b_to_packet();
	int run();
	void start();
private:
	concurrent_vector<balls_packet>*ball;
	void setup_world();
	bool running;
	float get_time();
	float calculate_dt();

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
	
	void add_circle();

	static unsigned __stdcall threadFunc(void *param) {
		if (param)
			
			return ((Game*)param)->run();
		return 1;  // Return error
	}


	HANDLE thread;


	float current_time;
	float accumlator;
	float dt;

	QT_square Q_tree;
	HDC   m_hdc;
	float m_dt;
	float m_previousTime;
	line *testline;
	line *lines[12];
	Grid game_grid[5][5];
	ContactManifold *m_manifold;
	std::vector<Circle>* Circlelist;
	float counter;
};

