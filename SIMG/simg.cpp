#include "stdafx.h"
#include "simg.h"

using namespace Simg;

Mat Simg::readBMP(const char *path)
{
	//bmp存储方式是以左下角第一个像素为坐标原点的，和通常的使用习惯不太一致，需在索引的时候注意。
	Mat ret;
	DWORD w, h;								//定义读取图像的长和宽
											//DWORD bitCorlorUsed;					//定义
	DWORD bitSize;							//定义图像的大小
	BITMAPFILEHEADER bf;					//图像文件头
	BITMAPINFOHEADER bi;					//图像文件头信息
	uchar *buf;								//定义文件读取缓冲区

	FILE *fp = fopen(path, "rb");
	assert(fp != 0);


	fread(&bf, sizeof(BITMAPFILEHEADER), 1, fp);
	fread(&bi, sizeof(BITMAPINFOHEADER), 1, fp);//读取BMP文件头文件信息

	w = bi.biWidth;                            //获取图像的宽
	h = bi.biHeight;                           //获取图像的高
	bitSize = bi.biSizeImage;                  //获取图像的size

	buf = new uchar[w*h * 3];                //分配缓冲区大小		
	fread(buf, 1, w*h * 3, fp);                   //开始读取数据

	ret = Mat(w, h, SIMG_3C8U);
	memcpy(ret.dataPtr(), buf, w*h * 3);

	delete buf; buf = NULL;

	fclose(fp);
	return ret;
}


Mat Simg::imread(const char* path)
{
	Mat ret;
	string s_path;
	assert(NULL != path); //path should have a value
	assert(_access(path, 0) == 0);

	//提取后缀名
	s_path = path;
	int ind_suffix = s_path.rfind('.');
	string suffix = s_path.substr(ind_suffix + 1, s_path.length());

	if ("bmp" == suffix)
	{
		ret = Simg::readBMP(path);
		ret._originalFormat = SIMG_FORMAT_IMG_BMP;
	}
	else if ("jpg" == suffix)
	{
		ret._originalFormat = SIMG_FORMAT_IMG_JPEG;
	}
	else if ("png" == suffix)
	{
		ret._originalFormat = SIMG_FORMAT_IMG_PNG;
	}
	else
	{
		//如果没有找到支持的后缀名
	}

	return ret;
}

int Simg::namedWindow(const char * windowName, int windowStyle)
{
	
	HWND         hwnd;
	
	WNDCLASSEX   wndclassex = { 0 };

	wndclassex.cbSize = sizeof(WNDCLASSEX);
	wndclassex.style = CS_HREDRAW | CS_VREDRAW;
	wndclassex.lpfnWndProc = WndProc;
	wndclassex.cbClsExtra = 0;
	wndclassex.cbWndExtra = 0;
	wndclassex.hInstance = hg_hinstance;
	wndclassex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclassex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclassex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclassex.lpszClassName = windowName;
	wndclassex.hIconSm = wndclassex.hIcon;

	if (!RegisterClassEx(&wndclassex))
	{
		MessageBox(NULL, TEXT("RegisterClassEx failed!"), windowName, MB_ICONERROR);
		return 0;
	}
	hwnd = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW,
		windowName,
		windowName,
		WS_OVERLAPPEDWINDOW,
		0,
		0,
		500,
		500,
		NULL,
		NULL,
		hg_hinstance,
		NULL);

	ShowWindow(hwnd, 1);
	UpdateWindow(hwnd);
	
}

int Simg::waitKey()
{
	MSG  msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}




LRESULT CALLBACK Simg::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	static RECT rect;
	TCHAR szDrawText[] = TEXT("欢迎访问我的博客http://blog.csdn.net/wenzhou1219");

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
		hdcmem = CreateCompatibleDC(hdc);
		hbmp = (HBITMAP)LoadImage(NULL, "D:\\0.data\\test.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

		char* buffer = new  char[480 * 480 * 3];
		for (size_t i = 0; i < 480 * 480; i++)
		{
			buffer[3 * i] = 255;
			buffer[3 * i + 1] = 0;
			buffer[3 * i + 2] = 125;
		}

		BITMAPINFO bmpInfo; //创建位图 
		bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmpInfo.bmiHeader.biWidth = 480;//宽度
		bmpInfo.bmiHeader.biHeight = 480;//高度
		bmpInfo.bmiHeader.biPlanes = 1;
		bmpInfo.bmiHeader.biBitCount = 24;
		bmpInfo.bmiHeader.biCompression = BI_RGB;

		UINT uiTotalBytes = 480 * 480 * 3;
		void *pArray = new BYTE(uiTotalBytes);
		hbmp = CreateDIBSection(NULL, &bmpInfo, DIB_RGB_COLORS, &pArray, NULL, 0);//创建DIB

																				  //! 将裸数据复制到bitmap关联的像素区域
		memcpy(pArray, buffer, uiTotalBytes);



		//GetObject(hbmp, sizeof(bmp), &bmp);
		SelectObject(hdcmem, hbmp);
		//BitBlt(hdc, 0, 0, bmp.bmWidth, bmp.bmHeight, hdcmem, 0, 0, SRCCOPY); //将内存中的图拷贝到屏幕上进行显示
		BitBlt(hdc, 0, 0, 480, 480, hdcmem, 0, 0, SRCCOPY); //将内存中的图拷贝到屏幕上进行显示

		DeleteObject(hbmp);
		DeleteDC(hdcmem);
		EndPaint(hwnd, &ps);
		return (0);
	}



	case WM_DESTROY:
		PostQuitMessage(0);
		return (0);
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}
