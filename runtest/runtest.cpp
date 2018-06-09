// runtest.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "../SIMG/simg.h"
#include "../SIMG/imgprocess.h"
#include <Windows.h>
#include <time.h>

#pragma comment(lib, "..\\Release\\SIMG.lib")

using namespace Simg;
int main()
{
	Mat kernel(5, 5, SIMG_1C8U);
	uchar* b = kernel.dataPtr();
	b[0] = 0; b[1] = 0; b[2] = 0; b[3] = 0; b[4] = 0;
	b[5] = 1; b[6] = 1; b[7] = 1; b[8] = 1; b[9] = 1;
	b[10] = 1; b[11] = 1; b[12] = 1; b[13] = 1; b[14] = 1;
	b[15] = 1; b[16] = 1; b[17] = 1; b[18] = 1; b[19] = 1;
	b[20] = 0; b[21] = 0; b[22] = 0; b[23] = 0; b[24] = 0;

	 
	Mat m = imread("..\\testImage\\test.jpg");
	Mat m1 = m.convertTo(SIMG_METHOD_CONVERT_RGB2GRAY_STANDARD);
	Mat m2,m3;
	clock_t start =  clock();
	for (size_t i = 0; i < 100; i++)
	{
		dilate(m1, m2, kernel);
	}
	
	clock_t stop1 = clock();
	for (size_t i = 0; i < 100; i++)
	{
		erode(m1, m3, kernel);
	}
	
	clock_t stop2 = clock();
	imshow("rgb", m);
	imshow("dilate", m2);
	imshow("erode", m3);
	printf("dilate: %f ms\n", (stop1 - start)/100.0f);
	printf("erode: %f ms\n", (stop2 - stop1)/100.0f);
	waitKey();
	
	return 0;
	
}

