#include "networking.h"


networking::networking(void)
{
	port_num = 9173;
	waiting = false;
	no_connection = true;
	broadcast_addr.sin_family = AF_INET;
	broadcast_addr.sin_port = htons(9172);	// port 9171 udp 7777
	broadcast_addr.sin_addr.S_un.S_addr = INADDR_BROADCAST;
	last_sucess = Get_time();
	broadcast_freq = 5000;//default is half a second
	current_time = 0;
	timeout_limit = 2; //2 second qualifys as timeout
}

networking::networking(int port,shared_memory *shared)
{
	port_num = port;
	waiting = false;
	shared_data = shared;
	no_connection = true;
	last_sucess = Get_time();
	broadcast_addr.sin_family = AF_INET;
	broadcast_addr.sin_port = htons(9172);	// port 9171 udp 7777
	broadcast_addr.sin_addr.S_un.S_addr = INADDR_BROADCAST;
	broadcast_freq = 5000;//default is half a second
	timeout_limit = 2; //2 second qualifys as timeout
	current_time = 0;
	
}

networking::~networking(void)
{
	
}

void networking::update_lastsucess()
{
	last_sucess = Get_time();
}

bool networking::setup()
{
	// Create version identifier
	WORD wVersionRequested = MAKEWORD( 2, 0 );
	// Startup windows sockets
	WSADATA wsaData;
	if ( WSAStartup( wVersionRequested, &wsaData )) {
		cerr << "Socket initialisation failed" << endl;
		return false;
	}

	s_broadcast = socket(AF_INET,SOCK_DGRAM,0);

	char optval = '1';
	int optlen = sizeof(int);
	setsockopt(s_broadcast,SOL_SOCKET,SO_BROADCAST,&optval,optlen);
	UDP_send = UDP_sender();
	UDP_send.start();
	UDP_reciever = UDP_recv(shared_data,&UDP_send,this);
	UDP_reciever.start();
	return true;
}

void networking::set_waiting(bool input)
{
	waiting = input;
}

void networking::broadcast(string message)  //only broadcast when lost connection! otherwise might messed up the reciver
{
	int size = sizeof(broadcast_addr);
	sendto(s_broadcast, message.c_str(), message.length()+1, 0,(sockaddr *)&broadcast_addr,sizeof(broadcast_addr));
}

void networking::start()
{
	thread = (HANDLE)_beginthreadex(NULL,0,threadFunc,this,0,NULL);
	SetThreadAffinityMask(thread,2); //core 2 = 2, core 3 = 4????
}

void networking::request_reset()
{
	string message = "reset";
	if(UDP_send.isready())
	{
		UDP_send.send(message);
	}
}

float networking::Get_time()
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

bool networking::check_dt()
{
	const float time = Get_time();
	float dt = (time - last_sucess)*1000;
	if(dt >1000/broadcast_freq) //the 100 should be dynamic
	{
		return true;
	}
	else
	{
		return false;
	}
}

void networking::close_sockets()
{
	UDP_reciever.disconnect();
	UDP_send.setready(false);
}

bool networking::check_timeout()
{
	const float time = Get_time();
	float dt = (time - last_sucess);
	if(dt >timeout_limit) //the 100 should be dynamic
	{
		//current_time = time;
		return true;
	}
	else
	{
		return false;
	}
}

void networking::add_ball(float x, float y)
{
	string message = "";
	stringstream ss;
	ss.str("");
	message.append("ab ");
	ss<<x<<" "<<y;
	message.append(ss.str());
	if(UDP_send.isready())
	{
		UDP_send.send(message);
	}
}

void networking::togglepause()
{
	string message = "pause";
	if(UDP_send.isready())
	{
		UDP_send.send(message);
	}
}

void networking::togglerope()
{
	string message = "rope";
	if(UDP_send.isready())
	{
		UDP_send.send(message);
	}
}

void networking::add_air(float x, float y)
{
	if(y > -5)
	{
		return; //not pointing up
	}
	string message = "";
	stringstream ss;
	ss.str("");
	message.append("ar ");
	ss<<x<<" "<<y;
	message.append(ss.str());
	if(UDP_send.isready())
	{
		UDP_send.send(message);
	}
}

int networking::run()
{
	string broadcast_msg = "";
	stringstream ss;
	ss<<port_num;
	broadcast_msg.append("hi ");
	broadcast_msg.append(ss.str());
	//broadcast_msg.append(" ");
	ss.str("");
	string msg = broadcast_msg.substr(3);
	ss.str(broadcast_msg.substr(3));
	int num = 0;
	ss>>num;

	while(true)
	{
		//if(check_dt())
		//{
		
			/*if(no_connection)
			{*/
		//current_time = Get_time();
				if(!UDP_send.isready())
				{
					shared_data->set_con("broadcasting");
					broadcast(broadcast_msg);
					if(!waiting)
					{
						UDP_reciever.setup();
						waiting = true;
						//UDP_reciever.wait_for_server();
					}
					Sleep(1500);
				}
				else
				{
					shared_data->set_con("connected");
					if(check_timeout())
					{
						close_sockets();
						no_connection = true;
						waiting = false;
					}
					Sleep(1000);
				}
			/*}
			else
			{
				if(check_timeout())
				{
					close_sockets();
					no_connection = true;
					waiting = false;
				}
			}*/
		//}
	}
}

