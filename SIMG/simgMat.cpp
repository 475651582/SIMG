/*******************************************************************
*
*	Author: Felix Shang
*	Company: [personal]
*	Date: 2018.06.05
*	Brief: A simple Mat structure used for basic image processing
*
******************************************************************/

#include "stdafx.h"
#include "simgMat.h"
#include "imgprocess.h"


using namespace Simg;

template<typename dtype1, typename dtype2> void convertDataType(Mat &src, Mat &dst, int datatype)
{
	assert(!src.isEmpty());
	const char* m = typeid(dtype1).name();
	dst = Mat(src.cols(), src.rows(), datatype);

	Mat _src = src.copy();
	int srcCols = _src.cols();
	int srcRows = _src.rows();
	int srcChannels = _src.channels();
	int dstChannels = dst.channels();
	dtype1* srcBuffer = (dtype1*)_src.dataPtr();
	dtype2* dstBuffer = (dtype2*)dst.dataPtr();

	if (srcChannels == 1 && srcChannels < dstChannels)
	{
		for (int i = 0; i < srcCols * srcRows; i++)
		{
			for (int ch = 0; ch < dstChannels; ch++)
			{
				dstBuffer[i*dstChannels + ch] = (dtype2)srcBuffer[i];
			}
		}
	}
	else if (srcChannels == dstChannels)
	{
		for (int i = 0; i < srcCols * srcRows; i++)
		{
			for (int ch = 0; ch < dstChannels; ch++)
			{
				dstBuffer[i*dstChannels + ch] = (dtype2)srcBuffer[i*dstChannels + ch];
			}
		}
	}
	else
	{
		assert(false && "unsupported format conversion!");
	}
	
}

template<typename dtype> void padMemory(dtype* dstBuffer, dtype* srcBuffer, int srcCols, int srcRows, int padX, int padY, int &ULpadPtrStarter)
{



	int padSrcCols = srcCols + 2 * padX;
	int padSrcRows = srcRows + 2 * padY;
	int padDataLength = padSrcCols * padSrcRows;
	int dataLength = srcCols * srcRows;


	

	for (int i = padY; i < padSrcRows - padY; i++)
	{
		int srcY = i - padY;
		memcpy(dstBuffer + padX + i * padSrcCols, srcBuffer + srcY * srcCols, srcCols * sizeof(dtype));
	}
	ULpadPtrStarter = padX + padY * padSrcCols;

}

Mat::Mat()
{
	_cols = 0;
	_rows = 0;
	_dataType = SIMG_NULL;
	_channels = 1;
	_dataPtr = NULL;
	_pcount = new size_t(1);
	_originalFormat = SIMG_FORMAT_IMG_NONE;
	init();
}

Mat::Mat(int ncols, int nrows, int datatype)
{
	_cols = ncols;
	_rows = nrows;
	_dataType = datatype;
	_channels = 1;
	_originalFormat = SIMG_FORMAT_IMG_NONE;

	init();

	_dataPtr = new uchar[_dataLength];
	memset(_dataPtr, 0, _dataLength);
	_pcount = new size_t(1);
}





Mat Mat::operator+(const Mat & m)
{
	assert(_cols == m._cols && _rows == m._rows && _dataType == m._dataType && _dataPtr != NULL && m._dataPtr != NULL);

	Mat ret(_cols, _rows, _dataType);
	

	switch (_dataType)
	{
	case SIMG_1C8U:
	{
		uchar* ptr_ret = (uchar*)ret._dataPtr;
		uchar* ptr = (uchar*)_dataPtr;
		uchar* ptr_m = (uchar*)m._dataPtr;
		for (int i = 0; i < _cols*_rows; i++)
		{
			ptr_ret[i] = MAX(MIN(ptr[i] + ptr_m[i], UCHAR_MAX), 0);
		}
		break;
	}
	case SIMG_1C8S:
	{
		char* ptr_ret = (char*)ret._dataPtr;
		char* ptr = (char*)_dataPtr;
		char* ptr_m = (char*)m._dataPtr;
		for (int i = 0; i < _cols*_rows; i++)
		{
			ptr_ret[i] = MAX(MIN(ptr[i] + ptr_m[i], CHAR_MAX), CHAR_MIN);
		}
		break;
	}
	case SIMG_1C16S:
	{
		short* ptr_ret = (short*)ret._dataPtr;
		short* ptr = (short*)_dataPtr;
		short* ptr_m = (short*)m._dataPtr;
		for (int i = 0; i < _cols*_rows; i++)
		{
			ptr_ret[i] = MAX(MIN(ptr[i] + ptr_m[i], SHRT_MAX), SHRT_MIN);
		}
		break;
	}
	case SIMG_1C32F:
	{
		float* ptr_ret = (float*)ret._dataPtr;
		float* ptr = (float*)_dataPtr;
		float* ptr_m = (float*)m._dataPtr;
		for (int i = 0; i < _cols*_rows; i++)
		{
			ptr_ret[i] = ptr[i] + ptr_m[i];
		}
		break;
	}
	case SIMG_1C64F:
	{
		double* ptr_ret = (double*)ret._dataPtr;
		double* ptr = (double*)_dataPtr;
		double* ptr_m = (double*)m._dataPtr;
		for (int i = 0; i < _cols*_rows; i++)
		{
			ptr_ret[i] = ptr[i] + ptr_m[i];
		}
		break;
	}
	default:
		break;
	}

	return ret;
}


