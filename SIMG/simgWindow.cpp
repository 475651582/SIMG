/*******************************************************************
*
*	Author: Felix Shang
*	Company: [personal]
*	Date: 2018.06.05
*	Brief: window use for showing the images
*
******************************************************************/


#include "stdafx.h"
#include "simgWindow.h"




Simg::sWindow::sWindow()
{
}

Simg::sWindow::sWindow(const char * winName, int x, int y, int w, int h, int windowStyle)
{
	
	assert(x >=0 && y >= 0 && w >= 0 && h >= 0);
	strcpy(_windowName, winName);
	_x = x; _y = y; _w = w; _h = h; _windowStyle = windowStyle;
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

void Simg::sWindow::refresh()
{
	bool isVisable = IsWindowVisible(_hwnd);
	if (!isVisable)
	{
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
	ShowWindow(_hwnd, 1);
	UpdateWindow(_hwnd);
	InvalidateRect(_hwnd, NULL, false);
	
}

void Simg::sWindow::resize(int w, int h)
{
	
}

Simg::sWindow::~sWindow()
{
	
}

int Simg::sWindow::updateMat(Mat m)
{
	assert(NULL != m._dataPtr);
	
	_mat = Mat(m.copy()); 
	_initialized = true;
	_needRender = true;
	return 0;
}





LRESULT CALLBACK Simg::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	static RECT rect;
	sWindow *win = NULL;
	int margin_w = 20, margin_h = 43, margin_x = -10, margin_y = -33;
	for (size_t i = 0; i < windowsList.size(); i++)
	{
		win = &windowsList[i];
		if (win->hwnd() == hwnd) break;
	}


	switch (message)
	{
	case WM_SIZE:
		GetClientRect(hwnd, &rect);
		win->_w = (int)(short)LOWORD(lParam) + margin_w;
		win->_h = (int)(short)HIWORD(lParam) + margin_h;
		return (0);

	case WM_MOVE:
		GetClientRect(hwnd, &rect);
		win->_x = LOWORD(lParam) + margin_x;
		win->_y = HIWORD(lParam) + margin_y;

		return (0);

	case WM_PAINT:
	{
		GetClientRect(hwnd, &rect);
		HDC hdcmem = NULL;
		HBITMAP hbmp;
		uchar* matBuffer = NULL;	//tmp buffer for reasssigning the mat data
		void *DIBArray;	//buffer for DIB
		UINT DIBTotalBytes;
		hdc = BeginPaint(hwnd, &ps);
		//����mat
		//����windowsList��Ѱ�ұ�����Ĵ���ID
		
		//û�б�����mat�Ĵ��岻���к�������ʾ������loadMat����ʼ����
		if (!win->_initialized)
		{
			return 0;
		}
		
		BITMAPINFO bmpInfo; //����λͼ��Ϣ
		bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmpInfo.bmiHeader.biWidth = win->_mat._cols;//���
		bmpInfo.bmiHeader.biHeight = -win->_mat._rows;//�߶�
		bmpInfo.bmiHeader.biPlanes = 1;

		switch (win->_mat._dataType)
		{
		case SIMG_3C8U:
		{
			

			bmpInfo.bmiHeader.biBitCount = 24;
			bmpInfo.bmiHeader.biCompression = BI_RGB;

			int lineByte = (bmpInfo.bmiHeader.biWidth * bmpInfo.bmiHeader.biBitCount / 8 + 3) / 4 * 4;
			
			hdcmem = CreateCompatibleDC(hdc);
			DIBTotalBytes = win->_mat._rows * lineByte;
			matBuffer = new  uchar[DIBTotalBytes];

		
			
			for (int i = 0; i <  win->_mat._rows; i++)
			{
				memcpy(matBuffer + i * lineByte, win->_mat._dataPtr + i * win->_mat._cols * 3, win->_mat._cols * 3);
			}
			break;
		}

		case SIMG_1C8U:
		{
			bmpInfo.bmiHeader.biCompression = BI_RGB;
			bmpInfo.bmiHeader.biBitCount = 24;
			hdcmem = CreateCompatibleDC(hdc);
			int lineByte = (bmpInfo.bmiHeader.biWidth * bmpInfo.bmiHeader.biBitCount / 8 + 3) / 4 * 4;
			DIBTotalBytes = win->_mat._rows * lineByte;
			matBuffer = new  uchar[DIBTotalBytes];

					
			for (int i = 0; i < win->_mat._rows * win->_mat._cols; i++)
			{
				int x = i % win->_mat._cols;
				int y = i / win->_mat._cols;
				matBuffer[3 * x + y * lineByte] = (uchar) win->_mat._dataPtr[i];
				matBuffer[3 * x + y * lineByte + 1] = (uchar)win->_mat._dataPtr[i];
				matBuffer[3 * x + y * lineByte + 2] = (uchar)win->_mat._dataPtr[i];
			}
			break;
		}
		case SIMG_1C16S:
		{
			bmpInfo.bmiHeader.biCompression = BI_RGB;
			bmpInfo.bmiHeader.biBitCount = 24;
			hdcmem = CreateCompatibleDC(hdc);
			int lineByte = (bmpInfo.bmiHeader.biWidth * bmpInfo.bmiHeader.biBitCount / 8 + 3) / 4 * 4;

			DIBTotalBytes = win->_mat._rows * lineByte;
			matBuffer = new  uchar[DIBTotalBytes];
			vector<short> matMax, matMin;
			win->_mat.getMax(matMax);
			win->_mat.getMin(matMin);
			assert(matMax.size() > 0 && matMin.size() > 0);

			short minValue = matMin[0];
			short maxValue = matMax[0];
			//minValue = 0;
			//maxValue = 255;
			for (int i = 0; i < win->_mat._rows * win->_mat._cols; i++)
			{
				int x = i % win->_mat._cols;
				int y = i / win->_mat._cols;

				short *ptr = (short*)win->_mat._dataPtr;
				short ptrData = MAX(MIN(ptr[i], maxValue), minValue);
				uchar grayData = (uchar)((ptrData - minValue) / (maxValue - minValue + 0.0f) * UCHAR_MAX);
				matBuffer[3 * x + y * lineByte] = grayData;
				matBuffer[3 * x + y * lineByte + 1] = grayData;
				matBuffer[3 * x + y * lineByte + 2] = grayData;
			}
			break;
		}
		case SIMG_1C32F:
		{
			bmpInfo.bmiHeader.biCompression = BI_RGB;
			bmpInfo.bmiHeader.biBitCount = 24;
			hdcmem = CreateCompatibleDC(hdc);
			int lineByte = (bmpInfo.bmiHeader.biWidth * bmpInfo.bmiHeader.biBitCount / 8 + 3) / 4 * 4;

			DIBTotalBytes = win->_mat._rows * lineByte;
			matBuffer = new  uchar[DIBTotalBytes];
			vector<float> matMax, matMin;
			win->_mat.getMax(matMax);
			win->_mat.getMin(matMin);
			assert(matMax.size() > 0 && matMin.size() > 0);

			float minValue = matMin[0];
			float maxValue = matMax[0];
			for (int i = 0; i < win->_mat._rows * win->_mat._cols; i++)
			{
				int x = i % win->_mat._cols;
				int y = i / win->_mat._cols;

				float *ptr = (float*)win->_mat._dataPtr;
				uchar grayData = (uchar)((ptr[i] - minValue) / (maxValue - minValue) * UCHAR_MAX);
				matBuffer[3 * x + y * lineByte] = grayData;
				matBuffer[3 * x + y * lineByte + 1] = grayData;
				matBuffer[3 * x + y * lineByte + 2] = grayData;
			}
			break;
		}
		case SIMG_3C32F:
		{
			bmpInfo.bmiHeader.biCompression = BI_RGB;
			bmpInfo.bmiHeader.biBitCount = 24;
			hdcmem = CreateCompatibleDC(hdc);
			int lineByte = (bmpInfo.bmiHeader.biWidth * bmpInfo.bmiHeader.biBitCount / 8 + 3) / 4 * 4;

			DIBTotalBytes = win->_mat._rows * lineByte;
			matBuffer = new  uchar[DIBTotalBytes];
			vector<float> matMax, matMin;
			win->_mat.getMax(matMax);
			win->_mat.getMin(matMin);
			assert(matMax.size() > 2 && matMin.size() > 2);

			float minValueB = matMin[0], maxValueB = matMax[0];
			float minValueG = matMin[1], maxValueG = matMax[1];
			float minValueR = matMin[2], maxValueR = matMax[2];

			for (int i = 0; i < win->_mat._rows * win->_mat._cols; i++)
			{
				int x = i % win->_mat._cols;
				int y = i / win->_mat._cols;

				float *ptr = (float*)win->_mat._dataPtr;
				uchar dataB = (uchar)((ptr[3*i] - minValueB) / (maxValueB - minValueB) * UCHAR_MAX);
				uchar dataG = (uchar)((ptr[3 * i + 1] - minValueG) / (maxValueG - minValueG) * UCHAR_MAX);
				uchar dataR = (uchar)((ptr[3 * i + 2] - minValueR) / (maxValueR - minValueR) * UCHAR_MAX);
				matBuffer[3 * x + y * lineByte] = dataB;
				matBuffer[3 * x + y * lineByte + 1] = dataG;
				matBuffer[3 * x + y * lineByte + 2] = dataR;
			}
			break;
		}
		default:
			break;
		}
		
		



		
		DIBArray = new BYTE(DIBTotalBytes);
		hbmp = CreateDIBSection(NULL, &bmpInfo, DIB_RGB_COLORS, &DIBArray, NULL, 0);//����DIB																				  
		memcpy(DIBArray, matBuffer, DIBTotalBytes);	//! �������ݸ��Ƶ�bitmap��������������


		SelectObject(hdcmem, hbmp);
		if (SIMG_WINDOW_STYLE_NORMAL == win->_windowStyle)
		{
			BitBlt(hdc, 0, 0, bmpInfo.bmiHeader.biWidth, -bmpInfo.bmiHeader.biHeight, hdcmem, 0, 0, SRCCOPY); //���ڴ��е�ͼ��������Ļ�Ͻ�����ʾ
		}
		else if (SIMG_WINDOW_STYLE_STRETCH == win->_windowStyle)
		{
			StretchBlt(hdc, rect.left, rect.top, rect.right - rect.left + 1, rect.bottom - rect.top + 1, hdcmem, 0, 0, bmpInfo.bmiHeader.biWidth, -bmpInfo.bmiHeader.biHeight, SRCCOPY);
		}
		
		
		DeleteObject(hbmp);
		DeleteDC(hdcmem);
		EndPaint(hwnd, &ps);

		delete matBuffer, DIBArray;
		matBuffer = NULL; DIBArray = NULL;
		return (0);
	}
	

	case WM_DESTROY:
		PostQuitMessage(0);
		win->_needRender = false;
		return (0);
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}

