#include "stdafx.h"
#include "imgprocess.h"

using namespace Simg;
inline float gamma(float x)
{
	return x>0.04045 ? pow((x + 0.055f) / 1.055f, 2.4f) : x / 12.92;
};
void convertMorphyKernel(Mat Kernel, Mat src, int *directArray, int &directNum)
{
	directArray[0] = 0;
	directArray[1] = -1;
	directArray[2] = 1;
	directArray[3] = -src.cols();
	directArray[4] = src.cols();
	directArray[5] = 1 - src.cols();
	directArray[6] = 1 + src.cols();
	directArray[7] = -1 - src.cols();
	directArray[8] = -1 + src.cols();
	directNum = 9;
}

void Simg::rgb2gray(Mat &src, Mat &dst, int methods)
{
	
	assert(!src.isEmpty() && src.channels() == 3);

	dst = Mat(src.cols(), src.rows(), SIMG_1C8U);
	uchar *src_buffer = src.dataPtr();
	uchar *dst_buffer = dst.dataPtr();

	float weight_b = 0, weight_g = 0, weight_r = 0;

	switch (methods)
	{
	case SIMG_METHOD_CONVERT_RGB2GRAY_STANDARD:
		// Rec.ITU-R BT.601-7 
		weight_b = 0.1140; weight_g = 0.5870; weight_r = 0.2989;
		break;
	case SIMG_METHOD_CONVERT_RGB2GRAY_AVERAGE:
		// Averaging the weight of each channel
		weight_b = 0.3333; weight_g = 0.3333; weight_r = 0.3333;
		break;
	default:
		// Averaging the weight of each channel
		weight_b = 0.3333; weight_g = 0.3333; weight_r = 0.3333;
		break;
	}
	for (size_t i = 0; i < src.cols() * src.rows(); i++)
	{
		uchar b = src_buffer[3 * i];
		uchar g = src_buffer[3 * i + 1];
		uchar r = src_buffer[3 * i + 2];
		uchar gray = (uchar)(b * weight_b + g * weight_g + r * weight_r);
		dst_buffer[i] = gray;
	}
}

void Simg::rgb2lab(Mat & src, Mat & dst, int methods)
{
	assert(!src.isEmpty() && src.channels() == 3);
	dst = Mat(src.cols(), src.rows(), SIMG_3C8U);
	uchar *src_buffer = src.dataPtr();
	uchar *dst_buffer = dst.dataPtr();
	uchar b = 0, g = 0, r = 0, lab_l = 0, lab_a = 0, lab_b = 0;
	for (size_t i = 0; i < src.cols() * src.rows(); i++)
	{
		b = src_buffer[3 * i];
		g = src_buffer[3 * i + 1];
		r = src_buffer[3 * i + 2];

		rgb2lab_pixel_standard(r, g, b, lab_l, lab_a, lab_b);

		dst_buffer[3 * i] = lab_l;
		dst_buffer[3 * i + 1] = lab_a;
		dst_buffer[3 * i + 2] = lab_b;
	}
}

void Simg::rgb2lab_pixel_standard(uchar r, uchar g, uchar b, uchar & lab_l, uchar & lab_a, uchar & lab_b)
{
	float B = gamma(b / 255.0f);
	float G = gamma(g / 255.0f);
	float R = gamma(r / 255.0f);
	float X = 0.412453*R + 0.357580*G + 0.180423*B;
	float Y = 0.212671*R + 0.715160*G + 0.072169*B;
	float Z = 0.019334*R + 0.119193*G + 0.950227*B;

	X /= 0.95047;
	Y /= 1.0;
	Z /= 1.08883;

	float FX = X > 0.008856f ? pow(X, 1.0f / 3.0f) : (7.787f * X + 0.137931f);
	float FY = Y > 0.008856f ? pow(Y, 1.0f / 3.0f) : (7.787f * Y + 0.137931f);
	float FZ = Z > 0.008856f ? pow(Z, 1.0f / 3.0f) : (7.787f * Z + 0.137931f);
	lab_l = (uchar)(Y > 0.008856f ? (116.0f * FY - 16.0f) : (903.3f * Y));
	lab_a = (uchar)(500.f * (FX - FY) + 127) ;
	lab_b = (uchar)(200.f * (FY - FZ) + 127) ;
}


void Simg::dilate(Mat &src, Mat &dst, Mat kernel)
{
	assert(!src.isEmpty() && !kernel.isEmpty());
	dst = src;

	if (1 == src.channels()) //consider single channel image first.
	{
		int directArray[9];
		uchar *src_buffer = src.dataPtr();
		uchar *dst_buffer = dst.dataPtr();

		int x = 0, y = 0, directNum = 0;
		convertMorphyKernel(kernel, src, directArray, directNum);
		for (size_t i = 0; i < src.cols()*src.rows(); i++)
		{
			x = i % src.cols();
			y = i / src.cols();
			
			if (0 == x || 0 == y || x > src.cols() - 1 || y > src.rows() - 1)  continue;  //no boundary first
			uchar max_neighbor = 0;
			for (size_t j = 0; j < directNum; j++)
			{
				uchar neighbor = src_buffer[i + directArray[j]];
				max_neighbor = MAX(neighbor, max_neighbor);
			}
			dst_buffer[i] = max_neighbor;
		}
	}
}