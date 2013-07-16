#include "UDP_recv.h"
#include "networking.h"

UDP_recv::UDP_recv(void)
{
	running = false;
}

UDP_recv::UDP_recv(shared_memory *shared,UDP_sender* send_thread,networking* net_thread)
{
	running = false;
	network_ctr = net_thread;
	sender = send_thread;
	shared_data = shared;
	current_time = 0;
	accumlator = 0;
	dt = 0.0166;
	connected = false;
	nettime = 100;
}

void UDP_recv::start()
{
	thread = (HANDLE)_beginthreadex(NULL,0,threadFunc,this,0,NULL);
	SetThreadAffinityMask(thread,2);//core 2 = 2, core 3 = 4????
}


UDP_recv::~UDP_recv(void)
{
}

bool UDP_recv::setup()
{
	int timeout = 100;
	// Create socket data space
	//sockaddr_in peer;
	//broadcast_addr.sin_family = AF_INET;
	//broadcast_addr.sin_port = htons(7777);	// port 9171
	//broadcast_addr.sin_addr.S_un.S_addr = INADDR_BROADCAST; !

	sockaddr_in udp_addr;
	udp_addr.sin_family = AF_INET;
	udp_addr.sin_port = htons(9173);	// port 9171
	udp_addr.sin_addr.S_un.S_addr = htonl( INADDR_ANY );

	// Create transfer socket
	//char buffer;
	//sender = socket(AF_INET, SOCK_DGRAM, 0);
	reciever = socket(AF_INET,SOCK_DGRAM,0);
	
	if (bind(reciever, (sockaddr *)&udp_addr, sizeof(udp_addr))==SOCKET_ERROR)
	{
		int result = WSAGetLastError();
		return 0;
	}
	running = true;
	//setsockopt(broadcaster,SOL_SOCKET,SO_BROADCAST,&optval,optlen);
}

int UDP_recv::run()
{
	//running = true;
	//bool connected = false;
	
	while(true)
	{
		if(running)//socket is up
		{
			if(!connected)
			{
				wait_for_server();
			}
			else
			{
				idle();
			}
		}
	}
	
	return 0;
}

void UDP_recv::wait_for_server()
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
	sockaddr_in server_addr;
	int size = sizeof(server_addr);
	int result = recvfrom(reciever,buffer,sizeof(buffer),0,(sockaddr *)&server_addr,&size);
	result = WSAGetLastError();
	string str = string(buffer);
	if(str == "ok")
	{
		server_addr.sin_port = htons(9172);
		network_ctr->set_waiting(false);
		sender->setup(server_addr);
		connected = true;
		network_ctr->update_lastsucess();
		if(!shared_data->have_lines())
		{
			sender->send("rl");
		}
		else
		{
			sender->send("done");
		}
		//cout<<"client connected!"<<endl;
	}	
	
}

void UDP_recv::disconnect()
{
	/*if(shutdown(s,SD_SEND) == SOCKET_ERROR)
	{
		cerr<<"shutdown failed!"<<WSAGetLastError<<endl;
	}*/
	//cout<<"client disconnected"<<endl;
	closesocket(reciever);
	connected = false;
	running = false;
	//WSACleanup();
}

float UDP_recv::Get_time()
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

void UDP_recv::recv_balls(string input)
{
	stringstream ss;
	ss.str(input.substr(1));

	int size;
	int msglength;
	ss>>size>>msglength;
	//cout<<size<<endl;
	sender->send("ready");
	//sendto(sender, "ready", 6, 0,(sockaddr *)&server_addr,sizeof(server_addr));
	//balls_packet pack;
	char data [65507];//max buffer size
	int datasize = sizeof(data);
	int result = recvfrom(reciever, data, sizeof(data), 0,0,0);

	string str = string(data);
	if(str.length() == msglength) //skip a cycle if message didn't come through
	{
		translate_balls(str,size);
		update_balls();
		network_ctr->update_lastsucess();
	}

}

void UDP_recv::recv_ropes(string input)
{
	stringstream ss;
	ss.str(input.substr(1));

	int size;
	int msglength;
	ss>>size>>msglength;
	if(size == 0)
	{
		shared_data->setrope(false);
		return;
	}
	//cout<<size<<endl;
	sender->send("ready_r");
	//sendto(sender, "ready", 6, 0,(sockaddr *)&server_addr,sizeof(server_addr));
	//balls_packet pack;
	char data [65507];//max buffer size
	shared_data->setrope(true);
	int result = recvfrom(reciever, data, sizeof(data), 0,0,0);

	string str = string(data);
	if(str.length() == msglength) //skip a cycle if message didn't come through
	{
		translate_rope(str,size);
		network_ctr->update_lastsucess();
	}
}

