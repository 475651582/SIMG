#include "stdafx.h"
#include "imgprocess.h"

using namespace Simg;
inline float gamma(float x)
{
	return x>0.04045f ? pow((x + 0.055f) / 1.055f, 2.4f) : x / 12.92f;
};
void convertMorphyKernel(Mat Kernel, Mat src, int *directArray, size_t &directNum)
{
	assert(!Kernel.isEmpty() && Kernel.cols() % 2 != 0 && Kernel.rows() % 2 != 0);
	size_t kernelLength = Kernel.cols() * Kernel.rows();
	uchar *kernelBuffer = Kernel.dataPtr();
	int anchorPointX = Kernel.cols() / 2;
	int anchorPointY = Kernel.rows() / 2;
	directNum = 0;
	for (size_t i = 0; i < kernelLength; i++)
	{
		if (kernelBuffer[i] != 0)
		{
			int x = i % Kernel.cols();
			int y = i / Kernel.cols();
			int direct = (x - anchorPointX) + (y - anchorPointY) * src.cols();
			directArray[directNum++] = direct;
		}
	}

	/*directArray[0] = 0;
	directArray[1] = -1;
	directArray[2] = 1;
	directArray[3] = -src.cols();
	directArray[4] = src.cols();
	directArray[5] = 1 - src.cols();
	directArray[6] = 1 + src.cols();
	directArray[7] = -1 - src.cols();
	directArray[8] = -1 + src.cols();
	directNum = 9;*/
}

void Simg::rgb2gray(Mat &src, Mat &dst, int methods)
{
	
	assert(!src.isEmpty() && src.channels() == 3);

	dst = Mat(src.cols(), src.rows(), SIMG_1C8U);
	uchar *srcBuffer = src.dataPtr();
	uchar *dstBuffer = dst.dataPtr();

	float weightB = 0, weightG = 0, weightR = 0;

	switch (methods)
	{
	case SIMG_METHOD_CONVERT_RGB2GRAY_STANDARD:
		// Rec.ITU-R BT.601-7 
		weightB = 0.1140f; weightG = 0.5870f; weightR = 0.2989f;
		break;
	case SIMG_METHOD_CONVERT_RGB2GRAY_AVERAGE:
		// Averaging the weight of each channel
		weightB = 0.3333f; weightG = 0.3333f; weightR = 0.3333f;
		break;
	default:
		// Averaging the weight of each channel
		weightB = 0.3333f; weightG = 0.3333f; weightR = 0.3333f;
		break;
	}
	for (int i = 0; i < src.cols() * src.rows(); i++)
	{
		uchar b = srcBuffer[3 * i];
		uchar g = srcBuffer[3 * i + 1];
		uchar r = srcBuffer[3 * i + 2];
		uchar gray = (uchar)(b * weightB + g * weightG + r * weightR);
		dstBuffer[i] = gray;
	}
}

void Simg::rgb2lab(Mat & src, Mat & dst, int methods)
{
	assert(!src.isEmpty() && src.channels() == 3);
	dst = Mat(src.cols(), src.rows(), SIMG_3C8U);
	uchar *srcBuffer = src.dataPtr();
	uchar *dstBuffer = dst.dataPtr();
	uchar b = 0, g = 0, r = 0, lab_l = 0, lab_a = 0, lab_b = 0;
	for (int i = 0; i < src.cols() * src.rows(); i++)
	{
		b = srcBuffer[3 * i];
		g = srcBuffer[3 * i + 1];
		r = srcBuffer[3 * i + 2];

		rgb2lab_pixelStandard(r, g, b, lab_l, lab_a, lab_b);

		dstBuffer[3 * i] = lab_l;
		dstBuffer[3 * i + 1] = lab_a;
		dstBuffer[3 * i + 2] = lab_b;
	}
}

void Simg::rgb2lab_pixelStandard(uchar r, uchar g, uchar b, uchar & lab_l, uchar & lab_a, uchar & lab_b)
{
	float B = gamma(b / 255.0f);
	float G = gamma(g / 255.0f);
	float R = gamma(r / 255.0f);
	float X = 0.412453f*R + 0.357580f*G + 0.180423f*B;
	float Y = 0.212671f*R + 0.715160f*G + 0.072169f*B;
	float Z = 0.019334f*R + 0.119193f*G + 0.950227f*B;

	X /= 0.95047f;
	Y /= 1.0f;
	Z /= 1.08883f;

	float FX = X > 0.008856f ? pow(X, 1.0f / 3.0f) : (7.787f * X + 0.137931f);
	float FY = Y > 0.008856f ? pow(Y, 1.0f / 3.0f) : (7.787f * Y + 0.137931f);
	float FZ = Z > 0.008856f ? pow(Z, 1.0f / 3.0f) : (7.787f * Z + 0.137931f);
	lab_l = (uchar)(Y > 0.008856f ? (116.0f * FY - 16.0f) : (903.3f * Y));
	lab_a = (uchar)(500.f * (FX - FY) + 127) ;
	lab_b = (uchar)(200.f * (FY - FZ) + 127) ;
}


