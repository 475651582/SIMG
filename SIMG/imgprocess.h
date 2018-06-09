#pragma once
#include "simgMat.h"


namespace Simg
{
	const int SIMG_METHOD_CONVERT_RGB2GRAY_STANDARD = 0x00000001;
	const int SIMG_METHOD_CONVERT_RGB2GRAY_AVERAGE = 0x00000002;
	const int SIMG_METHOD_CONVERT_RGB2LAB_STANDARD = 0x00000011;

	void rgb2gray(Mat &src,Mat &dst, int methods = 0);
	void rgb2lab(Mat &src, Mat &dst, int methods = 0);

	void rgb2lab_pixel_standard(uchar r, uchar g, uchar b, uchar &lab_l, uchar &lab_a, uchar &lab_b);
	void dilate(Mat &src, Mat &dst, Mat kernel);
	void erode(Mat &src, Mat &dst, Mat kernel);
}