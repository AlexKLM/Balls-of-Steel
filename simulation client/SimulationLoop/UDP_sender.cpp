#include "UDP_sender.h"


UDP_sender::UDP_sender(void)
{
	ready = false;
}

UDP_sender::UDP_sender(sockaddr_in address )
{
	ready = false;
	server_addr = address;
	send_socket = socket(AF_INET,SOCK_DGRAM,0);
}

UDP_sender::~UDP_sender(void)
{
}

void UDP_sender::setready(bool input)
{
	ready = input;
}

bool UDP_sender::isready()
{
	return ready;
}

bool UDP_sender::setup(sockaddr_in address)
{
	server_addr = address;
	send_socket = socket(AF_INET,SOCK_DGRAM,0);
	ready = true;
	return true;
}
void UDP_sender::close()
{
	closesocket(send_socket); //only use this function when closing program, send socket doesn't need to close otherwise
}

void UDP_sender::start()
{
	
	thread = (HANDLE)_beginthreadex(NULL,0,threadFunc,this,0,NULL);
	int result = SetThreadAffinityMask(thread,2);//core 2 = 2, core 3 = 4????
}


int UDP_sender::run()
{
	while(true)
	{
		Sleep(1);
	}
}

bool UDP_sender::send(string msg)
{
	int result = sendto(send_socket, msg.c_str(), msg.length()+1, 0,(sockaddr *)&server_addr,sizeof(server_addr));
	if(result != SOCKET_ERROR)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void UDP_sender::set_address(sockaddr_in address)
{
	server_addr = address;
}