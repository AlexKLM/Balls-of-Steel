#include <stdio.h>
#include <winsock2.h>
#include <Windows.h>
#include <concurrent_vector.h>
#include <vector>
#include "AntTweakBar.h"
#include "shared_struct.h"
#include "shared_memory.h"
#include "networking.h"
#include "Renderer.h"
const char TITLE[] = "Window Creation";

using namespace std;
using namespace Concurrency;

float current_time = 0;
float accumlator = 0;
float dt = 0.0166;
//TCP_comm tcp = TCP_comm();
networking* network;
Renderer* Render_thread;
shared_memory shared;
//********************OpenGL Initialization********************************
#include <gl\gl.h>                                // Header File For The OpenGL32 Library
#include <gl\glut.h>
#include <gl\glu.h>                               // Header File For The GLu32 Library
HDC			hDC=NULL;		// Private GDI Device Context
//*****************************************************************************
float camx = 0, camy = 0, zoom = 1;
float size_x = 0, size_y = 0;
//float zoomx, zoomy;
float W, H;
//************************ MESSAGE HANDLER **************************
bool button_down_L = false;
//bool button_down_R = false;
struct subwind{


};

void update_size()
{
	glMatrixMode( GL_MODELVIEW );
	glPopMatrix();
	glLoadIdentity();

	//Move camera to position
	glTranslatef( camx, camy, 0 );

	//Save default matrix again with camera translation
	glPushMatrix();
	if(shared.Acquire())
	{
		view* v = shared.get_current_v();
		__try
		{
			v->camx = camx;
			v->camy = camy;
			v->sizex = size_x;
			v->sizey = size_y;
		}
		__finally
		{
			shared.Release();
		}
	}
	

}