void UDP_recv::recv_qt(string input)
{
	stringstream ss;
	ss.str(input.substr(1));

	int size;
	int msglength;
	ss>>size>>msglength;
	//cout<<size<<endl;
	sender->send("ready_qt");
	//sendto(sender, "ready", 6, 0,(sockaddr *)&server_addr,sizeof(server_addr));
	//balls_packet pack;
	char data [65507];//max buffer size

	int result = recvfrom(reciever, data, sizeof(data), 0,0,0);

	string str = string(data);
	if(str.length() == msglength) //skip a cycle if message didn't come through
	{
		translate_qt(str,size);
		network_ctr->update_lastsucess();
	}
}

void UDP_recv::recv_grid(string input)
{
	stringstream ss;
	ss.str(input.substr(1));

	int size;
	int msglength;
	ss>>size>>msglength;
	//cout<<size<<endl;
	sender->send("ready_g");
	char data [65507];//max buffer size
	int result = recvfrom(reciever, data, sizeof(data), 0,0,0);

	string str = string(data);
	if(str.length() == msglength) //skip a cycle if message didn't come through
	{
		translate_grid(str,size);
		network_ctr->update_lastsucess();
	}
	else
	{
		sender->send("rg");
	}
}

void UDP_recv::recv_lines(string input)
{
	
	stringstream ss;
	ss.str(input.substr(1));

	int size;
	int msglength;
	ss>>size>>msglength;
	//cout<<size<<endl;
	sender->send("ready_l");
	char data [65507];//max buffer size
	int result = recvfrom(reciever, data, sizeof(data), 0,0,0);

	string str = string(data);
	if(str.length() == msglength) //skip a cycle if message didn't come through
	{
		translate_lines(str,size);
		network_ctr->update_lastsucess();
	}
	else
	{
		sender->send("rl");
	}
}

void UDP_recv::recv_status(string input)
{
	stringstream ss;
	ss.str(input.substr(1));
	int msglength;
	ss>>msglength;
	sender->send("ready_s");
	char data [65507];//max buffer size
	int result = recvfrom(reciever, data, sizeof(data), 0,0,0);
	string str = string(data);
	if(str.length() == msglength) //skip a cycle if message didn't come through
	{
		translate_status(str);
		network_ctr->update_lastsucess();
	}
}

void UDP_recv::recv_views(string input)
{
	stringstream ss;
	ss.str(input.substr(1));

	int size;
	int msglength;
	ss>>size>>msglength;
	sender->send("ready_v");
	char data [65507];//max buffer size
	int result = recvfrom(reciever, data, sizeof(data), 0,0,0);

	string str = string(data);
	if(str.length() == msglength) //skip a cycle if message didn't come through
	{
		translate_views(str,size);
		network_ctr->update_lastsucess();
	}
}
 
bool UDP_recv::idle()
{
	char buffer[1025];
	
	int result;
	result = recvfrom(reciever, buffer, sizeof(buffer), 0,NULL,NULL);
	network_ctr->update_lastsucess();
	
	if(result == SOCKET_ERROR)
	{
		result = WSAGetLastError();
		return false;
	}
	else if(result == 0)
	{
			return false;
			disconnect();
	}
	else
	{
		if(string(buffer) == "view")
		{
			send_view();
		}
		else if(string(buffer) == "change")
		{
			send_changes();
		}
		else //other commands
		{
			if(buffer[0] == 'b')
			{
				recv_balls(string(buffer));
			}
			else if(buffer[0] == 'l')
			{
				recv_lines(string(buffer));
			}
			else if(buffer[0] == 'g')
			{
				recv_grid(string(buffer));
			}
			else if(buffer[0] == 'q')
			{
				recv_qt(string(buffer));
			}
			else if(buffer[0] == 'r')
			{
				recv_ropes(string(buffer));
			}
			else if(buffer[0] == 's')
			{
				recv_status(string(buffer));
			}
			else if(buffer[0] == 'v')
			{
				recv_views(string(buffer));
			}
		}
	}
	
}

void UDP_recv::send_view()
{
	if(shared_data->Acquire())
	{
		view* v = shared_data->get_current_v();
		string message = "";
		stringstream ss;
		ss<<"view ";
		ss<<v->camx;
		ss<<" ";
		ss<<v->camy;
		ss<<" ";
		ss<<v->sizex;
		ss<<" ";
		ss<<v->sizey;
		message.append(ss.str());
		sender->send(message);
					
		shared_data->Release();
	
	}
}

void UDP_recv::send_changes()
{
	string message = "";
	stringstream ss;
	ss<<"chng ";
	ss<<shared_data->get_phy_change();
	ss<<" ";
	ss<<shared_data->get_net_change();
	ss<<" ";
	message.append(ss.str());
	sender->send(message);
	shared_data->reset_change();
}

void UDP_recv::translate_status(string &input)
{
	status *state = shared_data->get_status();
	stringstream ss;
	ss.str(input);
	//desire_phy_ps,act_phy_ps,desire_net_ps,actual_net_ps,total_balls,last_air_force,friction,elasticity_l,elasticity_b
	ss>>state->dp_per_s>>state->p_per_s>>state->dnet_pers>>state->net_per_s>>state->total_b>>state->last_air>>state->fric>>state->elas_l>>state->elas_b;
}

