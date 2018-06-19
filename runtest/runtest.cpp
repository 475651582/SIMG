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
	//Mat getMax() getMin()test
	Mat m_getMax = imread("..\\testImage\\lena-color.bmp");
	Mat m_test_getMax(1024, 768, SIMG_1C32F);
	m_test_getMax.setPixel(100, 100, 300);
	m_test_getMax.setPixel(10, 10, -300.1);
	vector<uchar> rgbmax;
	vector<float> floatmax;
	m_getMax.getMax(rgbmax);
	m_test_getMax.getMax(floatmax);

	return 0;
	
	///following are test code for SIMG project.

	//Mat add algorithm test
	Mat test_add(1024, 768, SIMG_1C8U), test_add_1(1024, 768, SIMG_1C8U);
	test_add.setTo(100);
	test_add_1.setTo(10);

	Mat test_add_f(1024, 768, SIMG_1C32F), test_add_f1(1024, 768, SIMG_1C32F);
	test_add_f.setTo(100);
	test_add_f1.setTo(10);
	test_add_f = test_add_f - test_add_f1;
	for (int i = 0; i < 256; i+=5)
	{
		test_add = test_add - test_add_1;
		imshow("add", test_add);
		waitKey(30);
	}
	
	

	//setTo test
	Mat test_m(1024, 768, SIMG_3C8U);
	for (int i = 0; i < 256; i++)
	{
		test_m.setTo(0, 0, i);
		imshow("setTo", test_m);
		waitKey(30);
	}


	//bmp read test
	Mat m_bmp_color = imread("..\\testImage\\lena-color.bmp");
	imshow("color bmp read", m_bmp_color);

	//gray bmp read test
	Mat m_bmp_gray = imread("..\\testImage\\lena-gray.bmp");
	imshow("gray bmp read", m_bmp_gray);

	//convertTo test
	m_bmp_gray = m_bmp_color.convertTo(SIMG_METHOD_CONVERT_RGB2GRAY_STANDARD);
	imshow("convertTo gray", m_bmp_gray);

	//Threshold and Animation test
	namedWindow("threshold animation",SIMG_WINDOW_STYLE_STRETCH);
	for (size_t i = 0; i < 255; i+=10)
	{
		Mat thr;
		threshold(m_bmp_gray, thr, i, SIMG_METHOD_THRESHOLD_BINARY, 255);
		imshow("threshold animation", thr);
		waitKey(30);
	}

	//Add/Sub test
	namedWindow("image sub", SIMG_WINDOW_STYLE_STRETCH);
	Mat bmp_add = m_bmp_gray + 20;
	imshow("image add", bmp_add);
	
	//Threshold & Erode & Dilate & waitKey test
	namedWindow("Morph test animation", SIMG_WINDOW_STYLE_STRETCH);
	Mat kernel = getMorphStructor(3, 3, SIMG_MORPH_RECTANGLE);
	int thresh = 100;
	bool flag = true;	
	while (flag)
	{
		clock_t c1 = clock();
		Mat thr, m_open(3,3,SIMG_1C8U);
		threshold(m_bmp_gray, thr, thresh, SIMG_METHOD_THRESHOLD_BINARY, 255);
		erode(thr, m_open, kernel);
		dilate(m_open, m_open, kernel);
		clock_t c2 = clock();
		imshow("open animation", m_open);
		printf("time = %d\n", c2 - c1);
		char c = waitKey(100);
		switch (c)
		{
		case 'W':
			thresh += 5;
			printf("thresh = %d\n", thresh);
			break;
		case 'S':
			thresh -= 5;
			printf("thresh = %d\n", thresh);
			break;
		case 27:
			flag = false;
			break;
		default:
			break;
		}
	}


	waitKey(0);
	
	return 0;
	
}