Mat Mat::operator-(const Mat & m)
{
	assert(_cols == m._cols && _rows == m._rows && _dataType == m._dataType && _dataPtr != NULL && m._dataPtr != NULL);

	Mat ret(_cols, _rows, _dataType);


	switch (_dataType)
	{
	case SIMG_1C8U:
	{
		uchar* ptr_ret = (uchar*)ret._dataPtr;
		uchar* ptr = (uchar*)_dataPtr;
		uchar* ptr_m = (uchar*)m._dataPtr;
		for (int i = 0; i < _cols*_rows; i++)
		{
			ptr_ret[i] = MAX(MIN(ptr[i] - ptr_m[i], UCHAR_MAX), 0);
		}
		break;
	}
	case SIMG_1C8S:
	{
		char* ptr_ret = (char*)ret._dataPtr;
		char* ptr = (char*)_dataPtr;
		char* ptr_m = (char*)m._dataPtr;
		for (int i = 0; i < _cols*_rows; i++)
		{
			ptr_ret[i] = MAX(MIN(ptr[i] - ptr_m[i], CHAR_MAX), CHAR_MIN);
		}
		break;
	}
	case SIMG_1C16S:
	{
		short* ptr_ret = (short*)ret._dataPtr;
		short* ptr = (short*)_dataPtr;
		short* ptr_m = (short*)m._dataPtr;
		for (int i = 0; i < _cols*_rows; i++)
		{
			ptr_ret[i] = MAX(MIN(ptr[i] - ptr_m[i], SHRT_MAX), SHRT_MIN);
		}
		break;
	}
	case SIMG_1C32F:
	{
		float* ptr_ret = (float*)ret._dataPtr;
		float* ptr = (float*)_dataPtr;
		float* ptr_m = (float*)m._dataPtr;
		for (int i = 0; i < _cols*_rows; i++)
		{
			ptr_ret[i] = ptr[i] - ptr_m[i];
		}
		break;
	}
	case SIMG_1C64F:
	{
		double* ptr_ret = (double*)ret._dataPtr;
		double* ptr = (double*)_dataPtr;
		double* ptr_m = (double*)m._dataPtr;
		for (int i = 0; i < _cols*_rows; i++)
		{
			ptr_ret[i] = ptr[i] - ptr_m[i];
		}
		break;
	}
	default:
		break;
	}

	return ret;
}


Mat Simg::Mat::copy()
{
	Mat ret(_cols, _rows, _dataType);
	memcpy(ret._dataPtr, _dataPtr, _dataLength);
	return ret;
}

Mat Simg::Mat::ROI(int x, int y, int w, int h)
{
	assert(x >= 0 && x < _cols && y >= 0 && y < _rows);
	assert(x + w < _cols && y + h < _rows);
	Mat ret(w, h, _dataType);

	int i0 = 0;
	for (int i = y ; i < y + h; i++)
	{
		memcpy(ret._dataPtr + ((i0++) * w) * ret._cellLength * ret._channels, _dataPtr + (x + i * _cols) * _cellLength * _channels, w * _cellLength * _channels);
	}
	return ret;
}

