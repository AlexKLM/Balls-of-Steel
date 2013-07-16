#include <iostream>
#include <winsock2.h>
#include <fstream>
#include "Game.h"
#include "Network_thread.h"
#include <concurrent_vector.h>
// Simulation SERVER
#pragma comment(lib,"ws2_32.lib")
using namespace std;
using namespace Concurrency;

Game *game;
CRITICAL_SECTION cs;
Network_thread network;
config configdata;
void ReadInVars(const std::wstring& FileName)
{
	configdata.friction = 0.05;
	configdata.e_ball = 0.8;
	configdata.e_line = 0.5;
	ifstream fin;
	std::wifstream filein(FileName);
	
	if(filein)
	{
		WCHAR strCommand[256] = {0};
		
		while(filein)
		{
			filein >> strCommand;
			if( 0 == wcscmp( strCommand, L"//" ) )
			{

			}
			else if(0 == wcscmp(strCommand, L"friction"))
			{
				filein>>configdata.friction;
			}
			else if(0 == wcscmp(strCommand, L"ball_e"))
			{
				filein>>configdata.e_ball;
			}
			else if(0 == wcscmp(strCommand, L"line_e"))
			{
				filein>>configdata.e_line;
			}
		}
	}
}

int main(int, char**) {
	configdata = config();
	InitializeCriticalSection(&cs);
	game = new Game(&cs);
	ReadInVars(L"config.txt");
	game->set_fric(configdata.friction);
	game->set_e_ball(configdata.e_ball);
	game->set_e_line(configdata.e_line);
	network = Network_thread(game,&cs);
	game->start();
	/*while(true)
	{
		game->run();
	}*/
	network.start();
	int n; cin >> n;

}