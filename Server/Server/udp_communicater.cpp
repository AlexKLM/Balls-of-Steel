#include "udp_communicater.h"


udp_communicater::udp_communicater(int _id,sockaddr_in address)
{
	//addr = address;
	phy_change = 0;
	net_change = 0;
	send_ready = false;
	id = _id;
	intiaited = false;
	client_view = view();
//	b_pack = _pack;
	udp = socket(AF_INET, SOCK_DGRAM, 0);
	//send_ready_b = false;
	addr_detail = address;
	//addr_detail.sin_port = htons(27900);
	sendto(udp,"ok",3,0,(sockaddr *)&addr_detail,sizeof(addr_detail));
}

udp_communicater::~udp_communicater(void)
{
}

bool udp_communicater::disconnect()
{
	closesocket(udp);
	return true;
}

bool udp_communicater::isaddr(sockaddr_in input)
{
	if(addr_detail.sin_addr.S_un.S_addr == input.sin_addr.S_un.S_addr)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void udp_communicater::start()
{
	thread = (HANDLE)_beginthreadex(NULL,0,threadFunc,this,0,NULL);
	int result = SetThreadAffinityMask(thread,2); //core 2 = 2, core 3 = 4????
	intiaited = true;
}

int udp_communicater::get_net_change()
{
	return net_change;
}

int udp_communicater::get_phy_change()
{
	return phy_change;
}

void udp_communicater::recv_changes(string input)
{
	stringstream ss;
	ss.str(input.substr(4));
	ss>>phy_change>>net_change;
}

void udp_communicater::reset_change()
{
	net_change = 0;
	phy_change = 0;
}

void udp_communicater::send_b_size(int size,string data)
{
	//int size = b_pack->size();
	ball_data = data;
	ostringstream convert;
	convert <<"b";
	convert << size; 
	convert << " ";
	convert << ball_data.length();

	string result = convert.str();
	if (sendto(udp, (char*)result.c_str(),result.length()+1, 0,(sockaddr *)&addr_detail,sizeof(addr_detail))==SOCKET_ERROR) {
		cerr << "Send failed with " << WSAGetLastError() << endl;
	}
}

void udp_communicater::send_r_size(int size, string data)
{
	rope_data = data;
	ostringstream convert;
	convert <<"r";
	convert << size; 
	convert << " ";
	convert << rope_data.length();

	string result = convert.str();
	if (sendto(udp, (char*)result.c_str(),result.length()+1, 0,(sockaddr *)&addr_detail,sizeof(addr_detail))==SOCKET_ERROR) {
		cerr << "Send failed with " << WSAGetLastError() << endl;
	}
}

void udp_communicater::send_l_size(int size, string data)
{
	ostringstream convert;
	convert <<"l";
	convert << size; 
	convert << " ";
	convert << data.length();

	string result = convert.str();
	if (sendto(udp, (char*)result.c_str(),result.length()+1, 0,(sockaddr *)&addr_detail,sizeof(addr_detail))==SOCKET_ERROR) {
		cerr << "Send failed with " << WSAGetLastError() << endl;
	}
}

void udp_communicater::send_status_size(string data)
{
	ostringstream convert;
	status_data = data;
	convert<<"s";
	convert<<data.length();
	string result = convert.str();
	if (sendto(udp, (char*)result.c_str(),result.length()+1, 0,(sockaddr *)&addr_detail,sizeof(addr_detail))==SOCKET_ERROR) {
		cerr << "Send failed with " << WSAGetLastError() << endl;
	}
}

void udp_communicater::send_qt_size(int size, string data)
{
	ostringstream convert;
	qt_data = data;
	convert <<"q";
	convert << size; 
	convert << " ";
	convert << data.length();

	string result = convert.str();
	if (sendto(udp, (char*)result.c_str(),result.length()+1, 0,(sockaddr *)&addr_detail,sizeof(addr_detail))==SOCKET_ERROR) {
		cerr << "Send failed with " << WSAGetLastError() << endl;
	}
}

void udp_communicater::send_g_size(int size, string data)
{
	ostringstream convert;
	convert <<"g";
	convert << 25; 
	convert << " ";
	convert << data.length();

	string result = convert.str();
	if (sendto(udp, (char*)result.c_str(),result.length()+1, 0,(sockaddr *)&addr_detail,sizeof(addr_detail))==SOCKET_ERROR) {
		cerr << "Send failed with " << WSAGetLastError() << endl;
	}
}

void udp_communicater::send_view_size(int size, string data)
{
	ostringstream convert;
	view_data = data;
	convert <<"v";
	convert << size; 
	convert << " ";
	convert << data.length();
	string result = convert.str();
	if (sendto(udp, (char*)result.c_str(),result.length()+1, 0,(sockaddr *)&addr_detail,sizeof(addr_detail))==SOCKET_ERROR) {
		cerr << "Send failed with " << WSAGetLastError() << endl;
	}

}

void udp_communicater::set_ready(bool input)
{
	send_ready = input;
}

bool udp_communicater::isready()
{
	return send_ready;
}

void udp_communicater::setview(string data)
{
	stringstream ss;
	ss.str(data.substr(4));
	ss>>client_view.camx>>client_view.camy>>client_view.sizex>>client_view.sizey;
}

view udp_communicater::getview()
{
	return client_view;
}

void udp_communicater::request_view()
{
	sendto(udp,"view",5,0,(sockaddr *)&addr_detail,sizeof(addr_detail));
}

void udp_communicater::request_change()
{
	sendto(udp,"change",7,0,(sockaddr *)&addr_detail,sizeof(addr_detail));
}

void udp_communicater::handle_cmd(string cmd)
{
	if(cmd == "ready_b")
	{
		sendballs();
	}
	else if(cmd == "rb") //DEBUG ONLY, server should be the one sending these out
	{
		//send_b_size();
	}
	else if(cmd == "ready_r")
	{
		sendropes();
	}
	else if(cmd == "ready")
	{
		sendballs();
	}
	else if(cmd == "ready_s")
	{
		sendstatus();
	}
	else if(cmd == "ready_qt")
	{
		sendqt();
	}
	else if(cmd == "ready_v")
	{
		sendview();
	}
	else if(cmd == "hi")
	{
		sendto(udp,"ok",3,0,(sockaddr *)&addr_detail,sizeof(addr_detail));
	}
}

void udp_communicater::sendlines(string l_data)
{
	sendto(udp, l_data.c_str(),l_data.length()+1,0,(sockaddr *)&addr_detail,sizeof(addr_detail));
}

void udp_communicater::sendstatus()
{
	sendto(udp, status_data.c_str(),status_data.length()+1,0,(sockaddr *)&addr_detail,sizeof(addr_detail));
}

void udp_communicater::sendgrid(string g_data)
{
	sendto(udp, g_data.c_str(),g_data.length()+1,0,(sockaddr *)&addr_detail,sizeof(addr_detail));
}

void udp_communicater::sendview()
{
	sendto(udp, view_data.c_str(),view_data.length()+1,0,(sockaddr *)&addr_detail,sizeof(addr_detail));
}

void udp_communicater::sendqt()
{
	sendto(udp, qt_data.c_str(),qt_data.length()+1,0,(sockaddr *)&addr_detail,sizeof(addr_detail));
}

void udp_communicater::sendropes()
{
	sendto(udp,rope_data.c_str(),rope_data.length()+1,0,(sockaddr *)&addr_detail,sizeof(addr_detail));
}

void udp_communicater::kill()
{
	CloseHandle(thread);
	thread = 0;
}

int udp_communicater::run()
{
	bool connected = true;
	while(connected)
	{
		Sleep(10);
		/*if(send_ready_b)
		{
			sendballs();
		}*/
	}
	return 0;
}

void udp_communicater::sendballs()
{
	char *buffer = (char*)malloc(1024);
	string output;
	//stringstream ss;

	sendto(udp, ball_data.c_str(),ball_data.length()+1,0,(sockaddr *)&addr_detail,sizeof(addr_detail));
	
}