Mat Simg::Mat::padMat(int padX, int padY, int & ULpadPtrStarter)
{
	assert(_channels == 1);
	int padSrcCols = _cols + 2 * padX;
	int padSrcRows = _rows + 2 * padY;
	int padDataLength = padSrcCols * padSrcRows;
	int dataLength = _cols * _rows;

	Mat dst = Mat(padSrcCols, padSrcRows, _dataType);

	switch (_dataType)
	{
	case SIMG_1C8U:
	{
		uchar *srcBuffer = (uchar*)_dataPtr; uchar *dstBuffer = (uchar*)dst._dataPtr;
		padMemory<uchar>(dstBuffer, srcBuffer, _cols, _rows, padX, padY, ULpadPtrStarter);
		break;
	}
	case SIMG_1C8S:
	{
		char *srcBuffer = (char*)_dataPtr; char *dstBuffer = (char*)dst._dataPtr;
		padMemory<char>(dstBuffer, srcBuffer, _cols, _rows, padX, padY, ULpadPtrStarter);
		break;
	}
	case SIMG_1C16S:
	{
		short *srcBuffer = (short*)_dataPtr; short *dstBuffer = (short*)dst._dataPtr;
		padMemory<short>(dstBuffer, srcBuffer, _cols, _rows, padX, padY, ULpadPtrStarter);
		break;
	}
	case SIMG_1C32F:
	{
		float *srcBuffer = (float*)_dataPtr; float *dstBuffer = (float*)dst._dataPtr;
		padMemory<float>(dstBuffer, srcBuffer, _cols, _rows, padX, padY, ULpadPtrStarter);
		break;
	}

	default:
		break;
	}


	return dst;
}




uchar * Mat::dataPtr()
{
	if (*_pcount > 1)
	{
		*_pcount--;
		//allocate new ptr and memory for dst mat
		size_t* tmpPcount = new size_t(1);
		uchar* tmpDataPtr = new uchar[_dataLength];
		memcpy(tmpDataPtr, _dataPtr, _dataLength);
		_dataPtr = tmpDataPtr;
		_pcount = tmpPcount;
	}

	return _dataPtr;
}

Mat Simg::Mat::convertTo(int convertType)
{
	Mat ret;

	switch (convertType)
	{
	case SIMG_METHOD_CONVERT_RGB2GRAY_STANDARD:
		rgb2gray(*this, ret, SIMG_METHOD_CONVERT_RGB2GRAY_STANDARD); break;
	case SIMG_METHOD_CONVERT_RGB2GRAY_AVERAGE:
		rgb2gray(*this, ret, SIMG_METHOD_CONVERT_RGB2GRAY_AVERAGE); break;
	case SIMG_METHOD_CONVERT_RGB2LAB_STANDARD:
		rgb2lab(*this, ret, SIMG_METHOD_CONVERT_RGB2LAB_STANDARD); break;
	case SIMG_METHOD_CONVERT_1C8U:
	{
		switch (_dataType)
		{
		case SIMG_1C8U:
			convertDataType<uchar, uchar>(*this, ret, SIMG_1C8U); break;
		case SIMG_1C16S:
			convertDataType<short, uchar>(*this, ret, SIMG_1C8U); break;
		case SIMG_3C8U:
			convertDataType<uchar, uchar>(*this, ret, SIMG_1C8U); break;
		case SIMG_1C32F:
			convertDataType<float, uchar>(*this, ret, SIMG_1C8U); break;
		case SIMG_3C32F:
			convertDataType<float, uchar>(*this, ret, SIMG_1C8U); break;
		default:
			break;
		}
		break;
	}
	case SIMG_METHOD_CONVERT_3C8U:
	{
		switch (_dataType)
		{
		case SIMG_1C8U:
			convertDataType<uchar, uchar>(*this, ret, SIMG_3C8U); break;
		case SIMG_3C8U:
			convertDataType<uchar, uchar>(*this, ret, SIMG_3C8U); break;
		case SIMG_1C32F:
			convertDataType<float, uchar>(*this, ret, SIMG_3C8U); break;
		case SIMG_3C32F:
			convertDataType<float, uchar>(*this, ret, SIMG_3C8U); break;
		default:
			break;
		}
		break;
	}
	case SIMG_METHOD_CONVERT_1C32F:
	{
		switch (_dataType)
		{
		case SIMG_1C8U:
			convertDataType<uchar, float>(*this, ret, SIMG_1C32F); break;
		case SIMG_1C16S:
			convertDataType<short, float>(*this, ret, SIMG_1C32F); break;
		case SIMG_3C8U:
			convertDataType<uchar, float>(*this, ret, SIMG_1C32F); break;
		case SIMG_1C32F:
			convertDataType<float, float>(*this, ret, SIMG_1C32F); break;
		case SIMG_3C32F:
			convertDataType<float, float>(*this, ret, SIMG_1C32F); break;
		default:
			break;
		}
		break;
	}
	case SIMG_METHOD_CONVERT_3C32F:
	{
		switch (_dataType)
		{
		case SIMG_1C8U:
			convertDataType<uchar, float>(*this, ret, SIMG_3C32F); break;
		case SIMG_3C8U:
			convertDataType<uchar, float>(*this, ret, SIMG_3C32F); break;
		case SIMG_1C32F:
			convertDataType<float, float>(*this, ret, SIMG_3C32F); break;
		case SIMG_3C32F:
			convertDataType<float, float>(*this, ret, SIMG_3C32F); break;
		default:
			break;
		}
		break;
	}
	case SIMG_METHOD_CONVERT_1C16S:
	{
		switch (_dataType)
		{
		case SIMG_1C8U:
			convertDataType<uchar, short>(*this, ret, SIMG_1C16S); break;
		case SIMG_1C8S:
			convertDataType<char, short>(*this, ret, SIMG_1C16S); break;
		case SIMG_3C8U:
			convertDataType<uchar, short>(*this, ret, SIMG_1C16S); break;
		case SIMG_1C32F:
			convertDataType<float, short>(*this, ret, SIMG_1C16S); break;
		case SIMG_3C32F:
			convertDataType<float, short>(*this, ret, SIMG_1C16S); break;
		default:
			break;
		}
		break;
	}
	default:
		break;
	}

	return ret;
}