void Simg::dilate(Mat &src, Mat &dst, Mat kernel)
{
	assert(!src.isEmpty() && !kernel.isEmpty() && src.channels() == 1);
	Mat _src = src;

	if (1 == src.channels()) //consider single channel image first.
	{
		dst = Mat(src.cols(), src.rows(), SIMG_1C8U);
		int *directArray = new int[kernel.cols()*kernel.rows()];
		uchar *srcBuffer = _src.dataPtr();
		uchar *dstBuffer = dst.dataPtr();

		int x = 0, y = 0;
		size_t directNum = 0;
		convertMorphyKernel(kernel, src, directArray, directNum);
		for (int i = 0; i < src.cols()*src.rows(); i++)
		{
			
			uchar max_neighbor = 0;
			for (size_t j = 0; j < directNum; j++)
			{
				int index = i + directArray[j];
				x = index % src.cols();
				y = index / src.cols();
				if (x < 0 || y < 0 || x > src.cols() - 1 || y > src.rows() - 1)  continue;  //no boundary first
				uchar neighbor = srcBuffer[index];
				max_neighbor = MAX(neighbor, max_neighbor);
			}
			dstBuffer[i] = max_neighbor;
		}

		delete directArray; directArray = NULL;
	}
}

void Simg::erode(Mat & src, Mat & dst, Mat kernel)
{
	assert(!src.isEmpty() && !kernel.isEmpty() && src.channels() == 1);
	Mat _src = src;

	if (1 == src.channels()) //consider single channel image first.
	{
		dst = Mat(src.cols(), src.rows(), SIMG_1C8U);
		int *directArray = new int[kernel.cols()*kernel.rows()];		
		uchar *srcBuffer =_src.dataPtr();
		uchar *dstBuffer = dst.dataPtr();

		int x = 0, y = 0;
		size_t directNum = 0;
		convertMorphyKernel(kernel, src, directArray, directNum);
		for (int i = 0; i < src.cols()*src.rows(); i++)
		{

			uchar min_neighbor = 255;
			for (size_t j = 0; j < directNum; j++)
			{
				int index = i + directArray[j];
				x = index % src.cols();
				y = index / src.cols();
				if (x < 0 || y < 0 || x > src.cols() - 1 || y > src.rows() - 1)  continue;  //no boundary first
				uchar neighbor = srcBuffer[index];
				min_neighbor = MIN(neighbor, min_neighbor);
			}
			dstBuffer[i] = min_neighbor;
		}

		delete directArray; directArray = NULL;
	}
}

int Simg::threshold(Mat & src, Mat & dst, int threshValue, int method, int value)
{
	assert(!src.isEmpty() && src.channels() == 1);
	Mat _src = src;
	if (src.channels() == 1)
	{
		dst = Mat(src.cols(), src.rows(), SIMG_1C8U);
		uchar *srcBuffer = _src.dataPtr();
		uchar *dstBuffer = dst.dataPtr();

		switch (method)
		{
		case SIMG_METHOD_THRESHOLD_BINARY:
			for (int i = 0; i < src.cols()*src.rows(); i++)
			{
				dstBuffer[i] = srcBuffer[i] > threshValue ? value : 0;
			}
			return threshValue;
			break;
		case SIMG_METHOD_THRESHOLD_BINARY_INV:
			for (int i = 0; i < src.cols()*src.rows(); i++)
			{
				dstBuffer[i] = srcBuffer[i] < threshValue ? value : 0;
			}
			return threshValue;
			break;
		default:
			break;
		}

		
	}

	return 0;
}

Mat Simg::getMorphStructor(int cols, int rows, int structorType)
{
	Mat ret(cols, rows, SIMG_1C8U);
	int anchorPointX = cols / 2;
	int anchorPointY = rows / 2;
	switch (structorType)
	{
	case SIMG_MORPH_CIRCLE:
	{
		int radius2 = (MIN(cols, rows) / 2) * (MIN(cols, rows) / 2);
		for (int x = 0; x < cols; x++)
			for (int y = 0; y < rows; y++)
			{
				int r2 = (x - anchorPointX)*(x - anchorPointX) + (y - anchorPointY)*(y - anchorPointY);
				if (r2 <= radius2)
				{
					ret.setPixel(x, y, 1);
				}
				else
				{
					ret.setPixel(x, y, 0);
				}
			}
		break;
	}
		
	case SIMG_MORPH_DIAMOND:
		for (int x = 0; x < cols; x++)
		for (int y = 0; y < rows; y++)
		{
			int l2 = abs(x - anchorPointX) + abs(y - anchorPointY);
			if (l2 <= (MIN(cols,rows) / 2))
			{
				ret.setPixel(x, y, 1);
			}
			else
			{
				ret.setPixel(x, y, 0);
			}
		}
		break;
	case SIMG_MORPH_CROSS:
		for (int x = 0; x < cols; x++)
		for (int y = 0; y < rows; y++)
		{
			if (x == (MIN(cols, rows) / 2) || y == (MIN(cols, rows) / 2))
			{
				ret.setPixel(x, y, 1);
			}
			else
			{
				ret.setPixel(x, y, 0);
			}
		}
		break;
	case SIMG_MORPH_RECTANGLE:
		for (int x = 0; x < cols; x++)
		for (int y = 0; y < rows; y++)
		{
			ret.setPixel(x, y, 1);
		}
		break;
	default:
		break;
	}

	return ret;
}
