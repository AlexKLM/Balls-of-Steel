#pragma once
#include <iostream>
#include <winsock2.h>


//handle data passsing
class communicate_thread
{
private:
	void listen_request();
	void send_data();
	bool disconnect();
	SOCKET s;
public:
	communicate_thread(void);
	~communicate_thread(void);
	bool set_socket(SOCKET _s);
	void kill();
};

