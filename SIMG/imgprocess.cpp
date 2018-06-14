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

	Mat _src = src;
	dst = Mat(_src.cols(), _src.rows(), SIMG_1C8U);
	uchar *srcBuffer = _src.dataPtr();
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
	Mat _src = src;
	dst = Mat(_src.cols(), _src.rows(), SIMG_3C8U);
	uchar *srcBuffer = _src.dataPtr();
	uchar *dstBuffer = dst.dataPtr();
	uchar b = 0, g = 0, r = 0, lab_l = 0, lab_a = 0, lab_b = 0;
	for (int i = 0; i < _src.cols() * _src.rows(); i++)
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
		int threshold = threshValue;

		switch (method)
		{
		case SIMG_METHOD_THRESHOLD_BINARY:
			for (int i = 0; i < src.cols()*src.rows(); i++)
			{
				dstBuffer[i] = srcBuffer[i] > threshold ? value : 0;
			}
			return threshold;
			break;
		case SIMG_METHOD_THRESHOLD_BINARY_INV:
			for (int i = 0; i < src.cols()*src.rows(); i++)
			{
				dstBuffer[i] = srcBuffer[i] < threshold ? value : 0;
			}
			return threshold;
			break;
		case SIMG_METHOD_THRESHOLD_OTSU:
		{
			Histogram h(1);
			calcHistogram(src, h);
			threshold = h.OtsuThreshold();
			for (int i = 0; i < src.cols()*src.rows(); i++)
			{
				dstBuffer[i] = srcBuffer[i] > threshold ? value : 0;
			}
			return threshold;
			break;
		}
			
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

void Simg::calcHistogram(Mat &src, Histogram &h)
{
	assert(!src.isEmpty() && !h.isEmpty());
	uchar *srcBuffer = src.dataPtr();

	switch (src.channels())
	{
	case 1:
	{
		h = Histogram(1, src.cols(), src.rows());
		int *hBuffer = h.dataPtr();
		for (int i = 0; i < src.cols()*src.rows(); i++)
		{
			uchar p = srcBuffer[i];
			hBuffer[p]++;
		}
		break;
	}
		
	case 3:
	{
		h = Histogram(3, src.cols(), src.rows());
		int *hBuffer = h.dataPtr();
		for (int i = 0; i < src.cols()*src.rows(); i++)
		{
			uchar b = srcBuffer[3 * i];
			uchar g = srcBuffer[3 * i + 1];
			uchar r = srcBuffer[3 * i + 2];
			hBuffer[b]++;
			hBuffer[g + 256]++;
			hBuffer[r + 256 * 2]++;
		}
		break;
	}
		
	default:
		break;
	}
	
	
}



Histogram::Histogram(int channel, int col, int row)
{
	assert(channel == 1 || channel == 3);
	_channels = channel;

	switch (_channels)
	{
	case 1:
		_dataLength = 256;

		break;
	case 3:
		_dataLength = 256 * 3;
		break;
	default:
		_dataLength = 256;
		break;
	}

	_cols = col; _rows = row;
	_dataPtr = new int[_dataLength];
	memset(_dataPtr, 0, sizeof(int)*_dataLength);
	_pcount = new size_t(1);
}

Histogram::~Histogram()
{
	if (--*_pcount == 0 && NULL != _dataPtr)
	{
		_cols = 0;
		_rows = 0;
		delete _dataPtr;
		delete _pcount;
		_dataPtr = NULL;
		_pcount = NULL;
	}
}

float Simg::Histogram::average()
{
	float mean = 0;
	switch (_channels)
	{
	case 1:
		for (size_t i = 0; i < 256; i++)
		{
			mean += i * _dataPtr[i];
		}
		mean /= (_cols * _rows);
		break;
	case 3:
		assert(false && "average for 3 channels is not available currently");
	default:
		break;
	}
	
	return mean;
}

int Simg::Histogram::OtsuThreshold()
{
	assert(_channels == 1);
	float mean = average();
	float maxVariance = 0;
	float w0 = 0, avgValue = 0;
	float size = (float)_cols * _rows;
	int threshold = 0;
	for (int i = 0; i < 256; i++)
	{
		w0 += _dataPtr[i] / size;
		avgValue += (i * _dataPtr[i] / size);

		float t = avgValue / w0 - mean;
		float v = t * t * w0 / (1 - w0);
		if (v > maxVariance)
		{
			maxVariance = v;
			threshold = i;
		}
	}
	return threshold;
}

int * Simg::Histogram::dataPtr()
{
	if (*_pcount > 1)
	{
		*_pcount--;
		//allocate new ptr and memory for dst mat
		size_t* tmpPcount = new size_t(1);
		int* tmpDataPtr = new int[_dataLength];
		memcpy(tmpDataPtr, _dataPtr, _dataLength);
		_dataPtr = tmpDataPtr;
		_pcount = tmpPcount;
	}

	return _dataPtr;
}

void Simg::resize(Mat & src, Mat & dst, Size dsize, int method)
{
	//assert(src.channels() == 1); //consider single channel first
	Mat _src = src;	//make a shallow copy in case of the situation of src=dst

	dst = Mat(dsize.x, dsize.y, src.datatype());
	

	uchar* srcBuffer = _src.dataPtr();
	uchar* dstBuffer = dst.dataPtr();
	switch (method)
	{
	case SIMG_METHOD_RESIZE_NEAREST:
		resize_nearest_neighbor_sample(srcBuffer, _src.cols(), _src.rows(), dstBuffer, dst.cols(), dst.rows(), src.channels());
		break;
	case SIMG_METHOD_RESIZE_LINEAR:
		resize_linear_sample(srcBuffer, _src.cols(), _src.rows(), dstBuffer, dst.cols(), dst.rows(), src.channels());
		break;
	case SIMG_METHOD_RESIZE_LINEAR_FAST:
		resize_linear_sample_fast(srcBuffer, _src.cols(), _src.rows(), dstBuffer, dst.cols(), dst.rows(), src.channels());
		break;
	default:
		break;
	}
}

void Simg::resize_nearest_neighbor_sample(uchar * srcBuffer, int srcCols, int srcRows, uchar * dstBuffer, int dstCols, int dstRows, int channels)
{
	float ratioCols = 1.0f * srcCols / dstCols;
	float ratioRows = 1.0f * srcRows / dstRows;
	for (int i = 0; i < dstCols * dstRows; i++)
	{
		int x = i % dstCols;
		int y = i / dstCols;
		int srcX = (int)round(x * ratioCols);
		int srcY = (int)round(y * ratioRows);
		if (1 == channels)
		{
			dstBuffer[i] = srcBuffer[srcX + srcY * srcCols];
		}
		else
		{
			dstBuffer[channels * i] = srcBuffer[channels * srcX + channels * srcY * srcCols];
			dstBuffer[channels * i + 1] = srcBuffer[channels * srcX + channels * srcY * srcCols + 1];
			dstBuffer[channels * i + 2] = srcBuffer[channels * srcX + channels * srcY * srcCols + 2];
		}
		
	}
	
}


void Simg::resize_linear_sample(uchar * srcBuffer, int srcCols, int srcRows, uchar * dstBuffer, int dstCols, int dstRows, int channels)
{
	float ratioCols = 1.0f * srcCols / dstCols;
	float ratioRows = 1.0f * srcRows / dstRows;
	for (int n = 0; n < dstCols * dstRows; n++)
	{
		int x = n % dstCols;
		int y = n / dstCols;
		
		
		float preciseX = (x + 0.5f) * ratioCols - 0.5f;
		float preciseY = (y + 0.5f) * ratioRows - 0.5f;;
		int i = MIN(MAX((int)floor(preciseX), 0), srcCols - 1);
		int j = MIN(MAX((int)floor(preciseY), 0), srcRows - 1);
		float u = preciseX - i;
		float v = preciseY - j;
		
		
		if (1 == channels)
		{
			float f_i_j = srcBuffer[i + j * srcCols];
			float f_i1_j = srcBuffer[i + 1 + j * srcCols];
			float f_i_j1 = srcBuffer[i + j * srcCols + srcCols];
			float f_i1_j1 = srcBuffer[i + 1 + j * srcCols + srcCols];

			float f_u_v = (1 - u)*(1 - v)*f_i_j + (1 - u)*v*f_i_j1 + u * (1 - v)*f_i1_j + u * v*f_i1_j1;
			dstBuffer[n] = (uchar)round(f_u_v);
		}
		else
		{
			float f_i_j_b = srcBuffer[(i + j * srcCols)*channels];
			float f_i1_j_b = srcBuffer[(i + 1 + j * srcCols)*channels];
			float f_i_j1_b = srcBuffer[(i + j * srcCols + srcCols)*channels];
			float f_i1_j1_b = srcBuffer[(i + 1 + j * srcCols + srcCols)*channels];
			float f_u_v_b = (1 - u)*(1 - v)*f_i_j_b + (1 - u)*v*f_i_j1_b + u * (1 - v)*f_i1_j_b + u * v*f_i1_j1_b;

			float f_i_j_g = srcBuffer[(i + j * srcCols)*channels + 1];
			float f_i1_j_g = srcBuffer[(i + 1 + j * srcCols)*channels + 1];
			float f_i_j1_g = srcBuffer[(i + j * srcCols + srcCols)*channels + 1];
			float f_i1_j1_g = srcBuffer[(i + 1 + j * srcCols + srcCols)*channels + 1];
			float f_u_v_g = (1 - u)*(1 - v)*f_i_j_g + (1 - u)*v*f_i_j1_g + u * (1 - v)*f_i1_j_g + u * v*f_i1_j1_g;

			float f_i_j_r = srcBuffer[(i + j * srcCols)*channels + 2];
			float f_i1_j_r = srcBuffer[(i + 1 + j * srcCols)*channels + 2];
			float f_i_j1_r = srcBuffer[(i + j * srcCols + srcCols)*channels + 2];
			float f_i1_j1_r = srcBuffer[(i + 1 + j * srcCols + srcCols)*channels + 2];
			float f_u_v_r = (1 - u)*(1 - v)*f_i_j_r + (1 - u)*v*f_i_j1_r + u * (1 - v)*f_i1_j_r + u * v*f_i1_j1_r;

			dstBuffer[channels * n] = (uchar)round(f_u_v_b);
			dstBuffer[channels * n + 1] = (uchar)round(f_u_v_g);
			dstBuffer[channels * n + 2] = (uchar)round(f_u_v_r);
		}
		
	}

}

void Simg::resize_linear_sample_fast(uchar * srcBuffer, int srcCols, int srcRows, uchar * dstBuffer, int dstCols, int dstRows, int channels)
{
	
	for (int n = 0; n < dstCols * dstRows; n++)
	{
		int x = n % dstCols;
		int y = n / dstCols;


		
		int preciseX_int = (x << 11) * srcCols / dstCols + (srcCols << 10) / dstCols - 1024;
		int preciseY_int = (y << 11) * srcRows / dstRows + (srcRows << 10) / dstRows - 1024;

	
		int i = MIN(MAX((preciseX_int>>11), 0), srcCols - 1);
		
		int j = MIN(MAX((preciseY_int>>11), 0), srcRows - 1);
	
		int u = preciseX_int - (i << 11);
		int v = preciseY_int - (j << 11);

	
		if (1 == channels)
		{
			int f_i_j = srcBuffer[i + j * srcCols];
			int f_i1_j = srcBuffer[i + 1 + j * srcCols];
			int f_i_j1 = srcBuffer[i + j * srcCols + srcCols];
			int f_i1_j1 = srcBuffer[i + 1 + j * srcCols + srcCols];
			int f_u_v = ((2048 - u)*(2048 - v)*f_i_j + (2048 - u)*v*f_i_j1 + u * (2048 - v)*f_i1_j + u * v*f_i1_j1) >> 22;
			dstBuffer[n] = (uchar)f_u_v;
		}
		else
		{
			int f_i_j_b = srcBuffer[(i + j * srcCols)*channels];
			int f_i1_j_b = srcBuffer[(i + 1 + j * srcCols)*channels];
			int f_i_j1_b = srcBuffer[(i + j * srcCols + srcCols)*channels];
			int f_i1_j1_b = srcBuffer[(i + 1 + j * srcCols + srcCols)*channels];
			int f_u_v_b = ((2048 - u)*(2048 - v)*f_i_j_b + (2048 - u)*v*f_i_j1_b + u * (2048 - v)*f_i1_j_b + u * v*f_i1_j1_b) >> 22;

			int f_i_j_g = srcBuffer[(i + j * srcCols)*channels+1];
			int f_i1_j_g = srcBuffer[(i + 1 + j * srcCols)*channels + 1];
			int f_i_j1_g = srcBuffer[(i + j * srcCols + srcCols)*channels + 1];
			int f_i1_j1_g = srcBuffer[(i + 1 + j * srcCols + srcCols)*channels + 1];
			int f_u_v_g = ((2048 - u)*(2048 - v)*f_i_j_g + (2048 - u)*v*f_i_j1_g + u * (2048 - v)*f_i1_j_g + u * v*f_i1_j1_g) >> 22;

			int f_i_j_r = srcBuffer[(i + j * srcCols)*channels+2];
			int f_i1_j_r = srcBuffer[(i + 1 + j * srcCols)*channels + 2];
			int f_i_j1_r = srcBuffer[(i + j * srcCols + srcCols)*channels + 2];
			int f_i1_j1_r = srcBuffer[(i + 1 + j * srcCols + srcCols)*channels + 2];
			int f_u_v_r = ((2048 - u)*(2048 - v)*f_i_j_r + (2048 - u)*v*f_i_j1_r + u * (2048 - v)*f_i1_j_r + u * v*f_i1_j1_r) >> 22;

			dstBuffer[channels*n] = (uchar)f_u_v_b;
			dstBuffer[channels*n + 1] = (uchar)f_u_v_g;
			dstBuffer[channels*n + 2] = (uchar)f_u_v_r;
		}

		
	}

}