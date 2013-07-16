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
#include "TCP_sender.h"
#pragma comment(lib,"ws2_32.lib")
using namespace std;
class networking;
class TCP_recv
{
public:
	TCP_recv(shared_memory *shared,TCP_sender* send_thread,networking* net_thread);
	TCP_recv(void);
	
	~TCP_recv(void);
	bool setup();
	void wait_for_server();
	void disconnect();
	void adjust_nettime(int input);
	void recieve();
	bool idle();
	int run();
	void start();
	bool isconnected();
private:
	//SOCKET sender;
	SOCKET reciever;
	//SOCKET broadcaster;
	//sockaddr_in broadcast_addr;
	networking* network_ctr;
	TCP_sender* sender;
	HANDLE thread;
	shared_memory* shared_data;
	float current_time;
	float accumlator;
	float dt;
	int nettime;
	bool running;
	bool connected;
	std::vector<balls_packet> balls;

	void update_balls();
	void update_qt(std::vector<qt_packet>* input);
	void update_grid(std::vector<grid_type>* input);
	void update_lines(std::vector<line_packet>* input);
	void update_ropes(std::vector<rope_packet>* input);
	void update_views(std::vector<view>* input);

	void recv_balls(string input);
	void recv_qt(string input);
	void recv_lines(string input);
	void recv_grid(string input);
	void recv_ropes(string input);
	void recv_status(string input);
	void recv_views(string input);


	float Get_time();

	void send_view();
	void send_changes();

	void translate_qt(string &input, int size);
	void translate_balls(string &input, int size);
	void translate_lines(string &input, int size);
	void translate_grid(string &input, int size);
	void translate_rope(string &input, int size);
	void translate_status(string&input);
	void translate_views(string&input, int size);


	bool check_dt(); //check if its time to update

	static unsigned __stdcall threadFunc(void *param) {
		if (param)
			return ((TCP_recv*)param)->run();
		return 1;  // Return error
	}
};

