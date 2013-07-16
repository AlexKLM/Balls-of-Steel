#include "Renderer.h"

float Renderer::GetTime()
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

bool Renderer::check_dt()
{
	const float time = GetTime();
	float dt = (time - current_time)*1000;
	if(dt >1000/des_render) //
	{
		current_time = time;
		return true;
	}
	else
	{
		return false;
	}
}

Renderer::Renderer(void)
{

}

Renderer::Renderer(shared_memory* shared,HDC _hdc)
{
	actual_render = 0;
	con = "";
	actual_phy = 0;
	des_phy = 0;
	actual_net = 0;
	des_net = 0;
	pre_render = 0;
	num_balls = 0;
	last_air = 0;
	fric = 0;
	e_b = 0;
	e_l = 0;
	shared_data = shared;
	m_hdc = _hdc;
	current_time = 0;
	accumlator = 0;
	num_r_balls = 0;
	des_render = 60;//preset to 60
	//dt = 0.16;

}

Renderer::~Renderer(void)
{
}

void Renderer::Render()
{
	status *state = shared_data->get_status();
	con = shared_data->get_con();
	actual_phy = state->p_per_s;
	des_phy = state->dp_per_s;
	des_net = state->dnet_pers;
	actual_net = state->net_per_s;
	num_balls = state->total_b;
	last_air = state->last_air;
	fric = state->fric;
	e_b = state->elas_b;
	e_l = state->elas_l;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	render_view();
	if(shared_data->have_airjet())
	{
		RenderAir();
	}
	
	if(shared_data->have_grid())
	{
		RenderGrid();
	}

	if(shared_data->have_lines())
	{
		RenderLines();
	}
	RenderQT();
	if(shared_data->hasrope())
	{
		RenderRope();
	}

	
	RenderBalls();
	TwDraw(); 
	SwapBuffers(m_hdc);	
}


