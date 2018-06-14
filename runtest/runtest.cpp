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

	 
	Mat m = imread("..\\testImage\\scene.jpg");
	Mat m1 = m.convertTo(SIMG_METHOD_CONVERT_RGB2GRAY_AVERAGE);
	Mat m2,m3;
	//m1.setPixel(0, 0, 0);
	//m1.setPixel(1, 0, 50);
	//m1.setPixel(0, 1, 100);
	//m1.setPixel(1, 1, 200);
	imshow("ori", m1,SIMG_WINDOW_STYLE_STRETCH);
	Size dsize;
	dsize.x = 1024;
	dsize.y = 768;

	clock_t t1 = clock();
	for (size_t i = 0; i < 100; i++)
	{
		resize(m1, m2, dsize, SIMG_METHOD_RESIZE_LINEAR_FAST);
	}
	clock_t t2 = clock();
	printf("fast = %f ms\n", (t2 - t1) / 100.0);



	clock_t t3 = clock();
	for (size_t i = 0; i < 100; i++)
	{
		resize(m1, m3, dsize, SIMG_METHOD_RESIZE_LINEAR);
	}
	clock_t t4 = clock();
	printf("slow = %f ms\n", (t4 - t3) / 100.0);

	imshow("fast", m2, SIMG_WINDOW_STYLE_STRETCH);
	imshow("slow", m3, SIMG_WINDOW_STYLE_STRETCH);
	waitKey(0);
	
	return 0;
	
}

