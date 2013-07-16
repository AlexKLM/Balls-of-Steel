#include "tcp_listener.h"


tcp_listener::tcp_listener(concurrent_vector<balls_packet>*pack)
{
	b_pack = pack;
}


tcp_listener::~tcp_listener(void)
{
}


int tcp_listener::run()
{
	//cout<<"intiaite tcp listener"<<endl;
	while(true)
	{
			accepted_connection();
	}
	return 0;
}

bool tcp_listener::setup(std::vector<tcp_communicater*>* _workers)
{
	WORD wVersionRequested = MAKEWORD( 2, 0 );
	connection_num = 0;
	workers = _workers;
	// Startup windows sockets
	WSADATA wsaData;
	if ( WSAStartup( wVersionRequested, &wsaData )) {
		//cerr << "Socket initialisation failed" << endl;
		return false;
	}

	sockaddr_in peer;
	peer.sin_family = AF_INET;
	peer.sin_port = htons(9171);	// port 9171
	peer.sin_addr.S_un.S_addr = htonl( INADDR_ANY );

		// Create listening socket
	s = socket(AF_INET, SOCK_STREAM, 0);
	if (s==INVALID_SOCKET) {
		//cerr << "Create socket failed" << endl;
		return false;
	} else if (bind(s, (sockaddr *)&peer, sizeof(peer))==SOCKET_ERROR) {
		//cerr << "Bind failed with " << WSAGetLastError()  << endl;
		return false;
	} else if (listen(s, 5)==SOCKET_ERROR) {
		//cerr << "Listen failed with " << WSAGetLastError()  << endl;
		return false;
	}
	return true;
}

void tcp_listener::start()
{
	thread = (HANDLE)_beginthreadex(NULL,0,threadFunc,this,0,NULL);
}

void tcp_listener::accepted_connection()
{
	//need to change this so that it passed the socket to another thread
	SOCKET s1 = accept(s,NULL,NULL);
	if(s1 == INVALID_SOCKET)
	{
		//bool what = true;
		//cerr << "Accept failed with " << WSAGetLastError() << endl;
	}
	else
	{
		workers->push_back(new tcp_communicater(connection_num,s1,b_pack));
		workers->back()->start();
		connection_num++;
	}
	
	
}

bool tcp_listener::close()
{
	// Cleanup windows sockets
	WSACleanup();
	return true;
}