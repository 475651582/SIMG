#pragma once
#include "simgMat.h"
#include <map>
#include <algorithm>

namespace Simg
{
	void readDataset_DT(const char* filepath, const char parseChar, Mat& dataset, Mat& label);

	void readDataset_DT2(const char* filepath, const char parseChar, Mat& dataset);

	int cmp(const pair<int, float>&x, const pair<int, float>&y);

	void sortMap(map<int, float> &tMap, vector<pair<int, float>> &tVector);

	template<class T>
	struct ddata
	{
	public:
		int number = 0;
		vector<T> attri;
		int label = -1;
	};


	template<class T>
	class Dataset
	{
	public:
		Dataset(Mat originMat)
		{
			loadFromMat(originMat);
		}
		Dataset() { ; };


		void loadFromMat(Mat originMat);

		ddata<T>& operator[](int i)
		{
			return _datalist[i];
		}

		float labelProbility(int labelValue, int attribute = -1, int attributeValue = 0);


		vector<map<int, int>> attriMap(){ return _attributeMap; };


		int attributeNum() { return _attributeNum; };
		int datasetNum() { return _datasetNum; };
		void remove(int num);
		void insert(ddata<T> newData);

		Dataset<T> subDataset(int attribute, int attributeValue, int labelValue = -1)
		{
			Dataset<T> ret;
			for (size_t i = 0; i < _datasetNum; i++)
			{
				ddata<T> tmp = _datalist[i];
				if (tmp.attri[attribute] == attributeValue)
				{
					if (labelValue == -1)
					{
						ret.insert(tmp);
					}
					else if(tmp.label == labelValue)
					{
						ret.insert(tmp);
					}
				}
			}

			ret._attributeNum = _attributeNum;
			ret.updateDataset();
			return ret;
		}


		vector<int> optimumAttribute(vector<int>A);


	private:
		vector<ddata<T>> _datalist;
		vector<map<int, int>> _attributeMap;
		void updateDataset();
		
		int _attributeNum = 0;
		int _datasetNum = 0;
		Mat _originDataMat;

		
	};
	template<class T>
	inline void Dataset<T>::loadFromMat(Mat originMat)
	{
		_originDataMat = originMat.copy();
		_attributeNum = _originDataMat.cols() - 1;
		int attributePostion = _originDataMat.cols() - 2;
		int labelPosition = _originDataMat.cols() - 1;
		_datasetNum = _originDataMat.rows();

		_attributeMap = vector<map<int, int>>(_attributeNum);
		for (int i = 0; i < _datasetNum; i++)
		{
			ddata<T> tmp;
			tmp.number = i;
			for (int j = 0; j < _attributeNum; j++)
			{
				
				T value = _originDataMat.at<T>(j, i);
				_attributeMap[j][value]++;
				tmp.attri.push_back(value);
			}
			tmp.label = _originDataMat.at<T>(labelPosition, i);
			_datalist.push_back(tmp);
		}
	}
	template<class T>
	inline float Dataset<T>::labelProbility(int labelValue, int attri, int attributeValue)
	{
		//统计所有属性的概率
		if (-1 == attri)
		{
			int sum = 0;
			for (size_t i = 0; i < _datasetNum; i++)
			{
				ddata<T> tmp = _datalist[i];
				if (tmp.label == labelValue)
				{
					sum++;
				}
			}
			return 1.0f * sum / _datasetNum;
		}
		else
		{
			int sum = 0, sumLabeled = 0;
			for (size_t i = 0; i < _datasetNum; i++)
			{
				ddata<T> tmp = _datalist[i];
				if (tmp.attri[attri] == attributeValue)
				{
					sum++;
					if (labelValue == tmp.label)
					{
						sumLabeled++;
					}
				}
			}
			return 1.0f * sumLabeled / sum;
		}
	}
	
	

	template<class T>
	inline void Dataset<T>::remove(int num)
	{
		for (size_t i = 0; i < _datalist.size(); i++)
		{
			ddata<T> tmp = _datalist[i];
			if (tmp.number == num)
			{
				_datalist.erase(_datalist.begin() + i);
				break;
			}
		}
		
		updateDataset();
	}

	template<class T>
	inline void Dataset<T>::insert(ddata<T> newData)
	{
		_datalist.push_back(newData);
		updateDataset();
	}

	



	template<class T>
	inline vector<int> Dataset<T>::optimumAttribute(vector<int> A)
	{
		vector<int> ret;
		sort(A.begin(), A.end());
		float p1 = 0, p2 = 0;

		p1 = labelProbility(1);
		p2 = labelProbility(0);

		float EntD = -p1 * log2(p1) - p2 * log2(p2);
		if (p1 == 0)
		{
			EntD = -0 - p2 * log2(p2);
		}
		else if (p2 == 0) //p1 & p2 can never be 0 together
		{
			EntD = -p1 * log2(p1) - 0;
		}
		else
		{
			EntD = -p1 * log2(p1) - p2 * log2(p2);
		}
		float EntDv = 0;
		float GainDi = 0;
		vector<map<int, int>> attriMap = _attributeMap;
		map<int, float>GainD;
		for (size_t i = 0; i < A.size(); i++)
		{
			int Dv = 0;
			GainDi = 0;
			
			map<int, int>::iterator it;
			for (it = attriMap[A[i]].begin(); it != attriMap[A[i]].end(); it++)
			{
				Dv = it->second;
				p1 = labelProbility(1, A[i], it->first);
				p2 = labelProbility(0, A[i], it->first);
				if (p1 == 0)
				{
					EntDv = -0 - p2 * log2(p2);
				}
				else if (p2 == 0) //p1 & p2 can never be 0 together
				{
					EntDv = -p1 * log2(p1) - 0;
				}
				else
				{
					EntDv = -p1 * log2(p1) - p2 * log2(p2);
				}
				GainDi = GainDi - abs(1.0 * Dv / _datasetNum)*EntDv;

			}
			GainDi = EntD + GainDi;
			GainD[A[i]] = GainDi;
		}

		vector<pair<int, float>> tVector;
		sortMap(GainD, tVector);

		vector<pair<int, float>>::iterator it = tVector.begin();
		while (it != tVector.end())
		{
			int attr = it->first;

			ret.push_back(attr);
			it++;
		}

		return ret;
	}



	template<class T>
	inline void Dataset<T>::updateDataset()
	{
		_attributeMap.clear();
		_attributeMap = vector<map<int, int>>(_attributeNum);
		_datasetNum = _datalist.size();
		for (size_t i = 0; i < _datasetNum; i++)
		{
			ddata<T> tmp = _datalist[i];
			for (size_t j = 0; j < _attributeNum; j++)
			{
				_attributeMap[j][tmp.attri[j]]++;
			}
		}
	}
}