Mat Simg::Mat::extendTo(int col, int row)
{
	assert(col >= _cols && row >= _rows);
	Mat ret(col, row, _dataType);
	for (int i = 0; i < _rows; i++)
	{
		memcpy(ret._dataPtr + i * col * _cellLength * _channels, _dataPtr + i * _cols * _cellLength * _channels, _cellLength * _channels * _cols);
	}

	return ret;
}


Mat::~Mat()
{


	if (--*_pcount == 0 && NULL != _dataPtr)
	{
		_cols = 0;
		_rows = 0;
		_dataType = SIMG_NULL;

		delete _dataPtr;
		delete _pcount;
		_dataPtr = NULL;
		_pcount = NULL;
	}

}

int Mat::init()
{
	_matLength = _cols * _rows;



	switch (_dataType)
	{
	case SIMG_1C8U:
		_cellLength = sizeof(uchar);
		_channels = 1;
		break;
	case SIMG_1C8S:
		_cellLength = sizeof(char);
		_channels = 1;
		break;
	case SIMG_1C16U:
		_cellLength = sizeof(unsigned short);
		_channels = 1;
		break;
	case SIMG_1C16S:
		_cellLength = sizeof(short);
		_channels = 1;
		break;
	case SIMG_1C32F:
		_cellLength = sizeof(float);
		_channels = 1;
		break;
	case SIMG_1C64F:
		_cellLength = sizeof(double);
		_channels = 1;
		break;
	case SIMG_NULL:
		_cellLength = 0;
		_channels = 1;
		break;

	case SIMG_3C8U:
		_cellLength = sizeof(uchar);
		_channels = 3;
		break;
	case SIMG_3C8S:
		_cellLength = sizeof(char);
		_channels = 3;
		break;
	case SIMG_3C16U:
		_cellLength = sizeof(unsigned short);
		_channels = 3;
		break;
	case SIMG_3C16S:
		_cellLength = sizeof(short);
		_channels = 3;
		break;
	case SIMG_3C32F:
		_cellLength = sizeof(float);
		_channels = 3;
		break;
	case SIMG_3C64F:
		_cellLength = sizeof(double);
		_channels = 3;
		break;

	default:
		_cellLength = 0;
		_channels = 1;
		break;
	}

	_dataLength = _matLength * _cellLength * _channels;
	_colDataLength = _cols * _cellLength;
	_rowDataLength = _rows * _cellLength;

	return 0;
}

void Simg::Mat::modified()
{
	if (*_pcount > 1)
	{
		*_pcount--;
		//allocate new ptr and memory for dst mat
		size_t* tmpPcount = new size_t(1);
		uchar* tmpDataPtr = new uchar[_dataLength];
		memcpy(tmpDataPtr, _dataPtr, _dataLength);
		_dataPtr = tmpDataPtr;
		_pcount = tmpPcount;
	}
}


