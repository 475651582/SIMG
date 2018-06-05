#pragma once
#include <Windows.h>
#include "simgMat.h"
#include "simgWindow.h"

namespace Simg
{
	Mat imread(const char *path);
	Mat readBMP(const char *path);
	int namedWindow(const char *windowName, int windowStyle = 0, int x =0, int y = 0, int w = 256, int h = 256);
	int imshow(const char *windowName, Mat img, int windowStyle = 0);
	int waitKey();

	
	
}

