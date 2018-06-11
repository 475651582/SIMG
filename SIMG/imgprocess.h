#pragma once
#include "simgMat.h"


namespace Simg
{
	const int SIMG_METHOD_CONVERT_RGB2GRAY_STANDARD = 0x00000001;
	const int SIMG_METHOD_CONVERT_RGB2GRAY_AVERAGE = 0x00000002;
	const int SIMG_METHOD_CONVERT_RGB2LAB_STANDARD = 0x00000011;
	const int SIMG_MORPH_RECTANGLE = 0x00010001;
	const int SIMG_MORPH_DIAMOND = 0x00010002;
	const int SIMG_MORPH_CROSS = 0x00010003;
	const int SIMG_MORPH_CIRCLE = 0x00010004;

	void rgb2gray(Mat &src,Mat &dst, int methods = 0);
	void rgb2lab(Mat &src, Mat &dst, int methods = 0);

	void rgb2lab_pixelStandard(uchar r, uchar g, uchar b, uchar &lab_l, uchar &lab_a, uchar &lab_b);
	void dilate(Mat &src, Mat &dst, Mat kernel);
	void erode(Mat &src, Mat &dst, Mat kernel);
	Mat getMorphStructor(int cols, int rows, int structorType = SIMG_MORPH_RECTANGLE);
}