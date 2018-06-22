/*******************************************************************
*
*	Author: Felix Shang
*	Company: [personal]
*	Date: 2018.06.05
*	Brief: window use for showing the images
*
******************************************************************/

#pragma once
#include <Windows.h>
#include <vector>
#include "simgMat.h"


#define MAX_WINDOW_NUMBER 200
#define MAX_WINDOW_NAME 200
#define DEFAULT_WINDOW_NAME "default window"



namespace Simg
{
	const int SIMG_WINDOW_STYLE_NORMAL =	0x00000001;
	const int SIMG_WINDOW_STYLE_STRETCH =	0x00000002;
	const int SIMG_WINDOW_MESSAGE_TIMEUP = 0x00010001;
	const int SIMG_WINDOW_TIMERID = 0x00000001;
	
	class sWindow
	{
	public:
		sWindow();
		sWindow(const char* winName, int x = 0, int y = 0, int w = 0, int h = 0, int windowStyle = SIMG_WINDOW_STYLE_NORMAL);
		void refresh();
		void resize(int w = 0, int h = 0);
		~sWindow();
		char* windowName() { return _windowName; };
		
		int updateMat(Mat m);		//load mat data to the window for showing
		bool needRender() { return _needRender; };
		HWND hwnd() { return _hwnd; }

	private:
		char _windowName[MAX_WINDOW_NAME];	//window name and id (they are the same)
		int _x, _y, _w, _h;	//location and size of the window
		int _windowStyle;
		
		bool _initialized;	//if the Mat data is assigned to the window, this value should be true, otherwise is false
		bool _needRender;	//is the window need to be rendered
		Mat _mat;			//assigned mat
		HWND _hwnd;			//handle of the window
		WNDCLASSEX wndclassex;	//some settings of the window
		
		friend LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam); //call back function of the window
	};

	

	
	
	LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);	//���д�����Ϣ�������
	static HINSTANCE hg_hinstance = 0;
	
}

extern std::vector<Simg::sWindow> windowsList;
