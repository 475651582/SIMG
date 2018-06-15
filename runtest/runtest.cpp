// runtest.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "../SIMG/simg.h"
#include "../SIMG/imgprocess.h"
#include <Windows.h>
#include <time.h>

#if _DEBUG
#pragma comment(lib, "..\\Debug\\SIMG.lib")
#else
#pragma comment(lib, "..\\Release\\SIMG.lib")
#endif // _DEBUG



using namespace Simg;
int main()
{
	Mat k(3, 3, SIMG_1C8S);
	k.setPixel(0, 0, -1); k.setPixel(1, 0, -1); k.setPixel(2, 0, -1);
	k.setPixel(0, 1, 0); k.setPixel(1, 1, 0); k.setPixel(2, 1, 0);
	k.setPixel(0, 2, 1); k.setPixel(1, 2, 1); k.setPixel(2, 2, 1);
	 
	Mat m = imread("..\\testImage\\lena-gray.bmp");
	Mat m1;
	//m = m.convertTo(SIMG_METHOD_CONVERT_RGB2GRAY_AVERAGE);
	conv(m, m1, k);

	imshow("fast", m1);
	waitKey(0);
	
	return 0;
	
}

