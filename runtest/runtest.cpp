// runtest.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "../SIMG/simg.h"


#pragma comment(lib, "..\\Debug\\SIMG.lib")

using namespace Simg;
int main()
{
	Mat m = imread("D:\\0.data\\test.bmp");
	namedWindow("haha");
	namedWindow("hahaa");
	waitKey();
	/*for (int n = 0; n < 100; n++)
	{
		int x = 0, y = 0;
		scanf("%d,%d", &x, &y);
		uchar* row_ind = m.row(y);
		int b = row_ind[3 * x];
		int g = row_ind[3 * x + 1];
		int r = row_ind[3 * x + 2];
		printf("x=%d,y=%d, RGB=(%d,%d,%d)\n", x, y, r, g, b);
	}*/

	//getchar();
	return 0;
	
}

