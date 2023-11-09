#pragma once
#include<iostream>
using namespace std;

template<class T>
class binaryTreeNode
{
public:
	T element;
	binaryTreeNode<T>* leftChild, * rightChild;

	binaryTreeNode() {
		leftChild = rightChild = NULL;
	}
	binaryTreeNode(T& theElement) {
		leftChild = rightChild = NULL;
		element = theElement;
	}
	binaryTreeNode(T& theElement, binaryTreeNode* leftT, binaryTreeNode* rightT) {
		leftChild = leftT;
		rightChild = rightT;
		element = theElement;
	}
};