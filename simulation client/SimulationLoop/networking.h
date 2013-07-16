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
#include "UDP_sender.h"
#include "UDP_recv.h"
class networking //networking thread will broadcast and manage sender and reciever thread
{
private:
	UDP_recv UDP_reciever; 
	UDP_sender UDP_send; //both networking thread and TCP_reciver gets control of this
	bool no_connection;//if no connection, use the broadcast socket to send
	SOCKET s_broadcast;
	sockaddr_in broadcast_addr;
	int port_num; //make this to be able to set in config
	int run();
	int broadcast_freq;//broadcast how often
	int timeout_limit;
	float last_sucess; //last sucessful send/recv
	void close_sockets();//if connection is lost then use this
	float Get_time();
	bool check_connected();
	bool check_dt();
	bool check_timeout();
	bool waiting;
	void broadcast(string message);
	float current_time;
	shared_memory *shared_data;
public:

	void add_ball(float x, float y);
	void add_air(float x, float y);
	void set_waiting(bool input);//used by UDP_recv
	void request_reset();
	void close();
	void togglepause();
	void togglerope();
	HANDLE thread;
	void update_lastsucess();
	void start();
	static unsigned __stdcall threadFunc(void *param) {
		if (param)
			return ((networking*)param)->run();
		return 1;  // Return error
	}
	bool setup();
	networking(int port,shared_memory *shared);
	networking(void);
	~networking(void);
};

