/*******************************************************************
*
*	Author: Felix Shang
*	Company: [personal]
*	Date: 2018.06.07
*	Brief: realized some basic image processing algorithm based on 
*	Mat class
*
******************************************************************/

#pragma once
#include "simgMat.h"


namespace Simg
{
	
	class Histogram
	{
	public:
		Histogram(int channel, int col = 0, int row = 0);
		~Histogram();
		Histogram & operator = (const Histogram& h)
		{
			if (this == &h)
				return *this;
			if (--*_pcount == 0)
			{
				_cols = 0;
				_rows = 0;
				_channels = 0;
				_dataLength = 0;

				delete _dataPtr;
				delete _pcount;
				_dataPtr = NULL;
				_pcount = NULL;
			}

			this->_cols = h._cols;
			this->_rows = h._rows;
			this->_channels = h._channels;
			this->_dataPtr = h._dataPtr;
			this->_pcount = h._pcount;
			this->_dataLength = h._dataLength;

			++*h._pcount;

			return *this;
		};

		Histogram(const Histogram& h)
		{
			if (this == &h)
				return;
			_pcount = h._pcount;
			_cols = h._cols;
			_rows = h._rows;
			_channels = h._channels;
			_dataPtr = h._dataPtr;
			_dataLength = h._dataLength;
			++*_pcount;
		}

		float average();
		int OtsuThreshold();
		int* dataPtr();

		bool isEmpty() { return NULL == _dataPtr; };
	private:
		size_t * _pcount; //copy ref counter
		int _cols, _rows, _channels, _dataLength;
		int *_dataPtr;
	};

	const int SIMG_METHOD_CONVERT_RGB2GRAY_STANDARD = 0x00000111;
	const int SIMG_METHOD_CONVERT_RGB2GRAY_AVERAGE = 0x00000112;
	const int SIMG_METHOD_CONVERT_RGB2LAB_STANDARD = 0x00000113;
	const int SIMG_METHOD_CONVERT_1C8U = SIMG_1C8U;
	const int SIMG_METHOD_CONVERT_3C8U = SIMG_3C8U;
	const int SIMG_METHOD_CONVERT_1C32F = SIMG_1C32F;
	const int SIMG_METHOD_CONVERT_3C32F = SIMG_3C32F;
	const int SIMG_METHOD_CONVERT_1C16S = SIMG_1C16S;

	const int SIMG_MORPH_RECTANGLE = 0x00010001;
	const int SIMG_MORPH_DIAMOND = 0x00010002;
	const int SIMG_MORPH_CROSS = 0x00010003;
	const int SIMG_MORPH_CIRCLE = 0x00010004;
	const int SIMG_METHOD_THRESHOLD_BINARY = 0x00020001;
	const int SIMG_METHOD_THRESHOLD_BINARY_INV = 0x00020002;
	const int SIMG_METHOD_THRESHOLD_OTSU = 0x00020003;
	const int SIMG_METHOD_RESIZE_NEAREST = 0x00030001;
	const int SIMG_METHOD_RESIZE_LINEAR = 0x00030002;
	const int SIMG_METHOD_RESIZE_LINEAR_FAST = 0x00030003;
	const int SIMG_METHOD_SOBEL_X = 0x00040001;
	const int SIMG_METHOD_SOBEL_Y = 0x00040002;
	const int SIMG_METHOD_SOBEL_XY = 0x00040003;
	const float SIMG_PI = 3.1415926f;


	void rgb2gray(Mat &src,Mat &dst, int methods = 0);
	void rgb2lab(Mat &src, Mat &dst, int methods = 0);

	void rgb2lab_pixelStandard(uchar r, uchar g, uchar b, uchar &lab_l, uchar &lab_a, uchar &lab_b);
	void dilate(Mat &src, Mat &dst, Mat kernel);
	void erode(Mat &src, Mat &dst, Mat kernel);

	void conv2(Mat &src, Mat &dst, Mat kernel,int datatype);

	//<deprecated>
	void conv2f(Mat &src, Mat &dst, Mat kernel);

	int threshold(Mat &src, Mat &dst, int threshold, int method = SIMG_METHOD_THRESHOLD_BINARY, int value = 255);
	Mat getMorphStructor(int cols, int rows, int structorType = SIMG_MORPH_RECTANGLE);
	void calcHistogram(Mat &src, Histogram &h);

	void resize(Mat &src, Mat &dst, Size dsize, int method = SIMG_METHOD_RESIZE_NEAREST);
	
	void resize_nearest_neighbor_sample(uchar* srcBuffer, int srcCols, int srcRows, uchar* dstBuffer, int dstCols, int dstRows, int channels);
	void resize_linear_sample(uchar* srcBuffer, int srcCols, int srcRows, uchar* dstBuffer, int dstCols, int dstRows, int channels);
	void resize_linear_sample_fast(uchar* srcBuffer, int srcCols, int srcRows, uchar* dstBuffer, int dstCols, int dstRows, int channels);

	void Gaussian(Mat &src, Mat &dst, int kernelSize, float sigma = 1.0f);
	void Sobel(Mat &src, Mat &dst, int method = SIMG_METHOD_SOBEL_XY);
	void canny(Mat &src, Mat &dst, int highThreshVal, int lowThreshVal);
	Mat mabs(Mat &m1);

}