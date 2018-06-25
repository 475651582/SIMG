/*******************************************************************
*	
*	Author: Felix Shang
*	Company: [personal]
*	Date: 2018.06.05
*	Brief: A simple Mat structure used for basic image processing
*
******************************************************************/

#pragma once

#include <assert.h>
#include <io.h>
#include <string>
#include <vector>
#include<windows.h>

using namespace std;
typedef unsigned char uchar;

#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)<(b)?(a):(b))


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


	struct BGR
	{
		uchar b, g, r;
	};

	struct Size
	{
		Size()
		{
			x = 0; y = 0;
		}
		Size(int _x, int _y)
		{
			x = _x; y = _y;
		}
		int x, y;
	};
	

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
		
		
		template <typename dtype> Mat operator + (const dtype m);
		template <typename dtype> Mat operator - (const dtype m);

		Mat	operator - (const Mat &m);
		Mat	operator + (const Mat &m);
		

		Mat copy(); //get a deep copy of the Mat;
		Mat ROI(int x, int y, int w, int h);
		Mat padMat(int padX, int padY, int& ULpadPtrStarter);	//pad a boundary memory around the original mat, UPpadPrtStarter is the ptr of the orginial data head
		
		template <typename dtype> void getMax(vector<dtype>& dst);
		template <typename dtype> void getMin(vector<dtype>& dst);

		//get the value of Mat(i,j,ch)
		template<typename dtype> dtype& at(int i, int j, int ch = 0);

		int rows() { return _rows; };
		int cols() { return _cols; };
		int channels() { return _channels; };
		int datatype() { return _dataType; };

		//<Deperecated> use at(i,j) instead
		template <typename dtype> void setPixel(int col, int row, dtype data);

		//<Deperecated> use at(i,j) instead
		template <typename dtype> void getPixel(int col, int row, vector<dtype>& dst);

		//<Deperecated> use at(i,j) instead
		template <typename dtype> void setPixel(int col, int row, dtype ch1, dtype ch2, dtype ch3); //set pixel to assigned value(3 channel only)
		
		//set whole image to assigned value (single channel only)
		template <typename dtype> void setTo(dtype data);	

		//extend the mat to a certain size to speed up algorithm in a certain circumstance
		Mat extendTo(int col, int row);
		

		~Mat();

		//get the pointer to Mat data
		uchar* dataPtr();	

		//convert the Mat to a certain datatype
		Mat convertTo(int convertType);

		//return null if the mat data is empty
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
		int _channels;			//channel number of the Mat. 
		uchar *_dataPtr;	//original data pointer
		size_t* _pcount; //copy ref counter
		void modified(); //if the data memory of the mat is modified, apply this to allocate new ptr and memory

	};

	template<typename dtype>
	inline void Mat::setPixel(int col, int row, dtype data)
	{
		assert(col >= 0 && col < _cols&&row >= 0 && row < _rows && _dataPtr != NULL && _channels == 1);

		modified();
		
		switch (_dataType)
		{
		case SIMG_1C8U:
		{
			uchar* ptr = (uchar*)_dataPtr;
			ptr[col + row * _cols] = (uchar)data;
			break;
		}			
		case SIMG_1C8S:
		{
			char* ptr = (char*)_dataPtr;
			ptr[col + row * _cols] = (char)data;
			break;
		}
		case SIMG_1C32F:
		{
			float* ptr = (float*)_dataPtr;
			ptr[col + row * _cols] = (float)data;
			break;
		}
		case SIMG_1C64F:
		{
			double* ptr = (double*)_dataPtr;
			ptr[col + row * _cols] = (double)data;
			break;
		}
		default:
			break;
		}

		
	}

	template<typename dtype>
	inline void Mat::getPixel(int col, int row, vector<dtype>& dst)
	{
		assert(col >= 0 && col < _cols&&row >= 0 && row < _rows && _dataPtr != NULL);
		assert(sizeof(dtype) == _cellLength);
		dst.clear();
		dtype * ptr = (dtype*)_dataPtr;
		for (int i = 0; i < _channels; i++)
		{
			dtype data = ptr[i + (col + row * _cols) *_channels];
			dst.push_back(data);
		}
		
	}

	template<typename dtype>
	inline void Mat::setPixel(int col, int row, dtype ch1, dtype ch2, dtype ch3)
	{
		assert(col >= 0 && col < _cols&&row >= 0 && row < _rows && _dataPtr != NULL && _channels == 3);

		modified();


		switch (_dataType)
		{
		case SIMG_3C8U:
		{
			uchar* ptr = (uchar*)_dataPtr;
			ptr[(col + row * _cols) * 3] = (uchar)ch1;
			ptr[(col + row * _cols) * 3 + 1] = (uchar)ch2;
			ptr[(col + row * _cols) * 3 + 2] = (uchar)ch3;
			break;
		}
		case SIMG_3C8S:
		{
			char* ptr = (char*)_dataPtr;
			ptr[(col + row * _cols) * 3] = (char)ch1;
			ptr[(col + row * _cols) * 3 + 1] = (char)ch2;
			ptr[(col + row * _cols) * 3 + 2] = (char)ch3;
			break;
		}
		case SIMG_3C32F:
		{
			float* ptr = (float*)_dataPtr;
			ptr[(col + row * _cols) * 3] = (float)ch1;
			ptr[(col + row * _cols) * 3 + 1] = (float)ch2;
			ptr[(col + row * _cols) * 3 + 2] = (float)ch3;
			break;
		}
		case SIMG_3C64F:
		{
			double* ptr = (double*)_dataPtr;
			ptr[(col + row * _cols) * 3] = (double)ch1;
			ptr[(col + row * _cols) * 3 + 1] = (double)ch2;
			ptr[(col + row * _cols) * 3 + 2] = (double)ch3;
			break;
		}
		default:
			break;
		}


	}
	template<typename dtype>
	inline void Mat::setTo(dtype data)
	{

		assert(_dataPtr != NULL && _channels == 1);
		modified();

		for (int i = 0; i < _cols *_rows; i++)
		{

			switch (_dataType)
			{
			case SIMG_1C8U:
			{
				uchar* ptr = (uchar*)_dataPtr;
				ptr[i] = (uchar)data;
				break;
			}
			case SIMG_1C8S:
			{
				char* ptr = (char*)_dataPtr;
				ptr[i] = (char)data;
				break;
			}
			case SIMG_1C32F:
			{
				float* ptr = (float*)_dataPtr;
				ptr[i] = (float)data;
				break;
			}
			case SIMG_1C64F:
			{
				double* ptr = (double*)_dataPtr;
				ptr[i] = (double)data;
				break;
			}
			default:
				break;
			}

		}
	}



	template<typename dtype>
	inline Mat Mat::operator+(dtype data)
	{
		assert(_dataPtr != NULL && 1 == _channels);
		Mat ret(_cols, _rows, _dataType);

		switch (_dataType)
		{
		case SIMG_1C8U:
		{
			uchar* ptr = (uchar*)_dataPtr;
			uchar* ret_ptr = (uchar*)ret._dataPtr;
			for (int i = 0; i < _cols*_rows; i++)
			{
				ret_ptr[i] = MAX(MIN(ptr[i] + (uchar)data, UCHAR_MAX), 0);
			}
			break;
		}
		case SIMG_1C8S:
		{
			char* ptr = (char*)_dataPtr;
			char* ret_ptr = (char*)ret._dataPtr;
			for (int i = 0; i < _cols*_rows; i++)
			{
				ret_ptr[i] = MAX(MIN(ptr[i] + (char)data, CHAR_MAX), 0);
			}
			break;
		}
		case SIMG_1C16S:
		{
			short* ptr = (short*)_dataPtr;
			short* ret_ptr = (short*)ret._dataPtr;
			for (int i = 0; i < _cols*_rows; i++)
			{
				ret_ptr[i] = MAX(MIN(ptr[i] + (char)data, SHRT_MAX), 0);
			}
			break;
		}
		case SIMG_1C32F:
		{
			float* ptr = (float*)_dataPtr;
			float* ret_ptr = (float*)ret._dataPtr;
			for (int i = 0; i < _cols*_rows; i++)
			{
				ret_ptr[i] =ptr[i] + (float)data;
			}
			break;
		}
		case SIMG_1C64F:
		{
			double* ptr = (double*)_dataPtr;
			double* ret_ptr = (double*)ret._dataPtr;
			for (int i = 0; i < _cols*_rows; i++)
			{
				ret_ptr[i] = ptr[i] + (double)data;
			}
			break;
		}
		default:
			break;
		}
		return ret;
	}

	template<typename dtype>
	inline Mat Mat::operator-(dtype data)
	{
		assert(_dataPtr != NULL && 1 == _channels);
		Mat ret(_cols, _rows, _dataType);

		switch (_dataType)
		{
		case SIMG_1C8U:
		{
			uchar* ptr = (uchar*)_dataPtr;
			uchar* ret_ptr = (uchar*)ret._dataPtr;
			for (int i = 0; i < _cols*_rows; i++)
			{
				ret_ptr[i] = MAX(MIN(ptr[i] - (uchar)data, UCHAR_MAX), 0);
			}
			break;
		}
		case SIMG_1C8S:
		{
			char* ptr = (char*)_dataPtr;
			char* ret_ptr = (char*)ret._dataPtr;
			for (int i = 0; i < _cols*_rows; i++)
			{
				ret_ptr[i] = MAX(MIN(ptr[i] - (char)data, CHAR_MAX), 0);
			}
			break;
		}
		case SIMG_1C8S:
		{
			short* ptr = (short*)_dataPtr;
			short* ret_ptr = (short*)ret._dataPtr;
			for (int i = 0; i < _cols*_rows; i++)
			{
				ret_ptr[i] = MAX(MIN(ptr[i] - (short)data, SHRT_MAX), 0);
			}
			break;
		}
		case SIMG_1C32F:
		{
			float* ptr = (float*)_dataPtr;
			float* ret_ptr = (float*)ret._dataPtr;
			for (int i = 0; i < _cols*_rows; i++)
			{
				ret_ptr[i] = ptr[i] - (float)data;
			}
			break;
		}
		case SIMG_1C64F:
		{
			double* ptr = (double*)_dataPtr;
			double* ret_ptr = (double*)ret._dataPtr;
			for (int i = 0; i < _cols*_rows; i++)
			{
				ret_ptr[i] = ptr[i] - (double)data;
			}
			break;
		}
		default:
			break;
		}
		return ret;
	}

	template<typename dtype>
	inline void Mat::getMax(vector<dtype> &dst)
	{
		dst.clear();
		switch (_dataType)
		{
		case SIMG_1C8U:
		{
			uchar * ptr = (uchar*)_dataPtr;
			uchar maxData = 0;
			for (int i = 0; i < _cols*_rows; i++)
			{
				uchar data = ptr[i];
				maxData = data > maxData ? data : maxData;
			}
			dst.push_back(maxData);
			break;
		}
			
		case SIMG_3C8U:
		{
			//vector<uchar> ret;
			uchar * ptr = (uchar*)_dataPtr;
			uchar maxDataB = 0;
			uchar maxDataG = 0;
			uchar maxDataR = 0;
			for (int i = 0; i < _cols*_rows; i++)
			{
				uchar dataB = ptr[i * 3];
				uchar dataG = ptr[i * 3 + 1];
				uchar dataR = ptr[i * 3] + 2;
				maxDataB = dataB > maxDataB ? dataB : maxDataB;
				maxDataG = dataG > maxDataG ? dataG : maxDataG;
				maxDataR = dataR > maxDataR ? dataR : maxDataR;
			}
			dst.push_back(maxDataB);
			dst.push_back(maxDataG);
			dst.push_back(maxDataR);
			//return ret;
			break;
		}
			
		case SIMG_1C8S:
		{
			//vector<char> ret;
			char * ptr = (char*)_dataPtr;
			char maxData = CHAR_MIN;
			for (int i = 0; i < _cols*_rows; i++)
			{
				char data = ptr[i];
				maxData = data > maxData ? data : maxData;
			}
			dst.push_back(maxData);
			//return ret;
			break;
		}
			
		case SIMG_3C8S:
		{
			//vector<char> ret;
			char * ptr = (char*)_dataPtr;
			char maxDataB = CHAR_MIN;
			char maxDataG = CHAR_MIN;
			char maxDataR = CHAR_MIN;
			for (int i = 0; i < _cols*_rows; i++)
			{
				char dataB = ptr[i * 3];
				char dataG = ptr[i * 3 + 1];
				char dataR = ptr[i * 3] + 2;
				maxDataB = dataB > maxDataB ? dataB : maxDataB;
				maxDataG = dataG > maxDataG ? dataG : maxDataG;
				maxDataR = dataR > maxDataR ? dataR : maxDataR;
			}
			dst.push_back(maxDataB);
			dst.push_back(maxDataG);
			dst.push_back(maxDataR);
			//return ret;
			break;
		}
			
		case SIMG_1C32F:
		{
			//vector<float> ret;
			float * ptr = (float*)_dataPtr;
			float maxData = FLT_MIN;
			for (int i = 0; i < _cols*_rows; i++)
			{
				float data = ptr[i];
				maxData = data > maxData ? data : maxData;
			}
			dst.push_back(maxData);
			//return ret;
			break;
		}
			
		case SIMG_3C32F:
		{
			//vector<float> ret;
			float * ptr = (float*)_dataPtr;
			float maxDataB = FLT_MIN;
			float maxDataG = FLT_MIN;
			float maxDataR = FLT_MIN;
			for (int i = 0; i < _cols*_rows; i++)
			{
				float dataB = ptr[i * 3];
				float dataG = ptr[i * 3 + 1];
				float dataR = ptr[i * 3] + 2;
				maxDataB = dataB > maxDataB ? dataB : maxDataB;
				maxDataG = dataG > maxDataG ? dataG : maxDataG;
				maxDataR = dataR > maxDataR ? dataR : maxDataR;
			}
			dst.push_back(maxDataB);
			dst.push_back(maxDataG);
			dst.push_back(maxDataR);
			//return ret;
			break;
		}

		case SIMG_1C16S:
		{
			short * ptr = (short*)_dataPtr;
			short maxData = SHRT_MIN;
			for (int i = 0; i < _cols*_rows; i++)
			{
				short data = ptr[i];
				maxData = data > maxData ? data : maxData;
			}
			dst.push_back(maxData);
			break;
		}
		default:
			break;
		}
		
	}

	template<typename dtype>
	inline void Mat::getMin(vector<dtype> &dst)
	{
		dst.clear();
		switch (_dataType)
		{
		case SIMG_1C8U:
		{
			uchar * ptr = (uchar*)_dataPtr;
			uchar minData = UCHAR_MAX;
			for (int i = 0; i < _cols*_rows; i++)
			{
				uchar data = ptr[i];
				minData = data < minData ? data : minData;
			}
			dst.push_back(minData);
			break;
		}

		case SIMG_3C8U:
		{
			//vector<uchar> ret;
			uchar * ptr = (uchar*)_dataPtr;
			uchar minDataB = UCHAR_MAX;
			uchar minDataG = UCHAR_MAX;
			uchar minDataR = UCHAR_MAX;
			for (int i = 0; i < _cols*_rows; i++)
			{
				uchar dataB = ptr[i * 3];
				uchar dataG = ptr[i * 3 + 1];
				uchar dataR = ptr[i * 3] + 2;
				minDataB = dataB < minDataB ? dataB : minDataB;
				minDataG = dataG < minDataG ? dataG : minDataG;
				minDataR = dataR < minDataR ? dataR : minDataR;
			}
			dst.push_back(minDataB);
			dst.push_back(minDataG);
			dst.push_back(minDataR);
			//return ret;
			break;
		}

		case SIMG_1C8S:
		{
			//vector<char> ret;
			char * ptr = (char*)_dataPtr;
			char minData = CHAR_MAX;
			for (int i = 0; i < _cols*_rows; i++)
			{
				char data = ptr[i];
				minData = data < minData ? data : minData;
			}
			dst.push_back(minData);
			//return ret;
			break;
		}

		case SIMG_3C8S:
		{
			//vector<char> ret;
			char * ptr = (char*)_dataPtr;
			char minDataB = CHAR_MAX;
			char minDataG = CHAR_MAX;
			char minDataR = CHAR_MAX;
			for (int i = 0; i < _cols*_rows; i++)
			{
				char dataB = ptr[i * 3];
				char dataG = ptr[i * 3 + 1];
				char dataR = ptr[i * 3] + 2;
				minDataB = dataB < minDataB ? dataB : minDataB;
				minDataG = dataG < minDataG ? dataG : minDataG;
				minDataR = dataR < minDataR ? dataR : minDataR;
			}
			dst.push_back(minDataB);
			dst.push_back(minDataG);
			dst.push_back(minDataR);
			//return ret;
			break;
		}

		case SIMG_1C32F:
		{
			//vector<float> ret;
			float * ptr = (float*)_dataPtr;
			float minData = FLT_MAX;
			for (int i = 0; i < _cols*_rows; i++)
			{
				float data = ptr[i];
				minData = data < minData ? data : minData;
			}
			dst.push_back(minData);
			//return ret;
			break;
		}

		case SIMG_3C32F:
		{
			//vector<float> ret;
			float * ptr = (float*)_dataPtr;
			float minDataB = FLT_MAX;
			float minDataG = FLT_MAX;
			float minDataR = FLT_MAX;
			for (int i = 0; i < _cols*_rows; i++)
			{
				float dataB = ptr[i * 3];
				float dataG = ptr[i * 3 + 1];
				float dataR = ptr[i * 3] + 2;
				minDataB = dataB < minDataB ? dataB : minDataB;
				minDataG = dataG < minDataG ? dataG : minDataG;
				minDataR = dataR < minDataR ? dataR : minDataR;
			}
			dst.push_back(minDataB);
			dst.push_back(minDataG);
			dst.push_back(minDataR);
			//return ret;
			break;
		}
		case SIMG_1C16S:
		{
			//vector<float> ret;
			short * ptr = (short*)_dataPtr;
			short minData = SHRT_MAX;
			for (int i = 0; i < _cols*_rows; i++)
			{
				short data = ptr[i];
				minData = data < minData ? data : minData;
			}
			dst.push_back(minData);
			break;
		}


		default:
			break;
		}

	}

	template<typename dtype>
	inline dtype & Mat::at(int col, int row, int ch)
	{
		// TODO: 在此处插入 return 语句
		assert(col >= 0 && col < _cols&&row >= 0 && row < _rows && _dataPtr != NULL && ch < _channels);
		modified();
		dtype* ptr = (dtype*)_dataPtr;
		return ptr[(col + row * _cols) * _channels + ch];
	}


}