void Renderer::InitializeOpenGL(HWND &hwnd, int width, int height)
{ 
	GLuint		PixelFormat;			// Holds The Results After Searching For A Match
	HGLRC		hRC=NULL;		// Permanent Rendering Context
	

	static	PIXELFORMATDESCRIPTOR pfd=				// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
		1,											// Version Number
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window
		PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,							// Must Support Double Buffering
		PFD_TYPE_RGBA,								// Request An RGBA Format
		24,										// Select Our Color Depth
		0, 0, 0, 0, 0, 0,							// Color Bits Ignored
		0,											// No Alpha Buffer
		0,											// Shift Bit Ignored
		0,											// No Accumulation Buffer
		0, 0, 0, 0,									// Accumulation Bits Ignored
		16,											// 16Bit Z-Buffer (Depth Buffer)  
		0,											// No Stencil Buffer
		0,											// No Auxiliary Buffer
		PFD_MAIN_PLANE,								// Main Drawing Layer
		0,											// Reserved
		0, 0, 0										// Layer Masks Ignored
	};

	if (!(m_hdc=GetDC(hwnd)))							// Did We Get A Device Context?
	{
		MessageBoxA(NULL,"Can't Create A GL Device Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
	}

	if (!(PixelFormat=ChoosePixelFormat(m_hdc,&pfd)))	// Did Windows Find A Matching Pixel Format?
	{
		MessageBoxA(NULL,"Can't Find A Suitable PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
	}

	if(!SetPixelFormat(m_hdc,PixelFormat,&pfd))		// Are We Able To Set The Pixel Format?
	{
		MessageBoxA(NULL,"Can't Set The PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
	}

	if (!(hRC=wglCreateContext(m_hdc)))				// Are We Able To Get A Rendering Context?
	{
		MessageBoxA(NULL,"Can't Create A GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
	}

	if(!wglMakeCurrent(m_hdc,hRC))					// Try To Activate The Rendering Context
	{
		MessageBoxA(NULL,"Can't Activate The GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
	}

	ShowWindow(hwnd,SW_SHOW);						// Show The Window

	SetForegroundWindow(hwnd);						// Slightly Higher Priority
	SetFocus(hwnd);									// Sets Keyboard Focus To The Window
	//glutMouseFunc(Mouse);

	if (height==0)										// Prevent A Divide By Zero By
	{
		height=1;										// Making Height Equal One
	}
	
	//glViewport(0,0,width,height);						// Reset The Current Viewport

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	//glTranslatef(50,50,0);
	glLoadIdentity();									// Reset The Projection Matrix
	float size_x, size_y,zoom;
	zoom = 1;
	size_x = (width/100)*zoom;
	size_y = (height/100)*zoom;
	////view* v = shared.get_current_v();
	//v->camx = 0;
	//v->camy = 0;
	//v->sizex = size_x;
	//v->sizey = size_y;
	glOrtho(-size_x,size_x,-size_y,size_y,0, 10000);	
	W = (float)width;
	H = (float)height;
	// Calculate The Aspect Ratio Of The Window
	//gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);
	//

	//glOrtho(0,0+width,0+height,0,1, -1);
	//glOrtho(-6,6,-6,6,-10, 10);
	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix
	//glTranslatef(0,0,0);
	glPushMatrix();


	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);				// Black Background
	glClearDepth(1.0f);									// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations

	TwInit(TW_OPENGL, NULL);
	TwWindowSize(W, H);
	
}

void Renderer::start()
{
	thread = (HANDLE)_beginthreadex(NULL,0,threadFunc,this,0,NULL);
	SetThreadAffinityMask(thread,2);//core 2 = 2, core 3 = 4????
}

void Renderer::check_render()
{
	if(check_dt())
	{
		float cur_render = GetTime();
		actual_render = 1000/((cur_render - pre_render)*1000);
		pre_render = cur_render;
		Render();
	}
}

void Renderer::modifyRS(int input)
{
	des_render+=input;
	if(des_render <=0)
	{
		des_render = 1;
	}
}

int Renderer::run()
{
	//running = true;
	//while(running)
	//
	//TwAddVarRO(bar, "NumTexUnits", TW_TYPE_UINT32, &NumberOfTexUnits, " label='# of texture units' ");
	Bar = TwNewBar("Client");
	TwDefine("Client position='200 40' ");
	TwAddVarRO(Bar, "con_state", TW_TYPE_STDSTRING, &con, " label='Network status' ");
	TwAddVarRO(Bar, "desiered_FPS", TW_TYPE_UINT16, &des_render, " label='desired fps' ");
	TwAddVarRO(Bar, "R_per_sec", TW_TYPE_UINT16, &actual_render, " label='frames per sec' ");
	TwAddVarRO(Bar, "desiered_phy", TW_TYPE_UINT16, &des_phy, " label='desired Phy' ");
	TwAddVarRO(Bar, "actual_phy", TW_TYPE_UINT16, &actual_phy, " label='Actual Phy' ");
	TwAddVarRO(Bar, "desiered_net", TW_TYPE_UINT16, &des_net, " label='desired Net' ");
	TwAddVarRO(Bar, "actual_net", TW_TYPE_UINT16, &actual_net, " label='Actual Net' ");
	TwAddVarRO(Bar, "server_balls", TW_TYPE_UINT16, &num_balls, " label='Server Objs' ");
	TwAddVarRO(Bar, "Render_balls", TW_TYPE_UINT16, &num_balls, " label='Render Objs' ");
	TwAddVarRO(Bar, "last_air", TW_TYPE_FLOAT, &last_air, " label='last airjet' ");
	TwAddVarRO(Bar, "fric", TW_TYPE_FLOAT, &fric, " label='friction' ");
	TwAddVarRO(Bar, "e_b", TW_TYPE_FLOAT, &e_b, " label='ball elasticity' ");
	TwAddVarRO(Bar, "e_l", TW_TYPE_FLOAT, &e_l, " label='line elasticity' ");

	while(true)
	{
		Sleep(10);
	}
		
	//}
	return 0;
}

void Renderer::render_view()
{
	if(shared_data->Acquire())
	{
		std::vector<view>* v = shared_data->getViews();
		__try
		{
			//glMatrixMode( GL_MODELVIEW );
			//glPopMatrix();
			//glLoadIdentity();
			for(int i = 0; i < v->size(); i++)
			{
				glPushMatrix();
				glTranslatef(-v->at(i).camx, -v->at(i).camy, 0 );
				glBegin(GL_LINE_LOOP );
				glColor3d(1, 0, 1);

				glVertex2d((-v->at(i).sizex),(-v->at(i).sizey));
				glVertex2d((v->at(i).sizex),(-v->at(i).sizey));
				glVertex2d((v->at(i).sizex),(v->at(i).sizey));
				glVertex2d((-v->at(i).sizex),(v->at(i).sizey));
				glEnd();
				glPopMatrix();
			}
			//glPushMatrix();
		}
		__finally
		{
			shared_data->Release();
		}
	}
}

void Renderer::RenderRope()
{
	if(shared_data->Acquire())
	{
		std::vector<rope_packet>* ropes = shared_data->getRData();
		__try
		{
			for(int i = 0; i < ropes->size(); i++)
			{
				glPushMatrix();

				glTranslatef(ropes->at(i).x, ropes->at(i).y, 0);
				glBegin(GL_LINE_LOOP);

				glColor3d(0.5,0.5,0.5);
				
				
				glVertex2d(0,0);
				for(float angle = 0.0f; angle < 2*M_PI; angle += 0.1f)
				{
					glVertex2d(sin(angle)*ropes->at(i).radius,  cos(angle)*ropes->at(i).radius);
				}
				
				//glVertex2d(sin(0.0f)*m_radius,  cos(0.0f)*m_radius);
				glEnd();
				glPopMatrix();
			}
			for(int i = 0; i < ropes->size()-1;i++)
			{
				glPushMatrix();

				
				glBegin(GL_LINE_LOOP);

				glColor3d(1,1,1);
				
				
				glVertex2d(ropes->at(i).x,ropes->at(i).y);
				glVertex2d(ropes->at(i+1).x,ropes->at(i+1).y);
				
				//glVertex2d(sin(0.0f)*m_radius,  cos(0.0f)*m_radius);
				glEnd();
				glPopMatrix();
			}
		}
		__finally
		{
			shared_data->Release();
		}
	}
}

void Renderer::RenderAir()
{
	glBegin(GL_LINES);
	if(shared_data->get_air_y() < -5)
	{
		glColor3d(1,1, 1);
	}
	else
	{
		glColor3d(1,0,0);
	}

	glVertex2d(0,-5);
	glVertex2d(shared_data->get_air_x(),shared_data->get_air_y());
	glEnd();
}

void Renderer::RenderLines()
{
	std::vector<line_packet>* lines = shared_data->getLineData();
	int size = lines->size();
	if(shared_data->hasrope())
	{
		size -=1;
	}
	for(int i = 0; i < size;i++)
	{

		glBegin(GL_LINES );
		glColor3d(1, 1, 1);
		glVertex2d(lines->at(i).x1,lines->at(i).y1);
		glVertex2d(lines->at(i).x2,lines->at(i).y2);
		glEnd();

	}
}

void Renderer::RenderGrid()
{
	std::vector<grid_type>* grid = shared_data->getGridData();
	int x, y, size;
	for(int i = 0; i < grid->size();i++)
	{
		x = grid->at(i).x;
		y = grid->at(i).y;
		size = grid->at(i).size;
		glBegin(GL_LINE_LOOP );
		glColor3d(0.2f, 0.2f, 0.2f);
		glVertex2d(x,y);
		glVertex2d(x+size,y);
		glVertex2d(x+size,y+size);
		glVertex2d(x,y+size);
		glVertex2d(x,y);
		glEnd();
	}
	
}

void Renderer::RenderQT()
{
		if(shared_data->Acquire())
	{
		std::vector<qt_packet>* grid = shared_data->getQTData();
		__try
		{
			int x, y, size;
			for(int i = 0; i < grid->size();i++)
			{
				glBegin(GL_LINE_LOOP );
				glColor3d(1, 0, 0);
				glVertex2d(grid->at(i).minx,grid->at(i).miny);
				glVertex2d(grid->at(i).minx,grid->at(i).maxy);
				glVertex2d(grid->at(i).maxx,grid->at(i).maxy);
				glVertex2d(grid->at(i).maxx,grid->at(i).miny);
				glEnd();
			}
		}
		__finally
		{
			shared_data->Release();
		}
	}
}

void Renderer::RenderBalls()
{
	if(shared_data->Acquire())
	{
		std::vector<balls_packet>* balls = shared_data->getBallData();
		__try
		{
			num_r_balls = balls->size();
			for(int i = 0; i < balls->size(); i++)
			{
				glPushMatrix();

				glTranslatef(balls->at(i).x, balls->at(i).y, 0);
				glRotatef(balls->at(i).angle * 180 / M_PI,0,0,1);
				glBegin(GL_LINE_LOOP);

				if(balls->at(i).mass < 4)
				{
					glColor3d(0,1,0);
				}
				else if(balls->at(i).mass < 7)
				{
					glColor3d(1,0,0);
				}
				else
				{
					glColor3d(0,0,1);
				}
				/*if(balls->at(i).stationary)
				{
					glColor3d(0,1,0);
				}
				else
				{
					glColor3d(1, 0, 0);
				}*/

				
				glVertex2d(0,0);
				for(float angle = 0.0f; angle < 2*M_PI; angle += 0.1f)
				{
					glVertex2d(sin(angle)*0.18,  cos(angle)*0.18);
				}
				
				//glVertex2d(sin(0.0f)*m_radius,  cos(0.0f)*m_radius);
				glEnd();
				if(balls->at(i).stationary)
				{
					glBegin(GL_POLYGON);
					glVertex2d(0,0);
					for(float angle = 0.0f; angle < 2*M_PI; angle += 0.1f)
					{
						glVertex2d(sin(angle)*0.18,  cos(angle)*0.18);
					}
					glEnd();
				}
				glPopMatrix();
			}
		}
		__finally
		{
			shared_data->Release();
		}
	}
}