#pragma once

#include <assert.h>
#include <io.h>
#include <string>
#include<windows.h>


using namespace std;
typedef unsigned char uchar;

#define MAX(a,b) a>b?a:b
#define MIN(a,b) a<b?a:b

namespace Simg
{


	const int SIMG_1C8U = 0x00010001;
	const int SIMG_1C8S = 0x00010002;
	const int SIMG_1C16U = 0x00010003;
	const int SIMG_1C16S = 0x00010004;
	const int SIMG_1C32F = 0x00010005;
	const int SIMG_1C64F = 0x00010006;
	const int SIMG_NULL = 0x00010000;

	const int SIMG_3C8U = 0x00010011;
	const int SIMG_3C8S = 0x00010012;
	const int SIMG_3C16U = 0x00010013;
	const int SIMG_3C16S = 0x00010014;
	const int SIMG_3C32F = 0x00010015;
	const int SIMG_3C64F = 0x00010016;

	const int SIMG_FORMAT_IMG_NONE = 0x00020000;
	const int SIMG_FORMAT_IMG_BMP = 0x00020001;
	const int SIMG_FORMAT_IMG_JPEG = 0x00020002;
	const int SIMG_FORMAT_IMG_PNG = 0x00020003;



	class Mat
	{
	public:
		Mat();
		Mat(int ncols, int nrows, int datatype);
		Mat &operator = (const Mat&m)
		{
			// TODO: 在此处插入 return 语句
			if (this == &m)
				return *this;
			if (--*_pcount==0)
			{
				_cols = 0;
				_rows = 0;
				_dataType = SIMG_NULL;

				delete _dataPtr;
				delete _pcount;
				_dataPtr = NULL;
				_pcount = NULL;
			}

			this->_cols = m._cols;
			this->_rows = m._rows;
			this->_dataType = m._dataType;
			this->_dataPtr = m._dataPtr;
			this->_pcount = m._pcount;
			_originalFormat = m._originalFormat;

			this->init();

			++*m._pcount;

			return *this;
		}

		Mat(const Mat& m)
		{
			if (this == &m)
				return;
			
			_pcount = m._pcount;
			_cols = m._cols;
			_rows = m._rows;
			_dataType = m._dataType;
			_dataPtr = m._dataPtr;
			_originalFormat = m._originalFormat;

			init();

			++*_pcount;
		}

		Mat	operator + (const Mat &m);
		Mat	operator + (const uchar m);

		Mat	operator - (const Mat &m);
		Mat	operator - (const uchar m);


		uchar* row(int indRow);
		int rows() { return _rows; };
		int cols() { return _cols; };
		int channels() { return _channels; };
		int datatype() { return _dataType; };
		~Mat();

		uchar* dataPtr();

		Mat convertTo(int datatype);
		bool isEmpty() { return NULL == _dataPtr; };

		friend class sWindow;
		friend LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

		friend Mat imread(const char* path);
		friend void split(Mat src, Mat* dst);


	private:

		int init(); //initialize a Mat

		int _cols;	//column num of Mat
		int _rows;	//rows num of Mat
		int _matLength; //cols*rows, length of Mat data
		int _cellLength; //data length of a single cell. e.g. SIMG_8U = 1, SIMG_16U = 2...
		int _colDataLength; //data length of a single col;
		int _rowDataLength; //data length of a single row;
		int _dataLength; //data length of whole Mat
		int _dataType;	//data type of Mat
		int _originalFormat;	//the orginal format of the image, applicable if the image is loaded from a file.
		int _channels;			//[reserved] channel number of the Mat. 
		uchar *_dataPtr;	//original data pointer

		size_t* _pcount; //copy ref counter



	};

}

