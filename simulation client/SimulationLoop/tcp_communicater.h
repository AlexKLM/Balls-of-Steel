#pragma once
#include <iostream>
#include <winsock2.h>
#include <process.h>
#include "shared_struct.h"
#include <concurrent_vector.h>
#include <string>
#include<Windows.h>
#include <sstream>
using namespace Concurrency;
using namespace std;
class tcp_communicater //send/recv
{
private:
	int run();
	bool disconnect();
	bool intiaited;
	SOCKET s;
	int id;
	concurrent_vector<balls_packet>*b_pack;
	void sendballs();

public:

	static unsigned __stdcall threadFunc(void *param) {
		if (param)
			return ((tcp_communicater*)param)->run();
		return 1;  // Return error
	}

	//tcp_communicater(void);
	tcp_communicater(int _id,SOCKET _s,	concurrent_vector<balls_packet>*_pack);
	~tcp_communicater(void);
	int get_id();
	void listen_request();

	void start();
	bool isRunning();
	HANDLE thread;
	bool set_socket(SOCKET _s);
	void kill();

};

