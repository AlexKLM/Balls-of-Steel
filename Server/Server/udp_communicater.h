#pragma once
#include <winsock2.h>
#include <iostream>
#include <vector>
#include <process.h>
#include "shared_struct.h"
#include <sstream>
//#include <concurrent_vector.h>
#include <windows.h>
using namespace std;
// namespace Concurrency;

class udp_communicater
{
public:
	udp_communicater(int _id,sockaddr_in address);
	~udp_communicater(void);
	void start();
	void handle_cmd(string cmd);
	void set_ready(bool input);
	//void set_sendready_b(bool input);
	void kill();

	void send_b_size(int size,string data); //notify the client to be ready
	void send_l_size(int size,string data);
	void send_g_size(int size,string data);
	void send_qt_size(int size,string data);
	void send_r_size(int size, string data);
	void send_status_size(string data);
	void send_view_size(int size, string data);

	void send_status();//sending actual data
	void sendballs();
	void sendropes();
	void sendstatus();
	void sendqt();
	void sendview();
	void sendgrid(string data);
	void sendlines(string data);

	void recv_changes(string data);

	void setview(string data);

	void request_view();
	void request_change();

	view getview();

	int get_net_change();
	int get_phy_change();

	void reset_change();

	bool isready();
	bool isbusy();
	bool isaddr(sockaddr_in input);//check if address is the same
private:
	SOCKET udp;
	sockaddr addr;
	sockaddr_in addr_detail;//for comparison and debug
	bool send_ready; //ok to start sending balls
	bool send_ready_qt;
	int id;
	view client_view;

	int net_change;
	int phy_change;

	string view_data;
	string status_data;
	string ball_data;
	string qt_data;
	string rope_data;
	//concurrent_vector<balls_packet>*b_pack;
	bool busy;
	bool intiaited;
	HANDLE thread;
	bool disconnect();
	int run();
	static unsigned __stdcall threadFunc(void *param) {
		if (param)
			
			return ((udp_communicater*)param)->run();
		return 1;  // Return error
	}
	
};

