#pragma once

#include"binaryTreeNode.h"
#include<iostream>
using namespace std;

template<class T>
class linkedBinaryTree
{
public:
	binaryTreeNode<T>* root;
	int treeSize;
	
	static int count;

	//静态方法，树公用，给定结点即使用
	static void addCount(binaryTreeNode<T>* t) { count++; }
	static void dispose(binaryTreeNode<T>* t) { if(t!=NULL) delete t; }
	static void output(binaryTreeNode<T>* t) {
		cout << t->element << " ";
	}
	static void countNodes(binaryTreeNode<T>* t) {
		visit = addCount;
		count = 0;
		preOrder(t);
	}
	static int height(binaryTreeNode<T>* t);
	static void (*visit)(binaryTreeNode<T>*);

	static void preOrder(binaryTreeNode<T>* t) {//遍历
		if (t != NULL)
		{
			linkedBinaryTree<T>::visit(t);
			preOrder(t->leftChild);
			preOrder(t->rightChild);
		}
	}
	static void inOrder(binaryTreeNode<T>* t) {
		if (t != NULL)
		{
			inOrder(t->leftChild);
			linkedBinaryTree<T>::visit(t);	
			inOrder(t->rightChild);
		}
	}
	static void postOrder(binaryTreeNode<T>* t) {
		if (t != NULL)
		{
			postOrder(t->leftChild);
			postOrder(t->rightChild);
			linkedBinaryTree<T>::visit(t);
		}
	}
	

	linkedBinaryTree() {
		root = NULL;
		treeSize = 0;
	}
	~linkedBinaryTree() {
		erase();
	}
	
	bool empty() { return treeSize == 0; }
	int size() { return treeSize; }
	int height() { return height(root); }
	void erase() {//清空
		postOrder(dispose);
		root = NULL;
		treeSize = 0;
	}

	void preOrder(void (*theVisit)(binaryTreeNode<T>*)) {//给定vist函数进行遍历
		visit = theVisit;
		preOrder(root);
	}
	void inOrder(void (*theVisit)(binaryTreeNode<T>*)) {
		visit = theVisit;
		inOrder(root);
	}
	void postOrder(void (*theVisit)(binaryTreeNode<T>*)) {
		visit = theVisit;
		postOrder(root);
	}

	void makeTree(const T& element, linkedBinaryTree<T>&, linkedBinaryTree<T>&);
	linkedBinaryTree<T>& removeLeft();
	linkedBinaryTree<T>& removeRight();
};

void (*linkedBinaryTree<int>::visit)(binaryTreeNode<int>*);
void (*linkedBinaryTree<char>::visit)(binaryTreeNode<char>*);
void (*linkedBinaryTree<pair<int, int> >::visit)(binaryTreeNode<pair<int, int> >*);
void (*linkedBinaryTree<pair<const int, char> >::visit)(binaryTreeNode<pair<const int, char> >*);
void (*linkedBinaryTree<pair<const int, int> >::visit)(binaryTreeNode<pair<const int, int> >*);

template<class T>
int linkedBinaryTree<T>::height(binaryTreeNode<T>* t)
{
	if (t == NULL)return 0;
	int hl = height(t->leftChild);
	int hr = height(t->rightChild);
	if (hl > hr)
		return ++hl;
	else
		return ++hr;
}

template<class T>
void linkedBinaryTree<T>::makeTree(const T& element, linkedBinaryTree<T>& leftTree, linkedBinaryTree<T>& rightTree)
{
	//将左右树与element结合成一个新树
	root = new binaryTreeNode<T>(element, leftTree.root, rightTree.root);
	treeSize = leftTree.treeSize + rightTree.treeSize + 1;

	leftTree.root = rightTree.root = NULL;
	leftTree.treeSize = rightTree.treeSize = 0;
}

template<class T>
linkedBinaryTree<T>& linkedBinaryTree<T>::removeLeft()
{
	if (empty()) {
		cout << "empty" << endl;
		return linkedBinaryTree<T>();
	}

	linkedBinaryTree<T> leftSubTree;
	leftSubTree.root = root->leftChild;
	count = 0;
	leftSubTree.treeSize = countNodes(leftSubTree.root);//从左子树根往下数
	root->leftChild = NULL;
	treeSize -= leftSubTree.treeSize;

	return leftSubTree;

}

template<class T>
linkedBinaryTree<T>& linkedBinaryTree<T>::removeRight()
{
	if (empty()) {
		cout << "empty" << endl;
		return linkedBinaryTree<T>();
	}

	linkedBinaryTree<T> rightSubTree;
	rightSubTree.root = root->rightChild;
	count = 0;
	rightSubTree.treeSize = countNodes(rightSubTree.root);//从左子树根往下数
	root->rightChild = NULL;
	treeSize -= rightSubTree.treeSize;

	return rightSubTree;

}



