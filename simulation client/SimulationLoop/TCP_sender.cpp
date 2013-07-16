#include "TCP_sender.h"


TCP_sender::TCP_sender(void)
{
	ready = false;
}

TCP_sender::TCP_sender(sockaddr_in address )
{
	ready = false;
	server_addr = address;
	send_socket = socket(AF_INET,SOCK_DGRAM,0);
}

TCP_sender::~TCP_sender(void)
{
}

void TCP_sender::setready(bool input)
{
	ready = input;
}

bool TCP_sender::isready()
{
	return ready;
}

bool TCP_sender::setup(sockaddr_in address)
{
	server_addr = address;
	send_socket = socket(AF_INET,SOCK_DGRAM,0);
	ready = true;
	return true;
}
void TCP_sender::close()
{
	closesocket(send_socket); //only use this function when closing program, send socket doesn't need to close otherwise
}

void TCP_sender::start()
{
	
	thread = (HANDLE)_beginthreadex(NULL,0,threadFunc,this,0,NULL);
	int result = SetThreadAffinityMask(thread,2);//core 2 = 2, core 3 = 4????
}


int TCP_sender::run()
{
	while(true)
	{
		Sleep(1);
	}
}

bool TCP_sender::send(string msg)
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

void TCP_sender::set_address(sockaddr_in address)
{
	server_addr = address;
}