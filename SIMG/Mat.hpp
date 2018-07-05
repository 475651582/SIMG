
#pragma once

#include <assert.h>
#include <io.h>
#include <string>
#include <vector>
#include<windows.h>
#include <type_traits>

using namespace std;
typedef unsigned char uchar;
typedef unsigned short ushort;

#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)<(b)?(a):(b))

namespace Simg
{
	enum eDataType
	{
		EMPTY = 0,
		UCHAR = 1,
		CHAR = 2,
		USHORT = 3,
		SHORT = 4,
		INT = 5,
		FLOAT = 6,
		DOUBLE = 7
	};

	template<class T = uchar>
	class Mat
	{
	public:
		Mat(int nCols, int nRows, int nChannels = 1)
		{
			initialize(nCols, nRows, nChannels);
			_dataPtr = new T[_matLength];
			memset(_dataPtr, 0, _dataLength);
			_pcount = new size_t(1);
		}


		template<class T1>
		Mat(const Mat<T1> &m)
		{
			if (this == &m)
				return;

			_pcount = m.pcount();
			_cols = m.cols();
			_rows = m.rows();
			_datatype = m.datatype();
			if (is_same<T,T1>::value)
			{
				_dataPtr = m.dataPtr();
			}
			else
			{
				T = T1;
			}
			
			

			//init();

			++*_pcount;
		}


		int cols() const { return _cols; }
		int rows() const { return _rows; }
		int channels() const { return _chs; }
		eDataType datatype() const { return _datatype; }
		size_t pcount() const { return size_t; }
		T* dataPtr()
		{
			modified();
			return _dataPtr;
		}

	private:
		int _cols, _rows, _chs;
		int _matLength;
		int _dataLength;
		eDataType _datatype;
		
		void initialize(int nCols, int nRows, int nChannels)
		{
			assert(nCols > 0 && nRows > 0 && nChannels > 0);
			
			if (is_same<T, uchar>::value) _datatype = eDataType::UCHAR;
			else if (is_same<T, char>::value) _datatype = eDataType::CHAR;
			else if (is_same<T, ushort>::value) _datatype = eDataType::USHORT;
			else if (is_same<T, short>::value) _datatype = eDataType::SHORT;
			else if (is_same<T, int>::value) _datatype = eDataType::INT;
			else if (is_same<T, float>::value) _datatype = eDataType::FLOAT;
			else if (is_same<T, double>::value) _datatype = eDataType::DOUBLE;
			else assert(0 && "unknown data type!");

			_cols = nCols; _rows = nRows; _chs = nChannels;
			_matLength = _cols * _rows;
			_dataLength = sizeof(T) * _matLength;
		}
		void modified()
		{
			if (*_pcount > 1)
			{
				*_pcount--;
				//allocate new ptr and memory for dst mat
				size_t* tmpPcount = new size_t(1);
				T* tmpDataPtr = new T[_matLength];
				memcpy(tmpDataPtr, _dataPtr, _dataLength);
				_dataPtr = tmpDataPtr;
				_pcount = tmpPcount;
			}
		}

		T* _dataPtr;
		size_t* _pcount; //copy ref counter
	};



	


}