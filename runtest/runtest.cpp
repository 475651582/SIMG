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
	Mat kernel(5, 5, SIMG_1C8U);
	uchar* b = kernel.dataPtr();
	b[0] = 0; b[1] = 0; b[2] = 1; b[3] = 0; b[4] = 0;
	b[5] = 0; b[6] = 1; b[7] = 1; b[8] = 1; b[9] = 0;
	b[10] = 1; b[11] = 1; b[12] = 1; b[13] = 1; b[14] = 1;
	b[15] = 0; b[16] = 1; b[17] = 1; b[18] = 1; b[19] = 0;
	b[20] = 0; b[21] = 0; b[22] = 1; b[23] = 0; b[24] = 0;

	 
	Mat m = imread("..\\testImage\\test.jpg");
	Mat m1 = m.convertTo(SIMG_METHOD_CONVERT_RGB2GRAY_STANDARD);
	Mat m2,m3;
	clock_t start =  clock();
	dilate(m1, m2, kernel);
	erode(m2, m2, kernel);
	
	clock_t stop2 = clock();
	imshow("rgb", m);
	imshow("dilate", m2);
	waitKey();
	
	return 0;
	
}

