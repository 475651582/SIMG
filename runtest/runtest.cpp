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

	 
	Mat m = imread("..\\testImage\\scene.jpg");
	resize(m, m, Size(1024, 768), SIMG_METHOD_RESIZE_LINEAR_FAST);

	imshow("fast", m);
	waitKey(1000);
	
	return 0;
	
}

