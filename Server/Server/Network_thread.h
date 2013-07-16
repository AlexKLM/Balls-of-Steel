#pragma once
#include "shared_struct.h"
#include <WinSock2.h>
#include <iostream>
#include "Circle.h"
#include "udp_communicater.h"
#include "RopePts.h"
#include <vector>
#include <process.h>
#include "Game.h"
#include "shared_struct.h"
#include "Udp_listener.h"
//#include <concurrent_vector.h>
#include <windows.h>
using namespace std;
//using namespace Concurrency;

class Network_thread
{
//manage all network process
public:
	Network_thread(Game* _game,CRITICAL_SECTION* c_lock);
	~Network_thread(void);
	Network_thread(void);
	void shutdown();
	void adjust_nettime(int input);
	void start();
private:
	std::vector<udp_communicater*> workers;
	std::vector<view> views;
	//concurrent_vector<balls_packet>b_pack;
	Game* simulator;
	HANDLE thread;
	CRITICAL_SECTION *cs;
	status states;
	string status_data;

	string view_data;

	float last_n;//last network time
	int act_net_s;//actual net/s
	int des_net_s;//desired net/s

	Udp_listener listener;
	string ball_data;
	string rope_data;
	int size;
	int ropesize;
	//string grid_data;
	string Q_tree_data;
	//string level_data;
	int level_size;
	float current_time;
	int nettime;
	int run();
	float Get_time();


	void update_changes();
	void update_views();
	void update_status();
	void update_physics_state();
	void update_rope_state();
	void update_QT();
	bool check_dt();
	static unsigned __stdcall threadFunc(void *param) {
		if (param)
			
			return ((Network_thread*)param)->run();
		return 1;  // Return error
	}
};

