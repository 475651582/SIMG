#include "stdafx.h"
#include <fstream>  
#include <string>  
#include <iostream> 
#include <vector>
#include "ML.h"


std::vector<std::string> split(std::string str, const char pattern){

	std::string::size_type pos;
	std::vector<std::string> result;
	str += pattern;//扩展字符串以方便操作
	int size = str.size();
	for (int i = 0; i<size; i++)
	{
		pos = str.find(pattern, i);
		if (pos<size)
		{
			std::string s = str.substr(i, pos - i);
			result.push_back(s);
			i = pos;
		}
	}

	return result;

};

void Simg::readDataset_DT(const char * filepath, const char parseChar, Mat & dataset, Mat & label)
{
	

	ifstream infile(filepath);
	if (!infile)
	{
		assert(false && "read file failed!");
	}

	string line;
	
	vector<vector<string>> str_dataset;
	int dataset_cols = -1;
	int dataset_rows = 0;
	while (getline(infile, line)) 
	{
		vector<string> strs = split(line, parseChar);
		if (dataset_cols != -1 && dataset_cols != strs.size())
		{
			assert(false && "file size mismatch!");	//	如果这一次读取和上一次读取的数据长度不一致，说明文件有问题，报错
		}
		else
		{
			dataset_cols = strs.size();
		}
		str_dataset.push_back(strs);
	}
	
	dataset_rows = str_dataset.size();

	dataset = Mat(dataset_cols - 1, dataset_rows, SIMG_1C16S);
	label = Mat(1, dataset_rows, SIMG_1C16S);

	for (size_t i = 0; i < dataset_rows; i++)
	{
		for (size_t j = 0; j < dataset_cols; j++)
		{
			string str = str_dataset[i][j];
			int data = atoi(str.c_str());
			if (j != dataset_cols - 1)
			{
				dataset.at<short>(j, i) = (short)data;
			}
			else
			{
				label.at<short>(0, i) = (short)data;
			}
			
		}
		

	}
	
	
}

void Simg::readDataset_DT2(const char * filepath, const char parseChar, Mat & dataset)
{
	ifstream infile(filepath);
	if (!infile)
	{
		assert(false && "read file failed!");
	}

	string line;

	vector<vector<string>> str_dataset;
	int dataset_cols = -1;
	int dataset_rows = 0;
	while (getline(infile, line))
	{
		vector<string> strs = split(line, parseChar);
		if (dataset_cols != -1 && dataset_cols != strs.size())
		{
			assert(false && "file size mismatch!");	//	如果这一次读取和上一次读取的数据长度不一致，说明文件有问题，报错
		}
		else
		{
			dataset_cols = strs.size();
		}
		str_dataset.push_back(strs);
	}
	dataset_rows = str_dataset.size();

	dataset = Mat(dataset_cols, dataset_rows, SIMG_1C16S);

	for (size_t i = 0; i < dataset_rows; i++)
	{
		for (size_t j = 0; j < dataset_cols; j++)
		{
			string str = str_dataset[i][j];
			int data = atoi(str.c_str());
			dataset.at<short>(j, i) = (short)data;
			

		}


	}
}

int Simg::cmp(const pair<int, float>& x, const pair<int, float>& y)
{
	if (x.second == y.second)
	{
		return x.first > y.first;
	}
	return x.second < y.second;
}

void Simg::sortMap(map<int, float>& tMap, vector<pair<int, float>>& tVector)
{
	for (map<int, float>::iterator it = tMap.begin(); it != tMap.end(); it++)
	{
		tVector.push_back(make_pair(it->first, it->second));
	}
	sort(tVector.begin(), tVector.end(), cmp);
}



