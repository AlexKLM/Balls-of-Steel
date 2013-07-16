#include "TCP_comm.h"

TCP_comm::TCP_comm(void)
{
}

TCP_comm::TCP_comm(shared_memory *shared)
{
	shared_data = shared;
	current_time = 0;
	accumlator = 0;
	dt = 0.0166;
	nettime = 100;
}

void TCP_comm::start()
{
	thread = (HANDLE)_beginthreadex(NULL,0,threadFunc,this,0,NULL);
}


TCP_comm::~TCP_comm(void)
{
}

bool TCP_comm::setup()
{
	// Create version identifier
	WORD wVersionRequested = MAKEWORD( 2, 0 );

	// Startup windows sockets
	WSADATA wsaData;
	if ( WSAStartup( wVersionRequested, &wsaData )) {
		cerr << "Socket initialisation failed" << endl;
		return false;
	}
	int timeout = 100;
	// Create socket data space
	//sockaddr_in peer;
	broadcast_addr.sin_family = AF_INET;
	broadcast_addr.sin_port = htons(7777);	// port 9171
	broadcast_addr.sin_addr.S_un.S_addr = INADDR_BROADCAST; //need to change this to dynamic !

	sockaddr_in udp_addr;
	udp_addr.sin_family = AF_INET;
	udp_addr.sin_port = htons(27900);	// port 9171
	udp_addr.sin_addr.S_un.S_addr = htonl( INADDR_ANY );

	// Create transfer socket
	//char buffer;
	sender = socket(AF_INET, SOCK_DGRAM, 0);
	reciever = socket(AF_INET,SOCK_DGRAM,0);
	broadcaster = socket(AF_INET,SOCK_DGRAM,0);
	if (bind(reciever, (sockaddr *)&udp_addr, sizeof(udp_addr))==SOCKET_ERROR)
	{
		
		return 0;
	}
	char optval = '1';
	int optlen = sizeof(int);
	setsockopt(broadcaster,SOL_SOCKET,SO_BROADCAST,&optval,optlen);
}

int TCP_comm::run()
{
	running = true;
	bool connected = false;
	
	while(running)
	{
		if(!connected)
		{
			connected = connect_to_server();
		}
		else
		{
			idle();
		}
		
	}
	
	return 0;
}

bool TCP_comm::connect_to_server()
{
	/*if (s==INVALID_SOCKET) {
		cerr << "Create socket failed" << endl;
		return false;
	}
	else if(connect(s, (sockaddr *)&peer, sizeof(peer))==SOCKET_ERROR)
	{
		cerr << "Connect to peer failed with " << WSAGetLastError() << endl;
		return false;
	}*/
	char buffer[512];
	int size = sizeof(server_addr);
	sendto(broadcaster, "hi", 3, 0,(sockaddr *)&broadcast_addr,sizeof(broadcast_addr));
	recvfrom(reciever,buffer,sizeof(buffer),0,(sockaddr *)&server_addr,&size);
	string str = string(buffer);
	if(str == "ok")
	{
		server_addr.sin_port = htons(7777);
		cout<<"client connected!"<<endl;
		return true;
	}
	else
	{
		return false;
	}
	
	
}

void TCP_comm::disconnect()
{
	/*if(shutdown(s,SD_SEND) == SOCKET_ERROR)
	{
		cerr<<"shutdown failed!"<<WSAGetLastError<<endl;
	}*/
	cout<<"client disconnected"<<endl;
	//closesocket(s);
	WSACleanup();
}

float TCP_comm::Get_time()
{
	static __int64 start = 0;
    static __int64 frequency = 0;

    if (start==0)
    {
        QueryPerformanceCounter((LARGE_INTEGER*)&start);
        QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);
        return 0.0f;
    }

    __int64 counter = 0;
    QueryPerformanceCounter((LARGE_INTEGER*)&counter);
    return (float) ((counter - start) / double(frequency));
}

bool TCP_comm::check_dt()
{
	const float time = Get_time();
	float dt = (time - current_time)*1000;
	if(dt >1000/nettime) //the 100 should be dynamic
	{
		current_time = time;
		return true;
	}
	else
	{
		return false;
	}
}

bool TCP_comm::idle()
{
	char buffer[1025];
	
	int result;
	result = recvfrom(reciever, buffer, sizeof(buffer), 0,NULL,NULL);
	if(result == SOCKET_ERROR)
	{
		return false;
	}
	else if(result == 0)
	{
			return false;
			disconnect();
	}
	else
	{
			
			//string str = string(buffer);
			stringstream ss;
			ss.str(string(buffer));

			int size;
			int msglength;
			ss>>size>>msglength;
			//cout<<size<<endl;
			sendto(sender, "ready", 6, 0,(sockaddr *)&server_addr,sizeof(server_addr));
			balls_packet pack;
			char data [65507];//max buffer size
			
			int result = recvfrom(reciever, data, sizeof(data), 0,0,0);

			string str = string(data);
			if(str.length() == msglength) //skip a cycle if message didn't come through
			{
				translate_balls(str,size);
				update_balls();
			}


			//for(int i = 0; i < size; i++)
			//{
			//	
			//	//if(result == sizeof(balls_packet))
			//	///{
			//	int error = WSAGetLastError();
			//	if(result == sizeof(balls_packet))
			//	{
			//		balls.push_back(pack);	
			//	}
			//	else
			//	{
			//		bool failed = true;
			//	}
			//					
			//	//}
			//	//char* balldata = (char*)malloc(sizeof(balls_packet));
			//	//memcpy(balldata,data[i*sizeof(balls_packet)],sizeof(balls_packet));
			//	//pack = (balls_packet*)&balldata;
			//	
			//}
			
			
	}
	
}

void TCP_comm::translate_balls(string &input, int size)
{
	stringstream ss;
	ss.str(input);
	balls_packet pack;
	balls.clear();
	for(int i = 0; i < size; i ++)
	{
		float x, y,angle,stationary;
		ss>>pack.x>>pack.y>>pack.angle>>pack.stationary;

		balls.push_back(pack);

	}
}

void TCP_comm::adjust_nettime(int input)
{
	nettime += input;
	if(nettime <=0)
	{
		nettime = 1;
	}
}

void TCP_comm::update_balls()
{
	if(shared_data->Acquire())
	{
		std::vector<balls_packet>* b = shared_data->getData();
		
		
		__try
		{
			
			b->clear();
			for(int i = 0; i < balls.size(); i++)
			{
				b->push_back(balls[i]);
			}
		}
		__finally
		{
			balls.clear();
			shared_data->Release();
		}

	}
}
