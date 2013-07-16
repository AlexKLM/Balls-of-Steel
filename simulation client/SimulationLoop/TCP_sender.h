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

class TCP_sender
{
private:
	HANDLE thread;
	SOCKET send_socket;
	sockaddr_in server_addr;
	bool ready;
	int run();
public:

	bool isready();//ready to send;
	void setready(bool input);
	void start();
	bool send(string msg);
	bool setup(sockaddr_in address);
	void close();
	void set_address(sockaddr_in address);
	TCP_sender(sockaddr_in address);
	TCP_sender(void);
	~TCP_sender(void);

	static unsigned __stdcall threadFunc(void *param) {
		if (param)
			return ((TCP_sender*)param)->run();
		return 1;  // Return error
	}
};

