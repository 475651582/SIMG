// runtest.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "../SIMG/simg.h"


#pragma comment(lib, "..\\Debug\\SIMG.lib")

using namespace Simg;
int main()
{
	Mat m = imread("D:\\0.data\\test.bmp");
	Mat m2 = imread("D:\\0.data\\test2.bmp");
	//namedWindow("a");
	imshow("a", m);
	imshow("b", m2);
	waitKey();
	
	return 0;
	
}

