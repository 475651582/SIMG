#include "stdafx.h"
#include "simgMat.h"
#include "imgprocess.h"


using namespace Simg;
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

uchar * Mat::row(int indRow)
{
	//if (*_pcount > 1)
	//{
	//	*_pcount--;
	//	//////allocate new ptr and memory for dst mat
	//	size_t* tmpPcount = new size_t(1);
	//	uchar* tmpDataPtr = new uchar[_dataLength];
	//	memcpy(tmpDataPtr, _dataPtr, _dataLength);
	//	_dataPtr = tmpDataPtr;
	//	_pcount = tmpPcount;
	//}
	//if (SIMG_FORMAT_IMG_BMP == _originalFormat)
	//{
	//	//bmp format start from lowerleft pixel
	//	return _dataPtr + _colDataLength * (_rows - indRow - 1);
	//}
	//else
	//{
	//	return _dataPtr + _colDataLength * indRow;
	//}
	return nullptr;
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

Mat Simg::Mat::convertTo(int datatype)
{
	Mat ret;
	switch (datatype)
	{
	case SIMG_METHOD_CONVERT_RGB2GRAY_STANDARD:
		rgb2gray(*this, ret, SIMG_METHOD_CONVERT_RGB2GRAY_STANDARD);
		break;
	case SIMG_METHOD_CONVERT_RGB2GRAY_AVERAGE:
		rgb2gray(*this, ret, SIMG_METHOD_CONVERT_RGB2GRAY_AVERAGE);
		break;
	case SIMG_METHOD_CONVERT_RGB2LAB_STANDARD:
		rgb2lab(*this, ret, SIMG_METHOD_CONVERT_RGB2LAB_STANDARD);
		break;
	default:
		break;
	}
	return ret;
}





Mat Simg::Mat::extendTo(int col, int row)
{
	assert(col >= _cols && row >= _rows);
	Mat ret(col, row, _dataType);
	for (size_t i = 0; i < _rows; i++)
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
