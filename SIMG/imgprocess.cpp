/*******************************************************************
*
*	Author: Felix Shang
*	Company: [personal]
*	Date: 2018.06.07
*	Brief: realized some basic image processing algorithm based on
*	Mat class
*
******************************************************************/

#include "stdafx.h"
#include "imgprocess.h"

using namespace Simg;
inline float gamma(float x)
{
	return x>0.04045f ? pow((x + 0.055f) / 1.055f, 2.4f) : x / 12.92f;
};

template<typename dtype>
inline dtype fastABS(dtype value)
{
	return (value ^ (value >> 31)) - (value >> 31);
}
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
}

template<typename dtype>
void convertConvKernel(Mat Kernel, Mat src, int *directArray, dtype *valueArray, size_t &directNum)
{
	assert(!Kernel.isEmpty() && Kernel.cols() % 2 != 0 && Kernel.rows() % 2 != 0);

	size_t kernelLength = Kernel.cols() * Kernel.rows();
	dtype *kernelBuffer = (dtype*)Kernel.dataPtr();
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
			directArray[directNum] = direct;
			valueArray[directNum++] = kernelBuffer[i];
		}
	}
}

template<typename dtype>
void convertConvKernel2(Mat Kernel, int srcX, int *directArray, dtype *valueArray, size_t &directNum)
{
	assert(!Kernel.isEmpty() && Kernel.cols() % 2 != 0 && Kernel.rows() % 2 != 0);

	size_t kernelLength = Kernel.cols() * Kernel.rows();
	dtype *kernelBuffer = (dtype*)Kernel.dataPtr();
	int anchorPointX = Kernel.cols() / 2;
	int anchorPointY = Kernel.rows() / 2;
	directNum = 0;
	for (size_t i = 0; i < kernelLength; i++)
	{
		if (kernelBuffer[i] != 0)
		{
			int x = i % Kernel.cols();
			int y = i / Kernel.cols();
			int direct = (x - anchorPointX) + (y - anchorPointY) * (srcX);
			directArray[directNum] = direct;
			valueArray[directNum++] = kernelBuffer[i];
		}
	}

}

template<typename dtype>
dtype* getPaddingMemory(dtype* srcBuffer, int srcCols, int srcRows, int padX, int padY, int &ULpadPtrStarter)
{

	

	int padSrcCols = srcCols + 2 * padX;
	int padSrcRows = srcRows + 2 * padY;
	int padDataLength = padSrcCols * padSrcRows;
	int dataLength = srcCols * srcRows;


	dtype *padSrcBuffer = new dtype[padDataLength];

	memset(padSrcBuffer, 0, padDataLength);

	for (int i = padY; i < padSrcRows - padY; i++)
	{
		int srcY = i - padY;
		memcpy(padSrcBuffer + padX + i * padSrcCols, srcBuffer + srcY * srcCols, srcCols * sizeof(dtype));
	}
	ULpadPtrStarter = padX + padY * padSrcCols;

	return padSrcBuffer;
}

