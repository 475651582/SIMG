#include "stdafx.h"
#include "simg.h"

using namespace Simg;

std::vector<Simg::sWindow> windowsList;

Mat Simg::readBMP(const char *path)
{
	//bmp存储方式是以左下角第一个像素为坐标原点的，和通常的使用习惯不太一致，需在索引的时候注意。
	Mat ret;
	DWORD w, h;								//定义读取图像的长和宽
											//DWORD bitCorlorUsed;					//定义
	DWORD bitSize;							//定义图像的大小
	BITMAPFILEHEADER bf;					//图像文件头
	BITMAPINFOHEADER bi;					//图像文件头信息
	uchar *buf;								//定义文件读取缓冲区
	RGBQUAD *rgbquad = NULL;
	int channels = 3;						//默认图像为RGB三通道
	FILE *fp = fopen(path, "rb");
	assert(fp != 0);


	fread(&bf, sizeof(BITMAPFILEHEADER), 1, fp);
	fread(&bi, sizeof(BITMAPINFOHEADER), 1, fp);//读取BMP文件头文件信息

	

	w = bi.biWidth;                            //获取图像的宽
	h = bi.biHeight;                           //获取图像的高
	bitSize = bi.biSizeImage;                  //获取图像的size


	switch (bi.biBitCount)
	{
	case 24:	//24位图
	{
		channels = 3;

		buf = new uchar[w * h * channels];                //分配缓冲区大小		
		fread(buf, 1, w * h * channels, fp);                  //开始读取数据
		ret = Mat(w, h, SIMG_3C8U);
		uchar* ret_buffer = ret.dataPtr();
		for (size_t i = 0; i < h; i++)
		{
			memcpy(ret_buffer + i * w * channels, buf + (h - i - 1) * w * channels, w * channels);  //copy bmp data to the new Mat, need to convert the direction
		}

		
		fclose(fp);
		delete buf; buf = NULL;
		return ret;
		break;
	}
	case 8:		//8位图
	{
		channels = 1;
		rgbquad = new RGBQUAD[256];
		fread(rgbquad, sizeof(RGBQUAD), 256, fp);

		buf = new uchar[w * h * channels];                //分配缓冲区大小		
		fread(buf, 1, w * h * channels, fp);                  //开始读取数据
		ret = Mat(w, h, SIMG_1C8U);
		uchar* ret_buffer = ret.dataPtr();
		for (size_t i = 0; i < h; i++)
		{
			memcpy(ret_buffer + i * w * channels, buf + (h - i - 1) * w * channels, w * channels);  //copy bmp data to the new Mat, need to convert the direction
		}


		fclose(fp);
		delete buf; buf = NULL;
		delete rgbquad; rgbquad = NULL;
		return ret;
		break;
	}
	default:
		//reserved
		return ret;
		break;
	}


}

Mat Simg::readJPG(const char * path)
{
	Mat ret;
	int w = 0, h = 0, comps = 0;
	int channels = 3;
	uchar *buf = jpgd::decompress_jpeg_image_from_file(path, &w, &h, &comps, 3);
	ret = Mat(w, h, SIMG_3C8U);
	memcpy(ret.dataPtr(), buf, w * h * channels);	//copy bmp data to the new Mat

	delete buf; buf = NULL;
	return ret;
}

void Simg::split(Mat src, Mat * dst)
{
	assert(src._channels > 1 && dst != NULL);
	int newDataType = 0;
	switch (src._dataType)
	{
	case SIMG_3C8U:
		newDataType = SIMG_1C8U;
		break;
	case SIMG_3C8S:
		newDataType = SIMG_1C8S;
		break;
	case SIMG_3C16U:
		newDataType = SIMG_1C16U;
		break;
	case SIMG_3C16S:
		newDataType = SIMG_1C16S;
		break;
	case SIMG_3C32F:
		newDataType = SIMG_1C32F;
		break;
	case SIMG_3C64F:
		newDataType = SIMG_1C64F;
		break;
	default:
		break;
	}
	
	for (size_t ch = 0; ch < src._channels; ch++)
	{
		dst[ch] = Mat(src._cols, src._rows, newDataType);
		for (size_t i = 0; i < src._cols * src._rows; i++)
		{
			dst[ch]._dataPtr[i] = src._dataPtr[src._channels * i + ch];
		}
	}
	return;
}


Mat Simg::imread(const char* path)
{
	Mat ret;
	string s_path;
	assert(NULL != path); //path should have a value
	assert(_access(path, 0) == 0);

	//提取后缀名
	s_path = path;
	int ind_suffix = s_path.rfind('.');
	string suffix = s_path.substr(ind_suffix + 1, s_path.length());

	if ("bmp" == suffix)
	{
		ret = Simg::readBMP(path);
		ret._originalFormat = SIMG_FORMAT_IMG_BMP;
	}
	else if ("jpg" == suffix)
	{
		ret = Simg::readJPG(path);
		ret._originalFormat = SIMG_FORMAT_IMG_JPEG;
	}
	else if ("png" == suffix)
	{
		ret._originalFormat = SIMG_FORMAT_IMG_PNG;
	}
	else
	{
		//如果没有找到支持的后缀名
	}

	return ret;
}


int Simg::namedWindow(const char * windowName, int windowStyle, int x, int y, int w, int h)
{


	//先检测是否存在这个窗体
	for (size_t i = 0; i < windowsList.size(); i++)
	{
		sWindow win = windowsList[i];
		if (0==strcmp(windowName, win.windowName()))
		{
			//已经建立过同名窗体，跳过
			return -1;
		}
	}
	sWindow win(windowName, x, y, w, h, windowStyle);
	
	windowsList.push_back(win);

	std::vector<sWindow> aaa;
	aaa.push_back(win);

	return 1;

}



int Simg::waitKey()
{
	MSG  msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}

int Simg::imshow(const char * windowName, Mat img, int windowStyle)
{
	namedWindow(windowName, windowStyle, 0, 0, img.cols(), img.rows());
	for (size_t i = 0; i < windowsList.size(); i++)
	{
		sWindow *win = &windowsList[i];
		if (0 == strcmp(windowName, win->windowName()))
		{
			win->loadMat(img);
		}
	}

	return 0;
}