LRESULT CALLBACK WindowProc(HWND hwnd, 
	UINT msg, 
	WPARAM wparam, 
	LPARAM lparam)
{
	// this is the main message handler of the system
	PAINTSTRUCT		ps;	// used in WM_PAINT
	HDC				hdc;	// handle to a device context

	// what is the message 
	switch(msg)
	{	
	case WM_LBUTTONDOWN:
		{
			button_down_L = true;
			break;
		}
	case WM_RBUTTONDOWN:
		{
			shared.set_airject(true);
			GLint viewport[4];                  
			glGetIntegerv(GL_VIEWPORT, viewport);
			GLdouble modelview[16];
			glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
			GLdouble projection[16];
			glGetDoublev(GL_PROJECTION_MATRIX, projection);
			POINT p;
			GetCursorPos(&p);
			ScreenToClient(hwnd,&p);
			GLfloat x,y,z;
			GLdouble posx, posy, posz;
			x = (float)p.x;
			y = (float)p.y;


			y = (float)viewport[3] - y;
			gluUnProject( x, y, 0, modelview, projection, viewport, &posx, &posy, &posz);

			shared.set_airpos(posx,posy);
			break;
		}
	case WM_MOUSEMOVE:
		{
			if(shared.have_airjet())
			{
				GLint viewport[4];                  
				glGetIntegerv(GL_VIEWPORT, viewport);
				GLdouble modelview[16];
				glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
				GLdouble projection[16];
				glGetDoublev(GL_PROJECTION_MATRIX, projection);
				POINT p;
				GetCursorPos(&p);
				ScreenToClient(hwnd,&p);
				GLfloat x,y,z;
				GLdouble posx, posy, posz;
				x = (float)p.x;
				y = (float)p.y;


				y = (float)viewport[3] - y;
				gluUnProject( x, y, 0, modelview, projection, viewport, &posx, &posy, &posz);

				shared.set_airpos(posx,posy);
			}
			break;
		}
	case WM_RBUTTONUP:
		{
			shared.set_airject(false);
			network->add_air(shared.get_air_x(),shared.get_air_y());
		}
	case WM_LBUTTONUP:
		{
			if(button_down_L)
			{
				GLint viewport[4];                  
				glGetIntegerv(GL_VIEWPORT, viewport);
				GLdouble modelview[16];
				glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
				GLdouble projection[16];
				glGetDoublev(GL_PROJECTION_MATRIX, projection);


				POINT p;
				GetCursorPos(&p);
				ScreenToClient(hwnd,&p);
				

				GLfloat x,y,z;
				GLdouble posx, posy, posz;
				x = (float)p.x;
				y = (float)p.y;
				//glReadPixels(winX, winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);

				y = (float)viewport[3] - y;
				gluUnProject( x, y, 0, modelview, projection, viewport, &posx, &posy, &posz);
				//game->add_circle(posx,posy );
				network->add_ball(posx,posy);
				button_down_L = false;

			}
			break;
		}

	case WM_CREATE: 
		{
			// do initialization stuff here
			return(0);
		} 

	case WM_PAINT: 
		{
			// validate the window
			hdc = BeginPaint(hwnd,&ps);	 
			EndPaint(hwnd,&ps);
			return(0);
		} 

	case WM_KEYDOWN:
		{
			// Handle any non-accelerated key commands
			switch (wparam)
			{
			case VK_ESCAPE:
			case VK_F12:
				PostMessage(hwnd, WM_CLOSE, 0, 0);
				return (0);
			case 0x57://W
				{
				camy-=0.05;
				//glMatrixMode( GL_MODELVIEW );
				//glPopMatrix();
				//glLoadIdentity();

				////Move camera to position
				//glTranslatef( camx, camy, 0 );

				////Save default matrix again with camera translation
				//glPushMatrix();
				update_size();
				break;
				}
			case 0x53: //S
				{
				camy+=0.05;
				//glMatrixMode( GL_MODELVIEW );
				//glPopMatrix();
				//glLoadIdentity();

				////Move camera to position
				//glTranslatef( camx, camy, 0 );
				//
				////Save default matrix again with camera translation
				//glPushMatrix();

				update_size();
				break;
				}
			case 0x41: //A
				{
				camx+=0.05;
				//glMatrixMode( GL_MODELVIEW );
				//glPopMatrix();
				//glLoadIdentity();

				////Move camera to position
				//glTranslatef( camx, camy, 0 );

				////Save default matrix again with camera translation
				//glPushMatrix();

				update_size();
				break;
				}
			case 0x44: //D
				{
				camx-=0.05;
				//glMatrixMode( GL_MODELVIEW );
				//glPopMatrix();
				//glLoadIdentity();

				////Move camera to position
				//glTranslatef( camx, camy, 0 );

				////Save default matrix again with camera translation
				//glPushMatrix();

				update_size();
				break;
				}

			case 0x6B://+
				{
				zoom-=0.05;
				glMatrixMode( GL_PROJECTION );
				glLoadIdentity();

				size_x = (W/100)*zoom;
				size_y = (H/100)*zoom;
				float x = size_x *100;
				float y = size_y *100;
				//glOrtho(-6*zoom,6*zoom,-6*zoom,6*zoom,-10, 10);
				glOrtho(-size_x,size_x,-size_y,size_y,-10, 10);
				//Move camera to position-
				//glTranslatef( camx, camy, 0 );
				update_size();
				//Save default matrix again with camera translation
				//glPushMatrix();
				break;
				}
			case 0x6D://-
				{
					zoom+=0.05;
					glMatrixMode( GL_PROJECTION );
					glLoadIdentity();
					size_x = (W/100)*zoom;
					size_y = (H/100)*zoom;
					//glOrtho(-6*zoom,6*zoom,-6*zoom,6*zoom,-10, 10);
					glOrtho(-size_x,size_x,-size_y,size_y,-10, 10);
				//Move camera to position
				//glTranslatef( camx, camy, 0 );
				update_size();
				//Save default matrix again with camera translation
				//glPushMatrix();
				break;
				}
			case 0x20: //spacebar
				{
					network->togglepause();
					break;
				}
			case 0x46: //F
				{
					Render_thread->modifyRS(-1);
					break;
				}
			case 0x47: //G
				{
					Render_thread->modifyRS(1);
					break;
				}
			case 0x42: //B
				{
					network->togglerope();
					break;
				}
			case 0x4F: //O
				{
					shared.change_phy(-1);
					break;
				}
			case 0x50: //P
				{
					shared.change_phy(1);
					break;
				}
			case 0x4D: //M
				{
					shared.change_net(1);
//					tcp.adjust_nettime(-1);
					break;
				}
			case 0x4E://N
				{
					shared.change_net(-1);
//					tcp.adjust_nettime(1);
					break;
				}
			case 0x43: //C
				{
					network->request_reset();
					break;
				}
			//default:
				//char message[15];
				//sprintf_s(message, "Key Pressed: %c", (char)wparam);
				//MessageBox(NULL, message, "Key Pressed", MB_OK);
			}
			break;
		}
	case WM_SIZING:
		{
			W = WMSZ_RIGHT - WMSZ_LEFT;
			H = WMSZ_TOP - WMSZ_BOTTOM;
			break;
		}
	case WM_DESTROY: 
		{
			// kill the application			
			PostQuitMessage(0);

			return(0);
		}

	default:
		break;

	} // end switch
	// process any messages that we didn't take care of 
	return (DefWindowProc(hwnd, msg, wparam, lparam));

} // end WinProc

