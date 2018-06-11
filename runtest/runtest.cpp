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
	//b[0] = 0; b[1] = 0; b[2] = 0; b[3] = 0; b[4] = 0;
	//b[5] = 0; b[6] = 0; b[7] = 1; b[8] = 0; b[9] = 0;
	//b[10] =0; b[11] = 1; b[12] = 1; b[13] = 1; b[14] = 0;
	//b[15] = 0; b[16] = 0; b[17] = 1; b[18] = 0; b[19] = 0;
	//b[20] = 0; b[21] = 0; b[22] = 0; b[23] = 0; b[24] = 0;
	kernel = getMorphStructor(11, 11, SIMG_MORPH_CIRCLE);

	 
	Mat m = imread("..\\testImage\\test.jpg");
	
	Mat m1 = m.convertTo(SIMG_METHOD_CONVERT_RGB2GRAY_STANDARD);
	Mat m2,m3;
	clock_t start =  clock();
	threshold(m1, m2, 100);
	threshold(m1, m3, 150);

	/*namedWindow("gray", SIMG_WINDOW_STYLE_STRETCH);
	namedWindow("dilate", SIMG_WINDOW_STYLE_STRETCH);*/
	
	clock_t stop2 = clock();
	imshow("gray", m1);
	imshow("100", m2);
	imshow("150", m3);
	waitKey();
	
	return 0;
	
}

