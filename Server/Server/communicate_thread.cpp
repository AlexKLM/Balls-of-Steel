#include "communicate_thread.h"
using namespace std;

struct test{
	float x;
	float y;
	float angle;
};


test testpacket;

communicate_thread::communicate_thread(void)
{
	
	testpacket.x = 50.5f;
	testpacket.y = 20.35f;
	testpacket.angle =11.3f;
}


communicate_thread::~communicate_thread(void)
{
}

bool communicate_thread::disconnect()
{
	closesocket(s);
	//WSACleanup();
	cout<<"disconnected"<<endl;
	return true;
}

bool communicate_thread::set_socket(SOCKET _s)
{
	s = _s;
	listen_request();
	return true;
}

void communicate_thread::listen_request()
{
	//while(true)
	//{
		char buffer;
		//SOCKET s1 = accept(s, NULL, NULL);
		if (s==INVALID_SOCKET) {
			cerr << "Accept failed with " << WSAGetLastError() << endl;
		} else if (recv(s, &buffer, 1, 0)==SOCKET_ERROR) {
			cerr << "Receive failed with " << WSAGetLastError() << endl;
		} else {
			cout << "Message= " << buffer << endl;

			if (send(s, "hello", 6, 0)==SOCKET_ERROR) {
				cerr << "Send failed with " << WSAGetLastError() << endl;
			}

			//send(s1, "hello2", 6, 0);
		}

		// Delay
		cout << "Waiting..." << endl; //char ch; cin >> ch;

	//}
		disconnect();
}