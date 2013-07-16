#pragma once
#include <winsock2.h>
#include <iostream>
#include "udp_communicater.h"
#include <vector>
#include <process.h>
#include "shared_struct.h"
//#include <concurrent_vector.h>
#include "Game.h"
#include <windows.h>
using namespace std;
//using namespace Concurrency;

class Udp_listener
{
private:
	SOCKET s;
	Game* simulator;
	std::vector<sockaddr> adddresses;
	std::vector<udp_communicater*>* workers;
	//concurrent_vector<balls_packet>*b_pack;
	int run();
	static unsigned __stdcall threadFunc(void *param) {
		if (param)
			
			return ((Udp_listener*)param)->run();
		return 1;  // Return error
	}
	HANDLE thread;
	bool check_address(sockaddr address);
	int l_size;
	int g_size;
	string l_data;
	string g_data;
	void udp_listen();

	int connection_num;

public:
	bool setup(Game* sim,std::vector<udp_communicater*>* _workers,int line_size, int grid_size, string level_data, string grid_data);
	Udp_listener(void);
	~Udp_listener(void);
	void start();
	bool close();

};

