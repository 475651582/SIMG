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
	for (int i = 0; i < _matLength * _channels; i++)
	{
		ret._dataPtr[i] = MAX(MIN(_dataPtr[i] + m._dataPtr[i], 255),0);
	}
	return ret;
}

Mat Simg::Mat::operator+(const uchar m)
{
	Mat ret(_cols, _rows, _dataType);
	for (int i = 0; i < _matLength * _channels; i++)
	{
		ret._dataPtr[i] = MAX(MIN(_dataPtr[i] + m, 255), 0);
	}
	return ret;
}

Mat Simg::Mat::operator-(const Mat & m)
{
	assert(_cols == m._cols && _rows == m._rows && _dataType == m._dataType && _dataPtr != NULL && m._dataPtr != NULL);

	Mat ret(_cols, _rows, _dataType);
	for (int i = 0; i < _matLength * _channels; i++)
	{
		ret._dataPtr[i] = MIN(MAX(_dataPtr[i] - m._dataPtr[i], 0), 255);
	}
	return ret;
}

Mat Simg::Mat::operator-(const uchar m)
{
	Mat ret(_cols, _rows, _dataType);
	for (int i = 0; i < _matLength * _channels; i++)
	{
		ret._dataPtr[i] = MIN(MAX(_dataPtr[i] - m, 0), 255);
	}
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






void Simg::Mat::setPixel(int col, int row, uchar data)
{
	assert(col >= 0 && col < _cols&&row >= 0 && row < _rows&&_dataPtr != NULL && _channels == 1);
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
	_dataPtr[col * _cellLength + row * _cellLength * _cols] = data;
}

Mat Simg::Mat::setTo(uchar data)
{
	assert(_dataPtr != NULL && _channels == 1);
	Mat ret(_cols, _rows, _dataType);

	memset(ret._dataPtr, data, ret._dataLength);
	return ret;
}

void Simg::Mat::setPixel(int col, int row, uchar ch1, uchar ch2, uchar ch3)
{
	assert(col >= 0 && col < _cols&&row >= 0 && row < _rows&&_dataPtr != NULL && _channels == 3);
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
	_dataPtr[col * _cellLength + row * _cellLength * _cols] = ch1;
	_dataPtr[col * _cellLength + row * _cellLength * _cols + 1] = ch2;
	_dataPtr[col * _cellLength + row * _cellLength * _cols + 2] = ch3;
}

Mat Simg::Mat::setTo(uchar ch1, uchar ch2, uchar ch3)
{
	
	assert(_dataPtr != NULL && _channels == 3);
	Mat ret(_cols, _rows, _dataType);

	for (int i = 0; i < _cols * _rows; i++)
	{
		ret._dataPtr[i * ret._channels] = ch1;
		ret._dataPtr[i * ret._channels + 1] = ch2;
		ret._dataPtr[i * ret._channels + 2] = ch3;
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
		_cellLength = 1;
		_channels = 1;
		break;
	case SIMG_1C8S:
		_cellLength = 1;
		_channels = 1;
		break;
	case SIMG_1C16U:
		_cellLength = 2;
		_channels = 1;
		break;
	case SIMG_1C16S:
		_cellLength = 2;
		_channels = 1;
		break;
	case SIMG_1C32F:
		_cellLength = 4;
		_channels = 1;
		break;
	case SIMG_1C64F:
		_cellLength = 8;
		_channels = 1;
		break;
	case SIMG_NULL:
		_cellLength = 0;
		_channels = 1;
		break;

	case SIMG_3C8U:
		_cellLength = 3;
		_channels = 3;
		break;
	case SIMG_3C8S:
		_cellLength = 3;
		_channels = 3;
		break;
	case SIMG_3C16U:
		_cellLength = 6;
		_channels = 3;
		break;
	case SIMG_3C16S:
		_cellLength = 6;
		_channels = 3;
		break;
	case SIMG_3C32F:
		_cellLength = 12;
		_channels = 3;
		break;
	case SIMG_3C64F:
		_cellLength = 24;
		_channels = 3;
		break;

	default:
		_cellLength = 0;
		_channels = 1;
		break;
	}

	_dataLength = _matLength * _cellLength;
	_colDataLength = _cols * _cellLength;
	_rowDataLength = _rows * _cellLength;

	return 0;
}