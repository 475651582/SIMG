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
	
	class sWindow
	{
	public:
		sWindow();
		sWindow(const char* winName, int x = 0, int y = 0, int w = 0, int h = 0, int windowStyle = 0);
		void resize(int w = 0, int h = 0);
		~sWindow();
		char* windowName() { return _windowName; };
		
		int loadMat(Mat m);		//load mat data to the window for showing
		HWND hwnd() { return _hwnd; }

	private:
		char _windowName[MAX_WINDOW_NAME];	//window name and id (they are the same)
		int _x, _y, _w, _h;	//location and size of the window
		
		bool _initialized;	//if the Mat data is assigned to the window, this value should be true, otherwise is false
		Mat _mat;			//assigned mat
		HWND _hwnd;			//handle of the window
		WNDCLASSEX wndclassex;	//some settings of the window
		
		friend LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam); //call back function of the window
	};

	

	
	
	LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);	//���д�����Ϣ�������
	static HINSTANCE hg_hinstance = 0;
	
}

extern std::vector<Simg::sWindow> windowsList;
