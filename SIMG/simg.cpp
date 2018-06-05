#include "stdafx.h"
#include "simg.h"

using namespace Simg;

std::vector<sWindow> windowsList;

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

	FILE *fp = fopen(path, "rb");
	assert(fp != 0);


	fread(&bf, sizeof(BITMAPFILEHEADER), 1, fp);
	fread(&bi, sizeof(BITMAPINFOHEADER), 1, fp);//读取BMP文件头文件信息

	w = bi.biWidth;                            //获取图像的宽
	h = bi.biHeight;                           //获取图像的高
	bitSize = bi.biSizeImage;                  //获取图像的size

	buf = new uchar[w*h * 3];                //分配缓冲区大小		
	fread(buf, 1, w*h * 3, fp);                   //开始读取数据

	ret = Mat(w, h, SIMG_3C8U);
	memcpy(ret.dataPtr(), buf, w*h * 3);

	delete buf; buf = NULL;

	fclose(fp);
	return ret;
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
	sWindow win(windowName, x, y, w, h);
	
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

int Simg::imshow(const char * windowName, Mat img)
{
	namedWindow(windowName, 0, 0, 0, img.cols(), img.rows());
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