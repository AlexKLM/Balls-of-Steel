#include "Udp_listener.h"


Udp_listener::Udp_listener()
{
	//b_pack = pack;
	connection_num = 0;
	//accepted = false;
}


Udp_listener::~Udp_listener(void)
{
}


int Udp_listener::run()
{
	while(true)//CHANGE THIS LATER
	{
		udp_listen();
	}
	return 0;
}

bool Udp_listener::setup(Game* sim,std::vector<udp_communicater*>* _workers, int level_size, int grid_size, string level_data, string grid_data)
{
	WORD wVersionRequested = MAKEWORD( 2, 0 );
	connection_num = 0;
	simulator = sim;
	l_size = level_size;
	g_size = grid_size;
	l_data = level_data;
	g_data = grid_data;
	workers = _workers;
	WSADATA wsaData;
	if ( WSAStartup( wVersionRequested, &wsaData )) {
		//cerr << "Socket initialisation failed" << endl;
		return false;
	}

	sockaddr_in peer;
	peer.sin_family = AF_INET;
	peer.sin_port = htons(9172);	// port 9171 udp 7777
	peer.sin_addr.S_un.S_addr = htonl( INADDR_ANY );

		// Create listening socket
	s = socket(AF_INET, SOCK_DGRAM, 0);
	//send_s = socket(AF_INET, SOCK_DGRAM, 0);
	//setsockopt(s,IPPROTO_TCP,TCP_NODELAY,0,0);
	if (s==INVALID_SOCKET) {
		//cerr << "Create socket failed" << endl;
		return false;
	} else if (bind(s, (sockaddr *)&peer, sizeof(peer))==SOCKET_ERROR) {
		//cerr << "Bind failed with " << WSAGetLastError()  << endl;
		return false;
	}

	//peer.sin_port = htons(27900);
	//bind(send_s, (sockaddr *)&peer, sizeof(peer));
	return true;
}

void Udp_listener::start()
{
	int result = SetThreadAffinityMask(thread,2);
	thread = (HANDLE)_beginthreadex(NULL,0,threadFunc,this,0,NULL);
}

bool Udp_listener::check_address(sockaddr address)
{
	return false;
}

void Udp_listener::udp_listen()
{
	sockaddr_in client_addr;
	int size = sizeof(client_addr);

	char buffer[512];
	int result = recvfrom(s, buffer, sizeof(buffer), 0,(sockaddr*)&client_addr,&size);
	string str = string(buffer);
	string target = "hi";
	char *ip = inet_ntoa(client_addr.sin_addr);
	if(str.find(target) != string::npos) //recieve broadcast message
	{
		cout<<"recieved broadcast message from "<<ip<<endl;
		sockaddr_in* client_detail = (sockaddr_in*)&client_addr;
		bool new_comer = true;
		for(int i = 0; i < workers->size(); i++)
		{
			if(workers->at(i)->isaddr(client_addr))
			{
				cout<<"broadcast was from known client, assigned to worker#"<<i<<endl;
				workers->at(i)->handle_cmd("hi");
				new_comer = false;
				break;
			}
		}
		if(new_comer)
		{
			cout<<"new client"<<endl;
			stringstream ss;
			ss.str(str.substr(3));
			int port_num;
			ss>>port_num;
			client_addr.sin_port = htons(port_num);
			cout<<"client requested to communicated via port "<<port_num<<endl;
			workers->push_back(new udp_communicater(connection_num,client_addr));
			workers->back()->start();
			cout<<"created worker #"<<connection_num<<" for communication with new client"<<endl;
			connection_num++;
		}
		//accepted = true;
	}
	else if(str.find("view") != string::npos)
	{
		for(int i = 0; i < workers->size(); i++)
		{
			if(workers->at(i)->isaddr(client_addr))
			{
				workers->at(i)->setview(str);
				break;
			}
		}
	}
	else if(str.find("chng") != string::npos)
	{
		for(int i = 0; i < workers->size();i++)
		{
			if(workers->at(i)->isaddr(client_addr))
			{
				workers->at(i)->recv_changes(str);
			}
		}
	}
	else if(str == "reset")
	{
		cout<<"client from "<<ip<<" requested reset"<<endl;
		simulator->set_reset();
	}
	else if(str.find("ab") != string::npos)
	{
		
		stringstream ss;
		ss.str(str.substr(3));
		float x,y;
		ss>>x>>y;
		cout<<"client from "<<ip<<" requested new ball at "<<x<<","<<y<<endl;
		simulator->add_circle(x,y);
	}
	else if(str.find("ar") != string::npos)
	{
		stringstream ss;
		ss.str(str.substr(3));
		float x,y;
		ss>>x>>y;
		cout<<"client from "<<ip<<" requested airjet blasting from point "<<x<<","<<y<<endl;
		simulator->airjet(x,y);
	}
	else if(str == "rl")
	{

		//sockaddr_in* client_detail = (sockaddr_in*)&client_addr;
		cout<<"client from "<<ip<<" request lines' position"<<endl;
		for(int i = 0; i < workers->size(); i++)
		{
			if(workers->at(i)->isaddr(client_addr))
			{
				workers->at(i)->set_ready(false);
				workers->at(i)->send_l_size(l_size,l_data);
				break;
			}
		}
	}
	else if(str == "rg")
	{
		//sockaddr_in* client_detail = (sockaddr_in*)&client_addr;
		cout<<"client from "<<ip<<" request grid positions"<<endl;
		for(int i = 0; i < workers->size(); i++)
		{
			if(workers->at(i)->isaddr(client_addr))
			{
				workers->at(i)->set_ready(false);
				workers->at(i)->send_g_size(g_size,g_data);
				break;
			}
		}
	}
	else if(str == "rope")
	{
		cout<<"client from "<<ip<<" toggled rope"<<endl;
		simulator->toggle_rope();
		
	}
	else if(str == "pause")
	{
		simulator->toggle_paused();
	}
	else if(str == "ready_l")
	{
		sockaddr_in* client_detail = (sockaddr_in*)&client_addr;
		for(int i = 0; i < workers->size(); i++)
		{
			if(workers->at(i)->isaddr(client_addr))
			{
				workers->at(i)->sendlines(l_data);
				break;
			}
		}
	}
	else if(str == "ready_g")
	{
		sockaddr_in* client_detail = (sockaddr_in*)&client_addr;
		for(int i = 0; i < workers->size(); i++)
		{
			if(workers->at(i)->isaddr(client_addr))
			{
				workers->at(i)->sendgrid(g_data);
				break;
			}
		}
	}
	else if(str == "done")
	{
		sockaddr_in* client_detail = (sockaddr_in*)&client_addr;
		cout<<"client from "<<ip<<" ready for balls and qtree data stream"<<endl;
		for(int i = 0; i < workers->size(); i++)
		{
			if(workers->at(i)->isaddr(client_addr))
			{
				workers->at(i)->set_ready(true);
				break;
			}
		}
	}
	else
	{
		sockaddr_in* client_detail = (sockaddr_in*)&client_addr;
		for(int i = 0; i < workers->size(); i++)
		{
			if(workers->at(i)->isaddr(client_addr))
			{
				workers->at(i)->handle_cmd(str);
				break;
			}
		}
	}

}

bool Udp_listener::close()
{
	// Cleanup windows sockets
	closesocket(s);
	//WSACleanup();
	return true;
}