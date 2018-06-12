#pragma once
#include <Windows.h>
#include <time.h>
#include "simgMat.h"
#include "simgWindow.h"
#include "jpgd.h"

namespace Simg
{
	
	Mat imread(const char *path);
	Mat readBMP(const char *path);
	Mat readJPG(const char *path);
	void split(Mat src, Mat* dst);
	int namedWindow(const char *windowName, int windowStyle = 0, int x =0, int y = 0, int w = 256, int h = 256);
	int imshow(const char *windowName, Mat img, int windowStyle = SIMG_WINDOW_STYLE_NORMAL);
	int waitKey(int time = 0);

	
	
}

