#include "shared_memory.h"


shared_memory::shared_memory(void)
{
	phy_change = 0;
	net_change = 0;
	airjet = false;
	airjet_x = 0;
	rope = false;
	airjet_y = 0;
	lines = false;
	grid = false;
	connection_state = "";
	con_state = new status();
	views = new std::vector<view>();
	Ballpackets = new std::vector<balls_packet>();
	Linepackets = new std::vector<line_packet>();
	Gridpackets = new std::vector<grid_type>();
	QTpackets = new std::vector<qt_packet>();
	Rpackets = new std::vector<rope_packet>();
	current_view = new view();
	_mutex=CreateMutexA(NULL,FALSE,"myMutex");
}

shared_memory::~shared_memory(void)
{
}

void shared_memory::reset_change()
{
	net_change = 0;
	phy_change = 0;
}

int shared_memory::get_net_change()
{
	return net_change;
}

int shared_memory::get_phy_change()
{
	return phy_change;
}

view* shared_memory::get_current_v()
{
	return current_view;
}

std::vector<view>* shared_memory::getViews()
{
	return views;
}

std::vector<balls_packet>* shared_memory::getBallData()
{
	return Ballpackets;
}

std::vector<line_packet>* shared_memory::getLineData()
{
	return Linepackets;
}

std::vector<grid_type>* shared_memory::getGridData()
{
	return Gridpackets;
}

std::vector<qt_packet>* shared_memory::getQTData()
{
	return QTpackets;
}

std::vector<rope_packet>* shared_memory::getRData()
{
	return Rpackets;
}

status* shared_memory::get_status()
{
	return con_state;
}

void shared_memory::change_net(int input)
{
	net_change += input;
}

void shared_memory::change_phy(int input)
{
	phy_change +=input;
}

void shared_memory::set_con(string input)
{
	connection_state = input;
}

string shared_memory::get_con()
{
	return connection_state;
}

bool shared_memory::hasrope()
{
	return rope;
}

bool shared_memory::have_grid()
{
	return grid;
}

bool shared_memory::have_lines()
{
	return lines;
}

void shared_memory::set_havegrids(bool input)
{
	grid = input;
}

void shared_memory::set_havelines(bool input)
{
	lines = input;
}

void shared_memory::setrope(bool input)
{
	rope = input;
}

void shared_memory::set_airpos(float x, float y)
{
	airjet_x = x;
	airjet_y = y;
}

float shared_memory::get_air_x()
{
	return airjet_x;
}

float shared_memory::get_air_y()
{
	return airjet_y;
}

void shared_memory::set_airject(bool input)
{
	airjet = input;
}

bool shared_memory::have_airjet()
{
	return airjet;
}
