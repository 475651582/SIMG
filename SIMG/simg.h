#pragma once
#include <Windows.h>
#include "simgMat.h"

namespace Simg
{
	Mat imread(const char *path);
	Mat readBMP(const char *path);

	int namedWindow(const char *windowName, int windowStyle = 0);
	int waitKey();
	LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

	static HINSTANCE hg_hinstance = 0;
	//HWND hDlgModeless = NULL;//保存全局非模态对话框
}

