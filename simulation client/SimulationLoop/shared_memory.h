#pragma once
#include <vector>
#include <process.h>
#include"shared_struct.h"
#include <Windows.h>
using namespace std;
class shared_memory
{
private:
	std::vector<balls_packet>* Ballpackets;
	std::vector<line_packet>* Linepackets;
	std::vector<grid_type>* Gridpackets;
	std::vector<qt_packet>* QTpackets;
	std::vector<rope_packet>* Rpackets;
	std::vector<view>* views;
	view* current_view;
	status* con_state;
	bool rope;
	string connection_state;
	int phy_change;
	int net_change;
	float airjet_x;//only for rendering
	float airjet_y;//only for rendering
	bool airjet;//only for rendering
	HANDLE _mutex;
	bool lines;
	bool grid;
public:

	bool Acquire() const {
		return (WaitForSingleObject(_mutex,500L) == WAIT_OBJECT_0);
	}

	void Release() const {
		ReleaseMutex(_mutex);
	}
	view* get_current_v();
	status* get_status();
	void set_con(string input);
	string get_con();
	void setrope(bool input);
	bool hasrope();

	int get_phy_change();
	int get_net_change();

	void reset_change();

	void change_phy(int input);
	void change_net(int input);


	float get_air_x();
	float get_air_y();
	void set_airpos(float x, float y);
	void set_airject(bool input);
	bool have_airjet();
	bool have_lines();
	bool have_grid();
	void set_havelines(bool input);
	void set_havegrids(bool input);
	std::vector<view>* getViews();
	std::vector<rope_packet>* getRData();
	std::vector<qt_packet>* getQTData();
	std::vector<line_packet>* getLineData(); //for both read and write
	std::vector<balls_packet>* getBallData(); //for both read and write
	std::vector<grid_type>* getGridData();
	shared_memory(void);
	~shared_memory(void);
};

