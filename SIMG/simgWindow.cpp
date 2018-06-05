#include "stdafx.h"
#include "simgWindow.h"







Simg::sWindow::sWindow()
{
}

Simg::sWindow::sWindow(const char * winName, int x, int y, int w, int h)
{
	//TODO �������û����
	strcpy(_windowName, winName);
	_x = x; _y = y; _w = w; _h = h;
	_channel = 3;
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
	
		HDC hdcmem;
		HBITMAP hbmp;
		hdc = BeginPaint(hwnd, &ps);
		//����mat
		//����windowsList��Ѱ�ұ�����Ĵ���ID
		sWindow *win = NULL;
		for (size_t i = 0; i < windowsList.size(); i++)
		{
			win = &windowsList[i];
			if (win->hwnd() == hwnd)
			{
				break;
			}
		}
		if (!win->_initialized)
		{
			return 0;
		}

		hdcmem = CreateCompatibleDC(hdc);
		UINT uiTotalBytes = win->_mat._rows * win->_mat._cols * 3;
		char* buffer = new  char[uiTotalBytes];
		memcpy(buffer, win->_mat._dataPtr, uiTotalBytes);
		

		BITMAPINFO bmpInfo; //����λͼ 
		bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmpInfo.bmiHeader.biWidth = win->_mat._cols;//���
		bmpInfo.bmiHeader.biHeight = win->_mat._rows;//�߶�
		bmpInfo.bmiHeader.biPlanes = 1;
		bmpInfo.bmiHeader.biBitCount = 24;
		bmpInfo.bmiHeader.biCompression = BI_RGB;

		
		void *pArray = new BYTE(uiTotalBytes);
		hbmp = CreateDIBSection(NULL, &bmpInfo, DIB_RGB_COLORS, &pArray, NULL, 0);//����DIB

																				  //! �������ݸ��Ƶ�bitmap��������������
		memcpy(pArray, buffer, uiTotalBytes);



		//GetObject(hbmp, sizeof(bmp), &bmp);
		SelectObject(hdcmem, hbmp);
		//BitBlt(hdc, 0, 0, bmp.bmWidth, bmp.bmHeight, hdcmem, 0, 0, SRCCOPY); //���ڴ��е�ͼ��������Ļ�Ͻ�����ʾ
		BitBlt(hdc, 0, 0, bmpInfo.bmiHeader.biWidth, bmpInfo.bmiHeader.biHeight, hdcmem, 0, 0, SRCCOPY); //���ڴ��е�ͼ��������Ļ�Ͻ�����ʾ

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

