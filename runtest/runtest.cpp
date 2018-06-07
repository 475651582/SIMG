// runtest.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "../SIMG/simg.h"


#pragma comment(lib, "..\\Debug\\SIMG.lib")

using namespace Simg;
int main()
{
	Mat m1 = imread("D:\\0.data\\test-gray.bmp");
	Mat m = imread("D:\\0.data\\test.jpg");

	m1 = m1 - 50;
	m = m - 100;
	namedWindow("m", SIMG_WINDOW_STYLE_STRETCH);
	
	namedWindow("m1", SIMG_WINDOW_STYLE_NORMAL);

	imshow("m", m);
	imshow("m1", m1);
	waitKey();
	
	return 0;
	
}

