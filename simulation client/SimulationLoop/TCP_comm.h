#pragma once
#include <iostream>
#include <winsock2.h>
#include <process.h>
#include<Windows.h>
#include <string>
#include <vector>
#include <sstream>
#include "shared_memory.h"
#include "shared_struct.h"
#pragma comment(lib,"ws2_32.lib")
using namespace std;

class TCP_comm
{
public:
	TCP_comm(shared_memory *shared);
	TCP_comm(void);
	~TCP_comm(void);
	bool setup();
	bool connect_to_server();
	void disconnect();
	void adjust_nettime(int input);
	bool recieve();
	bool idle();
	int run();
	void start();
private:
	SOCKET sender;
	SOCKET reciever;
	SOCKET broadcaster;
	sockaddr_in broadcast_addr;
	sockaddr_in server_addr;
	HANDLE thread;
	shared_memory* shared_data;
	float current_time;
	float accumlator;
	float dt;
	int nettime;
	bool running;
	std::vector<balls_packet> balls;
	void update_balls();
	float Get_time();
	void translate_balls(string &input, int size);
	bool check_dt(); //check if its time to update

	static unsigned __stdcall threadFunc(void *param) {
		if (param)
			return ((TCP_comm*)param)->run();
		return 1;  // Return error
	}
};

