#pragma once
#include <winsock2.h>
#include <iostream>
#include "tcp_communicater.h"
#include <vector>
#include <process.h>
#include "shared_struct.h"
#include <concurrent_vector.h>
#include <windows.h>
using namespace std;
using namespace Concurrency;

class tcp_listener
{

public:
	//tcp_listener(void);
	tcp_listener(concurrent_vector<balls_packet>*pack);
	~tcp_listener(void);
	std::vector<tcp_communicater*>* workers;
	//communicate_thread worker_thread;
	bool setup(std::vector<tcp_communicater*>* _workers);
	int run();
	void start();
	bool close();
private:
	SOCKET s;
	static unsigned __stdcall threadFunc(void *param) {
		if (param)
			
			return ((tcp_listener*)param)->run();
		return 1;  // Return error
	}
	HANDLE thread;
	void recieve();
	void accepted_connection();//should pass the accepted socket to a thread
	int connection_num;
	concurrent_vector<balls_packet>*b_pack;

};

