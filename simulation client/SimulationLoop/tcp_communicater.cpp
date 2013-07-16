#include "tcp_communicater.h"


tcp_communicater::tcp_communicater(int _id,SOCKET _s,	concurrent_vector<balls_packet>*_pack)
{
	intiaited = false;
	id = _id;
	s = _s;
	b_pack = _pack;
}


tcp_communicater::~tcp_communicater(void)
{
}


bool tcp_communicater::disconnect()
{
	closesocket(s);
	//cout<<"disconnected"<<endl;
	return true;
}

bool tcp_communicater::set_socket(SOCKET _s)
{
	s = _s;
	listen_request();
	return true;
}

int tcp_communicater::run()
{
//	cout<<"intiaite TCP thread "<< id<<endl;
	if (s==INVALID_SOCKET) {
			//cerr << "Accept failed with " << WSAGetLastError() << endl;
		}
	//cout<<"connection accepted"<<endl;
	bool connected = true;
	while(connected)
	{
		char buffer[512];
		int Result = recv(s, buffer, sizeof(buffer), 0);

		if (Result == SOCKET_ERROR) {
			if(WSAGetLastError() == 10054)
			{
				//cout<<"client dropped connection? closing thread"<<endl;
				connected = false;
			}
			//else
			//cerr << "Receive failed with " << WSAGetLastError() << endl;
		} 
		else if (Result == 0)
		{
			connected = false;
		}
		else {
			//packet.id = id;
			//cout << "Message= " << buffer << endl;
			string str(buffer);
			if(str == "rb")
			{
				sendballs();
			}
			//if (send(s, (char*)&packet, sizeof(test_packet), 0)==SOCKET_ERROR) {
					//cerr << "Send failed with " << WSAGetLastError() << endl;
			//}
			//cout<<"waiting..."<<endl;
			//send(s1, "hello2", 6, 0);
		}

		// Delay
		

	}
	disconnect();
	//cout << "dropped connection" << endl; //char ch; cin >> ch;
	return 0;
}

void tcp_communicater::sendballs()
{
	char buffer[512];
	int size = b_pack->size();
	ostringstream convert;
	convert << size; 
	string result = convert.str();
	
	//strcpy(buffer,result);
	if (send(s, (char*)result.c_str(),result.length()+1, 0)==SOCKET_ERROR) {
		cerr << "Send failed with " << WSAGetLastError() << endl;
	}
	memset(buffer,0,sizeof(buffer));
	int Result = recv(s, buffer, sizeof(buffer), 0);
	string reply(buffer);
	if(reply == "ready")
	{
		for(int i =0;i<size;i++)
		{
			send(s, (char*)&b_pack->at(i),sizeof(balls_packet), 0);
		}
	}
}

void tcp_communicater::start()
{
	thread = (HANDLE)_beginthreadex(NULL,0,threadFunc,this,0,NULL);
	intiaited = true;
}

void tcp_communicater::kill()
{

	CloseHandle(thread);
	thread = 0;
}

bool tcp_communicater::isRunning()
{
	DWORD result;
	if(!intiaited)
	{
		return true;
	}
	result = WaitForSingleObject(thread,0);
	if(result == WAIT_TIMEOUT)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void tcp_communicater::listen_request()
{
	//while(true)
	//{
		char buffer;
		
		//SOCKET s1 = accept(s, NULL, NULL);
		if (s==INVALID_SOCKET) {
			//cerr << "Accept failed with " << WSAGetLastError() << endl;
		} else if (recv(s, &buffer, 1, 0)==SOCKET_ERROR) {
			//cerr << "Receive failed with " << WSAGetLastError() << endl;
		} else {
			//cout << "Message= " << buffer << endl;
			//packet.id = id;
			//if (send(s, (char*)&packet, sizeof(test_packet), 0)==SOCKET_ERROR) {
			//	cerr << "Send failed with " << WSAGetLastError() << endl;
			//}

			//send(s1, "hello2", 6, 0);
		}

		// Delay
		//cout << "Waiting..." << endl; //char ch; cin >> ch;

	//}
		disconnect();
}
