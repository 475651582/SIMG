#include "stdafx.h"
#include "simgWindow.h"







Simg::sWindow::sWindow()
{
}

Simg::sWindow::sWindow(const char * winName, int x, int y, int w, int h, int windowStyle)
{
	
	assert(x >=0 && y >= 0 && w >= 0 && h >= 0);
	strcpy(_windowName, winName);
	_x = x; _y = y; _w = w; _h = h;
	_initialized = false;

	wndclassex = { 0 };
	wndclassex.cbSize = sizeof(WNDCLASSEX);
	wndclassex.style = CS_HREDRAW | CS_VREDRAW;
	wndclassex.lpfnWndProc = WndProc;
	wndclassex.cbClsExtra = 0;
	wndclassex.cbWndExtra = 0;
	wndclassex.hInstance = hg_hinstance;
	wndclassex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclassex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclassex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclassex.lpszClassName = winName;
	wndclassex.hIconSm = wndclassex.hIcon;

	if (!RegisterClassEx(&wndclassex))
	{
		MessageBox(NULL, TEXT("RegisterClassEx failed!"), _windowName, MB_ICONERROR);
		return;
	}
	_hwnd = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW,
		_windowName,
		_windowName,
		WS_OVERLAPPEDWINDOW,
		_x,
		_y,
		_w,
		_h,
		NULL,
		NULL,
		hg_hinstance,
		NULL);

	
	
}

void Simg::sWindow::resize(int w, int h)
{
	CloseWindow(_hwnd);
	_w = w; _h = h;
	_hwnd = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW,
		_windowName,
		_windowName,
		WS_OVERLAPPEDWINDOW,
		_x,
		_y,
		_w,
		_h,
		NULL,
		NULL,
		hg_hinstance,
		NULL);


	ShowWindow(_hwnd, 1);
	UpdateWindow(_hwnd);
}

Simg::sWindow::~sWindow()
{
}

int Simg::sWindow::loadMat(Mat m)
{
	assert(NULL != m._dataPtr);

	_mat = Mat(m); 
	_initialized = true;

	ShowWindow(_hwnd, 1);
	UpdateWindow(_hwnd);

	return 0;
}





LRESULT CALLBACK Simg::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	static RECT rect;
	

	switch (message)
	{
	case WM_SIZE:
		GetClientRect(hwnd, &rect);
		return (0);

	case WM_PAINT:
	{
		HDC hdcmem = NULL;
		HBITMAP hbmp;
		char* buffer = NULL;	//绘图buffer
		void *pArray;	//给位图的绘图buffer
		UINT uiTotalBytes;
		hdc = BeginPaint(hwnd, &ps);
		//绘制mat
		//遍历windowsList，寻找被激活的窗体ID
		sWindow *win = NULL;
		for (size_t i = 0; i < windowsList.size(); i++)
		{
			win = &windowsList[i];
			if (win->hwnd() == hwnd) break;
		}

		//没有被分配mat的窗体不进行后续的显示操作（loadMat，初始化）
		if (!win->_initialized)
		{
			return 0;
		}
		
		BITMAPINFO bmpInfo; //创建位图信息
		bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmpInfo.bmiHeader.biWidth = win->_mat._cols;//宽度
		bmpInfo.bmiHeader.biHeight = win->_mat._rows;//高度
		bmpInfo.bmiHeader.biPlanes = 1;
		bmpInfo.bmiHeader.biBitCount = 24;
		bmpInfo.bmiHeader.biCompression = BI_RGB;

		switch (win->_mat._dataType)
		{
		case SIMG_3C8U:
		{
			hdcmem = CreateCompatibleDC(hdc);
			uiTotalBytes = win->_mat._rows * win->_mat._cols * 3;
			buffer = new  char[uiTotalBytes];
			memcpy(buffer, win->_mat._dataPtr, uiTotalBytes);
		}
		default:
			break;
		}
		
		



		
		pArray = new BYTE(uiTotalBytes);
		hbmp = CreateDIBSection(NULL, &bmpInfo, DIB_RGB_COLORS, &pArray, NULL, 0);//创建DIB																				  
		memcpy(pArray, buffer, uiTotalBytes);	//! 将裸数据复制到bitmap关联的像素区域


		SelectObject(hdcmem, hbmp);
		BitBlt(hdc, 0, 0, bmpInfo.bmiHeader.biWidth, bmpInfo.bmiHeader.biHeight, hdcmem, 0, 0, SRCCOPY); //将内存中的图拷贝到屏幕上进行显示

		DeleteObject(hbmp);
		DeleteDC(hdcmem);
		EndPaint(hwnd, &ps);

		delete buffer, pArray;
		buffer = NULL; pArray = NULL;
		return (0);
	}
	

	case WM_DESTROY:
		PostQuitMessage(0);
		return (0);
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}

