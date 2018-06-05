#pragma once
#include <Windows.h>
#include <vector>
#include "simgMat.h"


#define MAX_WINDOW_NUMBER 200
#define MAX_WINDOW_NAME 200
#define DEFAULT_WINDOW_NAME "default window"

namespace Simg
{
	
	class sWindow
	{
	public:
		sWindow();
		sWindow(const char* winName, int x = 0, int y = 0, int w = 0, int h = 0);
		void resize(int w = 0, int h = 0);
		~sWindow();
		char* windowName() { return _windowName; };
		
		int loadMat(Mat m);
		HWND hwnd() { return _hwnd; }

	private:
		char _windowName[MAX_WINDOW_NAME];
		int _x, _y, _w, _h;
		int _channel;
		bool _initialized;
		Mat _mat;
		HWND _hwnd;
		WNDCLASSEX wndclassex;
		
		friend LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	};

	

	
	
	LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);	//所有窗口消息处理入口
	static HINSTANCE hg_hinstance = 0;
	
}

extern std::vector<Simg::sWindow> windowsList;
