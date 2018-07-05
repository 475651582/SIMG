#pragma once
#include <assert.h>
#include <io.h>
#include <iostream>

#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)<(b)?(a):(b))

template<class T> struct binaryTreeNode
{
	binaryTreeNode<T> *_left;
	binaryTreeNode<T> *_right;
	binaryTreeNode<T> *_dad;
	T _data;
	binaryTreeNode(const T data)
		:_data(data)
		, _left(NULL)
		, _right(NULL)
		, _dad(NULL)
	{};
};

template<class T> struct treeNode
{
	treeNode<T> *_child;
	treeNode<T> *_next;
	treeNode<T> *_dad;
	T _data;
	treeNode(const T data)
		:_data(data)
		, _next(NULL)
		, _child(NULL)
		, _dad(NULL)
	{};
};


template<class T>
class BinaryTree
{
	typedef binaryTreeNode<T> node;
public:
	BinaryTree(const T element)
	{
		_root = new node(element);
	}
	BinaryTree(node* root)
	{
		_root = root;
	}
	BinaryTree(node root)
	{
		_root = &root;
	}
	~BinaryTree()
	{
		makeEmpty(_root);
	}
	void remove(node* &_node)
	{
		makeEmpty(_node);
	}
	void preOrderPrint()
	{
		preOrder(_root);
		printf("\n");
	}
	void inOrderPrint()
	{
		inOrder(_root);
		printf("\n");
	}
	void postOrderPrint()
	{
		postOrder(_root);
		printf("\n");
	}

	void insert(const T element, node* &_node)
	{
		if (nullptr != _node)
		{
			insertNode(element, _node, _root);
		}
		else
		{
			assert(false && "try not to insert element to a null node!");
		}
		
	}
	vector<node*> find(const T element)
	{
		vector<node*> ret;
		findElement(element, _root, ret);
		return ret;
	}

	void print()
	{
		printNode(_root, 1);
		printf("\n");
	}

	int getDepth()
	{
		return getNodeDepth(_root);
	}

private:
	node * _root;

	int getNodeDepth(node *_node)
	{
		if (nullptr == _node)
		{
			return 0;
		}
		return  MAX(getNodeDepth(_node->_left), getNodeDepth(_node->_right)) + 1;
	}
	void insertNode(const T element, node *&_node, node *_dadNode)
	{
		if (nullptr == _node)
		{
			_node = new	node(element);	
			_node->_dad = _dadNode;
		}
		else if (nullptr == _node->_left)
		{
			insertNode(element, _node->_left, _node);
		}
		else if (nullptr == _node->_right)
		{
			insertNode(element, _node->_right, _node);
		}
		else
		{
			assert(false && "try not to insert a node to a full binary tree!");
		}
	}
	void makeEmpty(node* &_node)
	{
		if (nullptr != _node)
		{
			makeEmpty(_node->_left);
			makeEmpty(_node->_right);
			delete _node;
			_node = nullptr;
		}
	}
	void printNode(node* &_node, int rootDepth)
	{
		if (nullptr != _node)
		{
			printNode(_node->_right, rootDepth + 1);
			for (int i = 0; i < rootDepth; i++)
			{
				printf("\t");
			}
			cout << _node->_data << endl;
			printNode(_node->_left, rootDepth + 1);
		}
	}
	void preOrder(node* &_node)
	{
		if (nullptr != _node)
		{
			cout << _node->_data << "  ";
			preOrder(_node->_left);
			preOrder(_node->_right);
		}
	}
	void inOrder(node* &_node)
	{
		if (nullptr != _node)
		{
			inOrder(_node->_left);
			cout << _node->_data << "  ";
			inOrder(_node->_right);
		}
	}
	void postOrder(node* &_node)
	{
		if (nullptr != _node)
		{
			postOrder(_node->_left);
			postOrder(_node->_right);
			cout << _node->_data << "  ";
		}
	}

	void findElement(const T element, node* _node, vector<node*> &list)
	{
		if (nullptr != _node)
		{
			
			findElement(element, _node->_left, list);
			findElement(element, _node->_right, list);
			if (element == _node->_data)
			{
				list.push_back(_node);
			}
		}
	}
};

template<class T>
class Tree
{
	typedef treeNode<T> node;
public:
	Tree(const T element)
	{
		_root = new node(element);
	}
	Tree(node* root)
	{
		_root = root;
	}
	Tree(node root)
	{
		_root = &root;
	}
	~Tree()
	{
		
	}

	void print()
	{
		printNode(_root);
	}
	int depth()
	{
		return getNodeDepth(_root);
	}
	
	

	void insert(const T element, node* &_node)
	{
		if (nullptr != _node)
		{
			insertNode(element, _node);
		}
		else
		{
			assert(false && "try not to insert element to a null node!");
		}

	}

	void removeNode(node *&_node)
	{

		if (nullptr != _node)
		{
			removeNode(_node->_child);			
			

			
			delete _node; _node = NULL;
		}
	}
	

private:
	node * _root;

	
	void insertNode(const T element, node *&_dadNode)
	{
		node* newNode = new node(element);
		newNode->_dad = _dadNode;
		if (nullptr == _dadNode->_child)
		{
			_dadNode->_child = newNode;
			
		}
		else
		{
			node* nextNode = _dadNode->_child;
			
			while (nextNode)
			{
				if (nullptr == nextNode->_next)
				{
					nextNode->_next = newNode;
					break;
				}
				else
				{
					nextNode = nextNode->_next;
				}
				
			}
			
		}
	}
	
	void printNode(node *&_node)
	{
		
		if (nullptr != _node)
		{
			printNode(_node->_child);
			printNode(_node->_next);

			std::cout << _node->_data << " ";
		}
	}
	
	int getNodeDepth(node *_node)
	{
		if (nullptr == _node)
		{
			return 0;
		}
		return  MAX(getNodeDepth(_node->_child) + 1, getNodeDepth(_node->_next));
	}


	
};
