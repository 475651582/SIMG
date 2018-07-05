#pragma once
#include <assert.h>
#include <io.h>

template<class T> struct binaryTreeNode
{
	binaryTreeNode<T> *_left;
	binaryTreeNode<T> *_right;
	T _data;
	binaryTreeNode(const T data)
		:_data(data)
		, _left(NULL)
		, _right(NULL)
	{};
};

template<class T>
class Tree
{
	typedef binaryTreeNode<T> node;
public:
	Tree(const T element)
	{
		_root =new node(element);
	}
	~Tree()
	{

	}

	void insert(const T element, node* &t)
	{
		


	}

private:
	node* _root;
};