void UDP_recv::translate_qt(string &input, int size)
{
	std::vector<qt_packet> grid = std::vector<qt_packet>();
	stringstream ss;
	ss.str(input);
	qt_packet pack;

	for(int i = 0; i < size; i ++)
	{
		ss>>pack.minx>>pack.miny>>pack.maxx>>pack.maxy;
		grid.push_back(pack);
	}
	update_qt(&grid);
}

void UDP_recv::translate_grid(string &input, int size)
{
	std::vector<grid_type> grid = std::vector<grid_type>();
	stringstream ss;
	ss.str(input);
	grid_type pack;

	for(int i = 0; i < size; i ++)
	{
		ss>>pack.x>>pack.y>>pack.size;
		grid.push_back(pack);
	}
	update_grid(&grid);
}

void UDP_recv::translate_balls(string &input, int size)
{
	stringstream ss;
	ss.str(input);
	balls_packet pack;
	balls.clear();
	for(int i = 0; i < size; i ++)
	{
		ss>>pack.x>>pack.y>>pack.angle>>pack.stationary>>pack.mass;
		balls.push_back(pack);
	}
}

void UDP_recv::translate_rope(string &input, int size)
{
	std::vector<rope_packet> ropes = std::vector<rope_packet>();
	stringstream ss;
	ss.str(input);
	rope_packet pack;
	for(int i = 0; i < size; i ++)
	{
		ss>>pack.x>>pack.y>>pack.radius;
		ropes.push_back(pack);
	}
	update_ropes(&ropes);
}

void UDP_recv::translate_lines(string &input, int size)
{
	std::vector<line_packet> line = std::vector<line_packet>();
	stringstream ss;
	ss.str(input);
	line_packet pack;
	//line.clear();
	for(int i = 0; i < size; i ++)
	{
		ss>>pack.x1>>pack.y1>>pack.x2>>pack.y2;
		line.push_back(pack);
	}
	update_lines(&line);
}

void UDP_recv::translate_views(string &input, int size)
{
	std::vector<view> vs = std::vector<view>();
	stringstream ss;
	ss.str(input);
	view v;
	for(int i = 0; i < size; i++)
	{
		v = view();
		ss>>v.camx>>v.camy>>v.sizex>>v.sizey;
		vs.push_back(v);
	}
	update_views(&vs);
}

void UDP_recv::adjust_nettime(int input)
{
	nettime += input;
	if(nettime <=0)
	{
		nettime = 1;
	}
}

void UDP_recv::update_balls()
{
	if(shared_data->Acquire())
	{
		std::vector<balls_packet>* b = shared_data->getBallData();
		
		
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

void UDP_recv::update_ropes(std::vector<rope_packet>* input)
{
	if(shared_data->Acquire())
	{
		std::vector<rope_packet>* r = shared_data->getRData();
		__try
		{
			r->clear();
			rope_packet data;
			for(int i = 0; i < input->size(); i++)
			{
				data = input->at(i);
				r->push_back(data);
			}
		}
		__finally
		{
			shared_data->Release();
		}
	}
}

void UDP_recv::update_lines(std::vector<line_packet>* input)
{
	std::vector<line_packet>* l = shared_data->getLineData();
	line_packet data;
	for(int i = 0; i < input->size(); i++)
	{
		data = input->at(i);
		l->push_back(data);
	}
	shared_data->set_havelines(true);
	if(!shared_data->have_grid())
	{
		sender->send("rg");
	}
	else
	{
		sender->send("done");
	}
}

void UDP_recv::update_grid(std::vector<grid_type>* input)
{
	std::vector<grid_type>* g = shared_data->getGridData();
	grid_type data;
	for(int i = 0; i < input->size(); i++)
	{
		data = input->at(i);
		g->push_back(data);
	}
	shared_data->set_havegrids(true);
	if(!shared_data->have_lines())
	{
		sender->send("rl");
	}
	else
	{
		sender->send("done");
	}
}

void UDP_recv::update_qt(std::vector<qt_packet>* input)
{
	if(shared_data->Acquire())
	{
		std::vector<qt_packet>* q = shared_data->getQTData();
		__try
		{
			q->clear();
			qt_packet g;
			for(int i = 0; i < input->size(); i++)
			{
				g = input->at(i);
				q->push_back(g);
			}
		}
		__finally
		{
			shared_data->Release();
		}

	}
}

void UDP_recv::update_views(std::vector<view>* input)
{
	if(shared_data->Acquire())
	{
		std::vector<view>* v = shared_data->getViews();
		__try
		{
			v->clear();
			view data;
			for(int i = 0; i < input->size(); i++)
			{
				data = input->at(i);
				v->push_back(data);
			}
		}
		__finally
		{
			shared_data->Release();
		}
	}
}