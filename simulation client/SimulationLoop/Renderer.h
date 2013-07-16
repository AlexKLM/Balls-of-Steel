#pragma once
#include <process.h>
#include <Windows.h>
#include <gl\gl.h>
#include "shared_struct.h"
#include "AntTweakBar.h"
#include "shared_memory.h"
#include <concurrent_vector.h>
#define _USE_MATH_DEFINES
#include <math.h>
using namespace std;
using namespace Concurrency;
class Renderer
{
public:
	Renderer(void);
	Renderer(shared_memory* shared, HDC _hdc);
	~Renderer(void);
	int run();
	void check_render();
	
	void Render();
	void start();
	void modifyRS(int input);
	void InitializeOpenGL(HWND &hwnd, int width, int height);
private:
	TwBar *Bar;
	float current_time;
	float accumlator;
	float dt;

	string con; //connection state

	float pre_render; ///last render time

	int des_render;
	int actual_render;

	int actual_phy;
	int des_phy;

	int actual_net;
	int des_net;

	int num_balls;
	int num_r_balls;

	float last_air;
	float fric;
	float e_b;
	float e_l;

	HDC   m_hdc;
	int W; 


	int H;
	HANDLE thread;
	bool running;

	shared_memory* shared_data;
	void render_view();
	void RenderRope();
	void RenderAir();
	void RenderBalls();
	void RenderQT();
	void RenderLines();
	void RenderGrid();
	float GetTime();
	bool check_dt();

	static unsigned __stdcall threadFunc(void *param) {
		if (param)
			return ((Renderer*)param)->run();
		return 1;  // Return error
	}
};

