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
	//Threshold & Erode & Dilate & waitKey test
	Mat m = imread("..\\testImage\\lena-color.bmp");
	
	m = m.convertTo(SIMG_METHOD_CONVERT_RGB2GRAY_AVERAGE);
	m = m.convertTo(SIMG_METHOD_CONVERT_1C16S);
	imshow("ori-gray", m);
	waitKey(0);

	//Mat test_conv2_fast_bmp = imread("..\\testImage\\lena-color.bmp");
	//test_conv2_fast_bmp = test_conv2_fast_bmp.convertTo(SIMG_METHOD_CONVERT_RGB2GRAY_STANDARD);
	//imshow("ori", test_conv2_fast_bmp);
	//Mat dst;
	//Mat kernelX(3, 3, SIMG_1C8S);
	//kernelX.setPixel(1, 1, 1);
	///*kernelX.setPixel(0, 0, -3); kernelX.setPixel(1, 0, 0); kernelX.setPixel(2, 0, 3);
	//kernelX.setPixel(0, 1, -10); kernelX.setPixel(1, 1, 0); kernelX.setPixel(2, 1, 10);
	//kernelX.setPixel(0, 2, -3); kernelX.setPixel(1, 2, 0); kernelX.setPixel(2, 2, 3);*/

	//clock_t c1 = clock();
	//for (size_t i = 0; i < 100; i++)
	//{
	//	conv2(test_conv2_fast_bmp, dst, kernelX);
	//}
	//
	//clock_t c2 = clock();
	//printf("time = %d ms\n", (c2 - c1)/100);
	//imshow("sobel x", dst);
	//waitKey(0);
	//return 0;
	////Sobel test
	//Mat test_sobel_x, test_sobel_y, test_sobel_xy;
	//Mat test_sobel_bmp = imread("..\\testImage\\lena-gray.bmp");
	//Sobel(test_sobel_bmp, test_sobel_x, SIMG_METHOD_SOBEL_X);
	//Sobel(test_sobel_bmp, test_sobel_y, SIMG_METHOD_SOBEL_Y);
	//Sobel(test_sobel_bmp, test_sobel_xy, SIMG_METHOD_SOBEL_XY);

	//imshow("ori",test_sobel_bmp);
	//imshow("sobel x", test_sobel_x);
	//imshow("sobel y", test_sobel_y);
	//imshow("sobel xy", test_sobel_xy);
	//waitKey(0);

	
	/////following are test code for SIMG project.
	////Gaussian test
	//Mat test_gaussian_bmp = imread("..\\testImage\\lena-gray.bmp");
	//Mat test_gaussian_dst;
	//for (size_t i = 1; i < 10; i++)
	//{
	//	Gaussian(test_gaussian_bmp, test_gaussian_dst, 5, i);
	//	imshow("gaussian", test_gaussian_dst);
	//	waitKey(0);
	//}
	
	
	

	////conv2 test
	//Mat test_conv_kernel(5, 5, SIMG_1C32F);
	//test_conv_kernel.setPixel(0, 0, 0.075);
	//test_conv_kernel.setPixel(0, 1, 0.124);
	//test_conv_kernel.setPixel(0, 2, 0.075);
	//test_conv_kernel.setPixel(1, 0, 0.124);
	//test_conv_kernel.setPixel(1, 2, 0.124);
	//test_conv_kernel.setPixel(2, 0, 0.075);
	//test_conv_kernel.setPixel(2, 1, 0.124);
	//test_conv_kernel.setPixel(2, 2, 0.075);

	//test_conv_kernel.setPixel(1, 1, 0.204);

	//Mat test_conv_gray = imread("..\\testImage\\lena-gray.bmp");
	//Mat test_conv;
	//clock_t test_conv_c1 = clock();
	//for (size_t i = 0; i < 100; i++)
	//{
	//	conv2(test_conv_gray, test_conv, test_conv_kernel);
	//}
	//
	//clock_t test_conv_c2 = clock();
	//imshow("conv2", test_conv);
	//imshow("ori", test_conv_gray);
	//printf("time = %d ms", (test_conv_c2 - test_conv_c1)/100);
	//waitKey(0);

	//
	////float max imshow test
	//Mat test_imshow_f(1024, 768, SIMG_1C32F), test_imshow_f3(1024, 768, SIMG_3C32F);
	//
	//for (int i = 0; i < 256; i ++)
	//{
	//
	//	test_imshow_f.setTo(i*10.0);
	//	test_imshow_f.setPixel(10, 10, 100);
	//	test_imshow_f.setPixel(10, 9, 90);
	//	test_imshow_f.setPixel(10, 8, 80);
	//	test_imshow_f.setPixel(10, 7, 70);
	//	test_imshow_f.setPixel(10, 5, 60);
	//	test_imshow_f.setPixel(10, 10, 1);

	//	test_imshow_f3.setTo(i*10.0,0.0,0.0);
	//	test_imshow_f3.setPixel(10, 10, 100,100,0);
	//	test_imshow_f3.setPixel(10, 9, 90, 100, 0);
	//	test_imshow_f3.setPixel(10, 8, 80, 100, 0);
	//	test_imshow_f3.setPixel(10, 7, 70, 100, 0);
	//	test_imshow_f3.setPixel(10, 5, 60, 100, 0);
	//	test_imshow_f3.setPixel(10, 10, 1, 100, 0);
	//	//imshow("1C32F", test_imshow_f);
	//	imshow("3C32F", test_imshow_f3);
	//	printf("%d\n", i * 10);
	//	waitKey(30);
	//}

	//

	////Mat getMax() getMin()test
	//Mat m_getMax = imread("..\\testImage\\lena-color.bmp");
	//Mat m_test_getMax(1024, 768, SIMG_1C32F);
	//m_test_getMax.setPixel(100, 100, 300);
	//m_test_getMax.setPixel(10, 10, -300.1);
	//vector<uchar> rgbmax;
	//vector<float> floatmax;
	//m_getMax.getMax(rgbmax);
	//m_test_getMax.getMax(floatmax);

	//
	//

	////Mat add algorithm test
	//Mat test_add(1024, 768, SIMG_1C8U), test_add_1(1024, 768, SIMG_1C8U);
	//test_add.setTo(100);
	//test_add_1.setTo(10);

	//Mat test_add_f(1024, 768, SIMG_1C32F), test_add_f1(1024, 768, SIMG_1C32F);
	//test_add_f.setTo(100);
	//test_add_f1.setTo(10);
	//test_add_f = test_add_f - test_add_f1;
	//for (int i = 0; i < 256; i+=5)
	//{
	//	test_add = test_add - test_add_1;
	//	imshow("add", test_add);
	//	waitKey(30);
	//}
	//
	//

	////setTo test
	//Mat test_m(1024, 768, SIMG_3C8U);
	//for (int i = 0; i < 256; i++)
	//{
	//	test_m.setTo(0, 0, i);
	//	imshow("setTo", test_m);
	//	printf("%d\n", i);
	//	waitKey(30);
	//}


	////bmp read test
	//Mat m_bmp_color = imread("..\\testImage\\lena-color.bmp");
	//imshow("color bmp read", m_bmp_color);

	////gray bmp read test
	//Mat m_bmp_gray = imread("..\\testImage\\lena-gray.bmp");
	//imshow("gray bmp read", m_bmp_gray);

	////convertTo test
	//m_bmp_gray = m_bmp_color.convertTo(SIMG_METHOD_CONVERT_RGB2GRAY_STANDARD);
	//imshow("convertTo gray", m_bmp_gray);

	////Threshold and Animation test
	//namedWindow("threshold animation",SIMG_WINDOW_STYLE_STRETCH);
	//for (size_t i = 0; i < 255; i+=10)
	//{
	//	Mat thr;
	//	threshold(m_bmp_gray, thr, i, SIMG_METHOD_THRESHOLD_BINARY, 255);
	//	imshow("threshold animation", thr);
	//	waitKey(30);
	//}

	////Add/Sub test
	//namedWindow("image sub", SIMG_WINDOW_STYLE_STRETCH);
	//Mat bmp_add = m_bmp_gray + 20;
	//imshow("image add", bmp_add);
	//
	////Threshold & Erode & Dilate & waitKey test
	//namedWindow("Morph test animation", SIMG_WINDOW_STYLE_STRETCH);
	//Mat kernel = getMorphStructor(3, 3, SIMG_MORPH_RECTANGLE);
	//int thresh = 100;
	//bool flag = true;	
	//while (flag)
	//{
	//	clock_t c1 = clock();
	//	Mat thr, m_open(3,3,SIMG_1C8U);
	//	threshold(m_bmp_gray, thr, thresh, SIMG_METHOD_THRESHOLD_BINARY, 255);
	//	erode(thr, m_open, kernel);
	//	dilate(m_open, m_open, kernel);
	//	clock_t c2 = clock();
	//	imshow("open animation", m_open);
	//	printf("time = %d\n", c2 - c1);
	//	char c = waitKey(100);
	//	switch (c)
	//	{
	//	case 'W':
	//		thresh += 5;
	//		printf("thresh = %d\n", thresh);
	//		break;
	//	case 'S':
	//		thresh -= 5;
	//		printf("thresh = %d\n", thresh);
	//		break;
	//	case 27:
	//		flag = false;
	//		break;
	//	default:
	//		break;
	//	}
	//}


	//waitKey(0);
	//
	//return 0;
	
}