//**************************Setup OpenGL***********************


//************************ WIN MAIN***********************
bool started = false;
int WINAPI WinMain( HINSTANCE hinstance,
	HINSTANCE hprevinstance,
	LPSTR lpcmdline,
	int ncmdshow)
{
	WNDCLASS	winclass;	// this will hold the class we create
	HWND		hwnd;		// generic window handle
	MSG			msg;		// generic message

	// first fill in the window class stucture
	winclass.style			= CS_HREDRAW | CS_VREDRAW;                  
	winclass.lpfnWndProc	= WindowProc;
	winclass.cbClsExtra		= 0;
	winclass.cbWndExtra		= 0;
	winclass.hInstance		= hinstance;
	winclass.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
	winclass.hCursor		= LoadCursor(NULL, IDC_ARROW);
	winclass.hbrBackground  = (HBRUSH)GetStockObject(BLACK_BRUSH);
	winclass.lpszMenuName	= NULL;
	winclass.lpszClassName	= L"WindowCreation";

	// register the window class
	if (!RegisterClass(&winclass))
		return(0);

	// create the window
	if (!(hwnd = CreateWindowA("WindowCreation", // class
		"Balls of steel",	     // title
		WS_OVERLAPPEDWINDOW| WS_VISIBLE,
		0,
		0,
		//Set the size of the window to the size of the screen 
		1920,
		1024,
		//GetSystemMetrics(SM_CXSCREEN),
		//GetSystemMetrics(SM_CYSCREEN),
		NULL,	   // handle to parent 
		NULL,	   // handle to menu
		hinstance,	// instance
		NULL)))	// creation parms
		return(0);

	W = 1920;
	H = 1024;
	//InitializeOpenGL(hwnd, 1920, 1024);
	Render_thread = new Renderer(&shared,hDC);
	Render_thread->InitializeOpenGL(hwnd,1920,1024);
	shared = shared_memory();
	size_x = (1920/100)*zoom;
	size_y = (1024/100)*zoom;
	view* v = shared.get_current_v();
	v->camx = 0;
	v->camy = 0;
	v->sizex = size_x;
	v->sizey = size_y;
	//InitializeOpenGL(hwnd, 1920, 1024);
	TwInit(TW_OPENGL, NULL);


	


	network = new networking(9173,&shared);
	network->setup();
	
//	tcp = TCP_comm(&shared);
	// enter main event loop
	bool quit = false;
	//cout<<"test"<<endl;
//	tcp.setup();
	bool started = false;
	while(!quit)
	{
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{ 
			// test if this is a quit
			if (msg.message == WM_QUIT) quit = true;

			// translate any accelerator keys
			TranslateMessage(&msg);
			// send the message to the window proc
			DispatchMessage(&msg);
		} // end if
		else {
			if(!started)
			{
				network->start();
				Render_thread->start();
//				tcp.start();
				//Render_thread.start();
				started = true;
			}
			Render_thread->check_render();
		}

	} // end while

	//delete game;

	// return to Windows like this
	return(msg.wParam);

} // end WinMain


//************************ END OF WIN MAIN ***********************
