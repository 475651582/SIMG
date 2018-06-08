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
	Mat m1 = m.convertTo(SIMG_METHOD_CONVERT_RGB2LAB_STANDARD);
	Mat m2[3];
	split(m1, m2);
	imshow("rgb", m);
	imshow("lab-l", m2[0]);
	imshow("lab-a", m2[1]);
	imshow("lab-b", m2[2]);
	waitKey();
	
	return 0;
	
}

