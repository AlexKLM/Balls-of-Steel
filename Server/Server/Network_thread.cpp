#include "Network_thread.h"

Network_thread::Network_thread(void)
{

}

Network_thread::Network_thread(Game* _game,CRITICAL_SECTION* c_lock)
{
	cs = c_lock;
	last_n = 0;
	act_net_s = 0;
	states = status();
	views = std::vector<view>();
	simulator = _game;
	workers =  std::vector<udp_communicater*>();
	current_time = 0;
	nettime = 100;
}

Network_thread::~Network_thread(void)
{
}

bool Network_thread::check_dt()
{
	const float time = Get_time();
	float dt = (time - current_time)*1000;
	float value = 1000/(float)nettime;
	if(dt >value) //the 100 should be dynamic
	{
		current_time = time;
		return true;
	}
	else
	{
		return false;
	}
}

void Network_thread::adjust_nettime(int input)
{
	nettime += input;
	if(nettime <=0)
	{
		nettime = 1;
	}
}

float Network_thread::Get_time()
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

void Network_thread::start()
{
	//level_data = simulator->get_level_data();
	//level_size = simulator->get_level_size();
	thread = (HANDLE)_beginthreadex(NULL,0,threadFunc,this,0,NULL);
	int result = SetThreadAffinityMask(thread,2); //core 2 = 2, core 3 = 4????
	//intiaited = true;
}

void Network_thread::update_QT()
{
	Q_tree_data = simulator->get_q_T();
}

void Network_thread::update_physics_state()
{
	
	concurrent_vector<Circle>* Circlelist = simulator->get_balls();
	stringstream ss;
	ball_data = "";
	size = Circlelist->size();
	for(int i = 0; i < size;i++)
	{
		ss<< Circlelist->at(i).GetPos().GetX();
		ball_data.append(ss.str());
		ball_data.append(" ");
		ss.str("");

		ss<< Circlelist->at(i).GetPos().GetY();
		ball_data.append(ss.str());
		ball_data.append(" ");
		ss.str("");

		ss<< Circlelist->at(i).GetAngle();
		ball_data.append(ss.str());
		ball_data.append(" ");
		ss.str("");

		ss<< Circlelist->at(i).is_stationary();
		ball_data.append(ss.str());
		ball_data.append(" ");
		ss.str("");

		ss<< Circlelist->at(i).GetMass();
		ball_data.append(ss.str());
		ball_data.append(" ");
		ss.str("");
	}

}

void Network_thread::update_rope_state()
{
	concurrent_vector<RopePts>* Ropelist = simulator->get_rope();
	stringstream ss;
	rope_data = "";
	ropesize = Ropelist->size();
	for(int i =0; i < ropesize;i++)
	{
		ss<<Ropelist->at(i).GetPos().GetX();
		rope_data.append(ss.str());
		rope_data.append(" ");
		ss.str("");

		ss<<Ropelist->at(i).GetPos().GetY();
		rope_data.append(ss.str());
		rope_data.append(" ");
		ss.str("");

		ss<<Ropelist->at(i).Getradius();
		rope_data.append(ss.str());
		rope_data.append(" ");
		ss.str("");

	}
}

void Network_thread::update_status()
{
	//states.p_per_s = simulator->get_pps();
	//states.dp_per_s = simulator->get_apps();
	//states.last_air = simulator->get_lastairjet();

	status_data = "";
	stringstream ss;
	ss<<simulator->get_pps();
	ss<<" ";
	ss<<simulator->get_apps();
	ss<<" ";
	ss<<nettime;
	ss<<" ";
	ss<<act_net_s;
	ss<<" ";
	ss<<size;
	ss<<" ";
	ss<<simulator->get_lastairjet();
	ss<<" ";
	ss<<simulator->get_fric();
	ss<<" ";
	ss<<simulator->get_elasticity_l();
	ss<<" ";
	ss<<simulator->get_elasticity_b();

	status_data.append(ss.str());
}

void Network_thread::update_views()
{
	views.clear();
	view v;
	view_data = "";
	stringstream ss;
	for(int i = 0; i < workers.size();i++)
	{
		v = workers[i]->getview();
		ss<<v.camx;
		ss<<" ";
		ss<<v.camy;
		ss<<" ";
		ss<<v.sizex;
		ss<<" ";
		ss<<v.sizey;
		ss<<" ";
		view_data.append(ss.str());
		ss.str("");
		//views.push_back(workers[i]->getview());
	}
	
	//view_data
}

void Network_thread::update_changes()
{
	int net_change = 0, phy_change = 0;
	for(int i = 0; i < workers.size(); i ++)
	{
		net_change += workers[i]->get_net_change();
		phy_change += workers[i]->get_phy_change();
		workers[i]->reset_change();
	}
	adjust_nettime(net_change);
	simulator->set_pps(phy_change);
}

int Network_thread::run()
{
	listener = Udp_listener();
	listener.setup(simulator,&workers,simulator->get_level_size(),simulator->get_grid_size(),simulator->get_level_data(),simulator->get_grid());
	listener.start();
	cout<<"server is up"<<endl;
	while(true)
	{
		if(check_dt())
		{
			float current_n = Get_time();
			act_net_s = 1000/((current_n - last_n)*1000);
			last_n = current_n;

			EnterCriticalSection(cs);

			update_changes();
			update_status();
			update_physics_state();
			update_views();
			int qtsize = simulator->get_qt_size();
			update_QT();

			LeaveCriticalSection(cs);
	

			Sleep(1);
			if(simulator->hasrope())
			{
				update_rope_state();
				for(int i = 0; i < workers.size();i++)
				{
					if(workers[i]->isready())
					{
						workers[i]->send_r_size(ropesize,rope_data);
					}
				}
			}
			else
			{
				for(int i = 0; i < workers.size();i++)
				{
					if(workers[i]->isready())
					{
						workers[i]->send_r_size(0,"none");
					}
				}
			}

			Sleep(1);
			for(int i = 0; i < workers.size();i++)
			{
				if(workers[i]->isready())
				{
					workers[i]->send_b_size(size,ball_data);
					
				}
				
			}

			Sleep(1);
			for(int i = 0; i < workers.size();i++)
			{
				if(workers[i]->isready())
				{
					workers[i]->send_qt_size(qtsize,Q_tree_data);
				}
			}
			
			Sleep(1);
			for(int i = 0; i < workers.size(); i++)
			{
				if(workers[i]->isready())
				{
					workers[i]->send_status_size(status_data);
					//send status
				}
			}

			Sleep(1);
			for(int i = 0; i < workers.size(); i++)
			{
				if(workers[i]->isready())
				{
					workers[i]->send_status_size(status_data);
					//send status
				}
			}
			
			Sleep(1);
			for(int i = 0; i < workers.size(); i++)
			{
				if(workers[i]->isready())
				{
					workers[i]->send_view_size(workers.size(),view_data);
					//send status
				}
			}

			Sleep(1);
			for(int i = 0; i < workers.size(); i++)
			{
				if(workers[i]->isready())
				{
					workers[i]->request_view();
					//send status
				}
			}

			Sleep(2);
			for(int i = 0; i < workers.size(); i++)
			{
				if(workers[i]->isready())
				{
					workers[i]->request_change();
					//send status
				}
			}

		}
	}
	return 0;
}


