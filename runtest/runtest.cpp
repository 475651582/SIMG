// runtest.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "../SIMG/simg.h"


#pragma comment(lib, "..\\Debug\\SIMG.lib")

using namespace Simg;
int main()
{
	//Mat m = imread("D:\\0.data\\test-gray.bmp");
	Mat m = imread("D:\\0.data\\test-gray.jpg");



	imshow("m", m);
	waitKey();
	
	return 0;
	
}

