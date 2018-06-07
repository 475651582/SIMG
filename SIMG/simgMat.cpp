#include "stdafx.h"
#include "simgMat.h"

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
	if (*_pcount > 1)
	{
		*_pcount--;
		//////allocate new ptr and memory for dst mat
		size_t* tmpPcount = new size_t(1);
		uchar* tmpDataPtr = new uchar[_dataLength];
		memcpy(tmpDataPtr, _dataPtr, _dataLength);
		_dataPtr = tmpDataPtr;
		_pcount = tmpPcount;
	}
	if (SIMG_FORMAT_IMG_BMP == _originalFormat)
	{
		//bmp format start from lowerleft pixel
		return _dataPtr + _colDataLength * (_rows - indRow - 1);
	}
	else
	{
		return _dataPtr + _colDataLength * indRow;
	}

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