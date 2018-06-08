// runtest.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "../SIMG/simg.h"
#include "../SIMG/imgprocess.h"


#pragma comment(lib, "..\\Debug\\SIMG.lib")

using namespace Simg;
int main()
{
	Mat m = imread("D:\\0.data\\test.jpg");
	Mat m1 = m.convertTo(SIMG_METHOD_CONVERT_RGB2GRAY_STANDARD);
	Mat m2;
	Mat kernel(3, 3, SIMG_1C8U);
	dilate(m1, m2, kernel);
	imshow("rgb", m);
	imshow("dilate", m2 - 100);
	waitKey();
	
	return 0;
	
}