inline short getGradDirect(int x, int y)
{
	if (x == 0)
	{
		return 0;
	}
	if (y == 0)
	{
		return 2;
	}

	int divXY = ((x << 10) / y);
	int divYX = ((y << 10) / x);
	int tan67_5 = 2472;
	if (divXY > tan67_5 || divXY < - tan67_5)
	{
		return 0;
	}
	else if (divYX > tan67_5 || divYX < - tan67_5)
	{
		return 2;
	}
	else if (x * y > 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
	

}

short getGradDirectAccu(int x, int y)
{
	int angle = (short)(atan2(y, x) * 180 / SIMG_PI);
	if (angle < 0)
	{
		angle += 180;
	}
	if (angle >= 0 && angle < 22.5)
	{
		return 0;
	}
	else if (angle >= 22.5 && angle < 67.5)
	{
		return 1;
	}
	else if (angle >= 67.5 && angle < 112.5)
	{
		return 2;
	}
	else if (angle >= 112.5 && angle < 167.5)
	{
		return 3;
	}
	else
	{
		return 0;
	}

}

void getGradDirectIndex(Mat _sx, Mat _sy, Mat &dstN0)
{
	assert(_sx.datatype() == SIMG_1C16S && _sy.datatype() == SIMG_1C16S);
	Mat sx = _sx.copy();
	Mat sy = _sy.copy();

	int cols = sx.cols();
	int rows = sx.rows();

	dstN0 = Mat(cols, rows, SIMG_1C16S);

	short *N0DataPtr = (short*)dstN0.dataPtr();

	short *xDataPtr = (short*)sx.dataPtr();
	short *yDataPtr = (short*)sy.dataPtr();
	//{ 1,1 - gradCols,-gradCols,-1 - gradCols };
	int directArrayN0[] = { -1,1 - cols , -cols, -1 - cols };

	for (int i = 0; i < cols * rows; i++)
	{
		short x = xDataPtr[i];
		short y = yDataPtr[i];
		int direct = getGradDirect(x, y);
		//printf("%d,%d,%d\n", x, y, direct);
		short indexN0 = directArrayN0[direct];
		N0DataPtr[i] = indexN0;
	}
}

Mat nonMaxSuppresion(Mat _grad, Mat _N0)
{
	Mat grad = _grad.copy();
	Mat N0 = _N0.copy();
	int gradCols = grad.cols();
	int gradRows = grad.rows();
	int gradDataLength = gradCols * gradRows;

	Mat ret = Mat(gradCols, gradRows, SIMG_1C16S);

	short* gradBuffer = (short*)grad.dataPtr();
	short* N0Buffer = (short*)N0.dataPtr();
	short* retBuffer = (short*)ret.dataPtr();

	for (int i = 0; i < gradDataLength; i++)
	{
		int x = i % gradCols;
		int y = i / gradCols;
		if (x > 0 && y > 0 && x < gradCols - 2 && y < gradCols - 2)
		{
			short indexN0 = N0Buffer[i];

			short originGrad= gradBuffer[i];
			short N0Grad = gradBuffer[i + indexN0];
			short N1Grad = gradBuffer[i - indexN0];
			

			if (originGrad < N0Grad || originGrad < N1Grad)
			{
				retBuffer[i] = 0;
			}
			else
			{
				retBuffer[i] = originGrad;
			}
		}
	}
	return ret;
}

template<typename dtype>
Mat funcAbs(Mat &m1)
{
	Mat ret(m1.cols(), m1.rows(), m1.datatype());
	Mat src = m1.copy();
	dtype* srcBuffer = (dtype*)src.dataPtr();
	dtype* retBuffer = (dtype*)ret.dataPtr();
	int dataLength = src.cols() * src.rows();
	int channel = ret.channels();
	for (int i = 0; i < dataLength ; i++)
	{
		for (int ch = 0; ch < channel; ch++)
		{
			retBuffer[i*channel + ch] = abs(srcBuffer[i*channel + ch]);
		}
		
	}
	return ret;
}





template<typename dtype>
Mat funcSobelAmp(Mat sx, Mat sy)
{
	Mat ret(sx.cols(), sx.rows(), sx.datatype());
	Mat _sx = sx.copy();
	Mat _sy = sy.copy();
	dtype* _sxBuffer = (dtype*)_sx.dataPtr();
	dtype* _syBuffer = (dtype*)_sy.dataPtr();

	dtype* retBuffer = (dtype*)ret.dataPtr();
	int dataLength = _sx.cols() * _sx.rows();
	int channel = ret.channels();
	for (int i = 0; i < dataLength; i++)
	{
		for (int ch = 0; ch < channel; ch++)
		{
			dtype d1 = _sxBuffer[i*channel + ch];
			dtype d2 = _syBuffer[i*channel + ch];
			retBuffer[i*channel + ch] = (dtype)sqrt(d1 * d1 + d2 * d2);
		}

	}
	return ret;
}

Mat doubleThreshold(Mat _nms, int LT, int HT)
{
	Mat nms = _nms.copy();
	Mat ret(nms.cols(), nms.rows(), SIMG_1C8U);
	int dataLength = ret.cols() *ret.rows();
	short *nmsDataPtr = (short*)nms.dataPtr();
	uchar *retDataPtr = ret.dataPtr();
	for (int i = 0; i < dataLength; i++)
	{
		short nmsData = nmsDataPtr[i];
		if (nmsData > HT)
		{
			retDataPtr[i] = 255;	 //strong boundary
		}
		else if (nmsData < LT)
		{
			retDataPtr[i] = 0;	 //strong boundary
		}
		else
		{
			retDataPtr[i] = 1;	//weak boundary
		}
	}
	return ret;
}

void doubleThresholdLink(Mat &_dt)
{
	Mat dt = _dt.copy();
	int dtCols = dt.cols();
	int dtRows = dt.rows();
	int dataLength = dtCols * dtRows;
	
	uchar* dtPtr = dt.dataPtr();
	for (int i = 0; i < dataLength; i++)
	{
		uchar p = dtPtr[i];
		if (1 == p)	//weak boundard
		{
			int x = i % dtCols;
			int y = i / dtCols;
			if (x < 1 || y < 1 || x > dtCols - 2 || y > dtRows - 2) continue;
			uchar pUL = dtPtr[i - 1 - dtCols];
			uchar pU = dtPtr[i  - dtCols];
			uchar pUR = dtPtr[i + 1 - dtCols];
			uchar pL = dtPtr[i - 1];
			uchar pR = dtPtr[i + 1];
			uchar pD = dtPtr[i + dtCols];
			uchar pDL = dtPtr[i - 1 + dtCols];
			uchar pDR = dtPtr[i + 1 + dtCols];
			if (pUL == 255 || pU == 255 || pUR == 255 || pL == 255 || pR == 255 || pD == 255 || pDL == 255 || pDR == 255)
			{
				dtPtr[i] = 255;
				doubleThresholdLink(dt);
			}
			else
			{
				dtPtr[i] = 0;
			}

		}
		
	}
}

//需要特例化float的类型
template<typename dstType>
Mat funcConv(Mat src, Mat kernel, int datatype)
{
	size_t directNum = 0;

	int padX = kernel.cols();
	int padY = kernel.rows();

	int srcCols = src.cols();
	int srcRows = src.rows();
	int srcDataLength = srcCols * srcRows;

	int ULpadPtrStarter = 0;
	int DRpadPtrEnder = 0;
	Mat padSrc = src.padMat(padX, padY, ULpadPtrStarter, DRpadPtrEnder);
	int padSrcCols = padSrc.cols();
	int padSrcRows = padSrc.rows();
	int padDataLength = padSrc.cols() * padSrc.rows();
	Mat dst = Mat(padSrcCols, padSrcRows, datatype);

	dstType *dstBuffer = (dstType *)dst.dataPtr();
	dstType *padSrcBuffer =(dstType *) padSrc.dataPtr();
	dstType *ULpadPtr =(dstType *) padSrcBuffer + ULpadPtrStarter;

	int *directArray = new int[kernel.cols()*kernel.rows()];
	float *convArray = new float[kernel.cols()*kernel.rows()];
	int *convArrayFast = new int[kernel.cols()*kernel.rows()];
	convertConvKernel2(kernel, padSrcCols, directArray, convArray, directNum);
	for (int i = 0; i < kernel.cols()*kernel.rows(); i++)
	{
		convArrayFast[i] = (int)(convArray[i] * 1024);
	}
	for (int i = 0; i < DRpadPtrEnder; i++)
	{
		int sum = 0;
		int index = 0;
		int neighbor = 0;
		for (size_t j = 0; j < directNum; j++)
		{
			index = i + directArray[j];
			neighbor =(int) ULpadPtr[index];
			sum += neighbor * convArrayFast[j];
		}
		sum = sum >> 10;
		dstBuffer[i] = (dstType)sum;

	}

	dst = dst.ROI(0, 0, srcCols, srcRows);

	delete directArray; directArray = NULL;
	delete convArray; convArray = NULL;
	delete convArrayFast; convArrayFast = NULL;

	return dst;
}


void erodeOrdilate(Mat &src, Mat &dst, Mat kernel, bool erode = true)
{
	assert(!src.isEmpty() && !kernel.isEmpty() && SIMG_1C8U == src.datatype());
	Mat _src = src.copy();

	

	if (1 == src.channels()) //consider single channel image first.
	{
		size_t directNum = 0;
		int padX = kernel.cols();int padY = kernel.rows();
		int srcCols = _src.cols();int srcRows = _src.rows();


		int padSrcCols = srcCols + 2 * padX;
		int padSrcRows = srcRows + 2 * padY;
		int padDataLength = padSrcCols * padSrcRows;
		int dataLength = srcCols * srcRows;

		dst = Mat(padSrcCols, padSrcRows, SIMG_1C8U);
		int *directArray = new int[kernel.cols()*kernel.rows()];
		char *convArray = new char[kernel.cols()*kernel.rows()];
		convertConvKernel2(kernel, padSrcCols, directArray, convArray, directNum);

		uchar *srcBuffer = _src.dataPtr();
		uchar *dstBuffer = dst.dataPtr();

		//pad up memory to avoid boundary check to speed up
		int ULpadPtrStarter = 0;
		uchar* padSrcBuffer = getPaddingMemory(srcBuffer, srcCols, srcRows, padX, padY, ULpadPtrStarter);
		uchar *ULpadPtr = padSrcBuffer + ULpadPtrStarter;

		if (erode)
		{
			for (int i = 0; i < dataLength; i++)
			{				
				uchar min_neighbor = 255;
				uchar neighbor = 0;
				for (size_t j = 0; j < directNum; j++)
				{
					int index = i + directArray[j];
					neighbor = ULpadPtr[index];					
					min_neighbor = MIN(neighbor, min_neighbor);
				}
				dstBuffer[i] = min_neighbor;
			}
		}
		else
		{
			for (int i = 0; i < dataLength; i++)
			{
				uchar max_neighbor = 0;
				uchar neighbor = 0;
				for (size_t j = 0; j < directNum; j++)
				{
					int index = i + directArray[j];
					neighbor = ULpadPtr[index];
					max_neighbor = MAX(neighbor, max_neighbor);
				}
				dstBuffer[i] = max_neighbor;
			}
		}
		

		delete directArray; directArray = NULL;
		delete padSrcBuffer; padSrcBuffer = NULL;
		delete convArray; convArray = NULL;
	}
}


int findSmallestPow2(int x)
{
	int n = 0;
	int m = 1;
	while (true)
	{
		n++;
		m = m << 1;
		if (m >= x)
		{
			return n;
		}
		
	}
}

void Simg::rgb2gray(Mat &src, Mat &dst, int methods)
{
	
	assert(!src.isEmpty() && SIMG_3C8U == src.datatype());

	Mat _src = src.copy();
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
	assert(!src.isEmpty() && SIMG_3C8U == src.datatype());
	Mat _src = src.copy();
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
	erodeOrdilate(src, dst, kernel, false);
}

void Simg::erode(Mat & src, Mat & dst, Mat kernel)
{
	erodeOrdilate(src, dst, kernel, true);
}




void Simg::conv2(Mat & src, Mat & dst, Mat kernel, int datatype)
{	
	assert(!src.isEmpty() && src.channels() == 1);
	assert(src.datatype() == SIMG_1C8U || src.datatype() == SIMG_1C16S || src.datatype() == SIMG_1C32F);
	assert(kernel.datatype() == SIMG_1C32F);
	assert(datatype == SIMG_1C8U || datatype == SIMG_1C16S || datatype == SIMG_1C32F);

	
	
	
	Mat _src = src.convertTo(datatype);

	Mat ret;

	switch (datatype)
	{
	case SIMG_1C8U:
		ret = funcConv<uchar>(_src, kernel, datatype);
		break;
	case SIMG_1C16S:
		ret = funcConv<short>(_src, kernel, datatype);
		break;
	case SIMG_1C32F:
		ret = funcConv<float>(_src, kernel, datatype);
		break;
	default:
		break;
	}
	
	
	dst = ret.copy();
}


void Simg::conv2f(Mat & src, Mat & dst, Mat kernel)
{
	assert(!src.isEmpty() && src.channels() == 1);
	assert(src.datatype() == SIMG_1C32F);
	assert(kernel.datatype() == SIMG_1C8S || kernel.datatype() == SIMG_1C32F);
	Mat _src = src.copy();
	dst = Mat(src.cols(), src.rows(), SIMG_1C32F);

	switch (kernel.datatype())
	{
	case SIMG_1C8S:
	{
		int *directArray = new int[kernel.cols()*kernel.rows()];
		uchar *convArray = new uchar[kernel.cols()*kernel.rows()];

		int x = 0, y = 0;
		size_t directNum = 0;
		convertConvKernel(kernel, src, directArray, convArray, directNum);

		float *srcBuffer = (float*)_src.dataPtr();
		float *dstBuffer = (float*)dst.dataPtr();
		for (int i = 0; i < src.cols()*src.rows(); i++)
		{
			float sum = 0;
			for (size_t j = 0; j < directNum; j++)
			{
				int index = i + directArray[j];
				x = index % src.cols();
				y = index / src.cols();
				if (x < 0 || y < 0 || x > src.cols() - 1 || y > src.rows() - 1)  continue;  //boundary test
				int neighbor = (int)srcBuffer[index];
				sum += neighbor * convArray[j];
			}
			dstBuffer[i] = sum;
		}
		delete directArray; directArray = NULL;
		delete convArray; convArray = NULL;
		break;
	}

	case SIMG_1C32F:
	{
		int *directArray = new int[kernel.cols()*kernel.rows()];
		float *convArray = new float[kernel.cols()*kernel.rows()];

		int x = 0, y = 0;
		size_t directNum = 0;
		convertConvKernel(kernel, src, directArray, convArray, directNum);

		float *srcBuffer = (float*)_src.dataPtr();
		float *dstBuffer = (float*)dst.dataPtr();
		for (int i = 0; i < src.cols()*src.rows(); i++)
		{
			float sum = 0;
			for (size_t j = 0; j < directNum; j++)
			{
				int index = i + directArray[j];
				x = index % src.cols();
				y = index / src.cols();
				if (x < 0 || y < 0 || x > src.cols() - 1 || y > src.rows() - 1)  continue;  //boundary test
				int neighbor = (int)srcBuffer[index];
				sum += neighbor * convArray[j];
			}
			dstBuffer[i] = sum;
		}
		delete directArray; directArray = NULL;
		delete convArray; convArray = NULL;
		break;
	}

	default:
		break;
	}
}

int Simg::threshold(Mat & src, Mat & dst, int threshValue, int method, int value)
{
	assert(!src.isEmpty() && src.channels() == 1);
	Mat _src = src.copy();
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
					ret.setPixel(x, y, (uchar)1);
				}
				else
				{
					ret.setPixel(x, y, (uchar)0);
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
				ret.setPixel(x, y, (uchar)1);
			}
			else
			{
				ret.setPixel(x, y, (uchar)0);
			}
		}
		break;
	case SIMG_MORPH_CROSS:
		for (int x = 0; x < cols; x++)
		for (int y = 0; y < rows; y++)
		{
			if (x == (MIN(cols, rows) / 2) || y == (MIN(cols, rows) / 2))
			{
				ret.setPixel(x, y, (uchar)1);
			}
			else
			{
				ret.setPixel(x, y, (uchar)0);
			}
		}
		break;
	case SIMG_MORPH_RECTANGLE:
		for (int x = 0; x < cols; x++)
		for (int y = 0; y < rows; y++)
		{
			ret.setPixel(x, y, (uchar)1);
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
	Mat _src = src.copy();	//make a shallow copy in case of the situation of src=dst

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

void Simg::Gaussian(Mat & src, Mat & dst, int kernelSize, float sigma)
{
	assert(!src.isEmpty() && src.channels() == 1 && kernelSize > 0 && kernelSize % 2 == 1);
	assert(src.datatype() == SIMG_1C8U);
	
	Mat _src = src.copy();
	Mat kernel(kernelSize, kernelSize, SIMG_1C32F);
	dst = Mat(src.cols(), src.rows(), SIMG_1C8U);

	int center = kernelSize / 2;
	double sum = 0;
	for (int i = 0; i < kernelSize; i++)
	{
		double x2 = pow(i - center, 2);
		for (int j = 0; j < kernelSize; j++)
		{
			double y2 = pow(j - center, 2);
			double g = exp(-(x2 + y2) / (2 * sigma * sigma));
			sum += g;
			kernel.setPixel(i, j, g);	
		}
	}
	for (int i = 0; i < kernelSize; i++)
	{
		
		for (int j = 0; j < kernelSize; j++)
		{
			vector<float> d;
			kernel.getPixel(i, j, d);
			kernel.setPixel(i, j, d[0] / sum);
			kernel.getPixel(i, j, d);
		}
	}

	conv2(_src, dst, kernel,SIMG_1C8U);
}

void Simg::Sobel(Mat & src, Mat & dst, int method)
{
	assert(!src.isEmpty() && src.channels() == 1);
	assert(src.datatype() == SIMG_1C8U);

	Mat _src = src.copy();
	Mat kernelY(3, 3, SIMG_1C32F);
	Mat kernelX(3, 3, SIMG_1C32F);

	kernelX.setPixel(0, 0, -1); kernelX.setPixel(1, 0, 0); kernelX.setPixel(2, 0, 1);
	kernelX.setPixel(0, 1, -2); kernelX.setPixel(1, 1, 0); kernelX.setPixel(2, 1, 2);
	kernelX.setPixel(0, 2, -1); kernelX.setPixel(1, 2, 0); kernelX.setPixel(2, 2, 1);

	kernelY.setPixel(0, 0, 1); kernelY.setPixel(1, 0, 2); kernelY.setPixel(2, 0, 1);
	kernelY.setPixel(0, 1, 0); kernelY.setPixel(1, 1, 0); kernelY.setPixel(2, 1, 0);
	kernelY.setPixel(0, 2, -1); kernelY.setPixel(1, 2, -2); kernelY.setPixel(2, 2, -1);
	switch (method)
	{
	case SIMG_METHOD_SOBEL_X:
		conv2(_src, dst, kernelX,SIMG_1C16S);
		break;
	case SIMG_METHOD_SOBEL_Y:
		conv2(_src, dst, kernelY, SIMG_1C16S);
		break;
	case SIMG_METHOD_SOBEL_XY:
	{
		Mat tmp1,tmp2;
		conv2(_src, tmp1, kernelX, SIMG_1C16S);
		conv2(_src, tmp2, kernelY, SIMG_1C16S);
		dst = mabs(tmp1) + mabs(tmp2);
		break;
	}
		
	default:
		break;
	}
}

void Simg::canny(Mat & src, Mat & dst, int highThreshVal, int lowThreshVal)
{
	assert(!src.isEmpty() && src.datatype() == SIMG_1C8U);

	Mat _src = src.copy();
	Mat sx,sy, sxy;
	Sobel(_src, sx, SIMG_METHOD_SOBEL_X);
	Sobel(_src, sy, SIMG_METHOD_SOBEL_Y);
	sxy = mabs(sx) + mabs(sy);
	
	Mat N0;
	getGradDirectIndex(sx, sy, N0);
	Mat nms = nonMaxSuppresion(sxy, N0);
	Mat doubleTH = doubleThreshold(nms, lowThreshVal, highThreshVal);
	doubleThresholdLink(doubleTH);
	dst =  doubleTH;
}

Mat Simg::mabs(Mat & m1)
{
	Mat ret;
	switch (m1.datatype())
	{
	case SIMG_1C8U:
		ret = funcAbs<uchar>(m1); break;
	case SIMG_3C8U:
		ret = funcAbs<uchar>(m1); break;
	case SIMG_1C8S:
		ret = funcAbs<char>(m1); break;
	case SIMG_3C8S:
		ret = funcAbs<char>(m1); break;
	case SIMG_1C16S:
		ret = funcAbs<short>(m1); break;
	case SIMG_1C32F:
		ret = funcAbs<float>(m1); break;
	case SIMG_3C32F:
		ret = funcAbs<float>(m1); break;
	default:
		break;
	}
	return ret;
}

