#pragma once
#include"linkedBinaryTree.h"
#include<list>


template<class T>
struct huffmanNode
{
	linkedBinaryTree<T>* tree;
	int weight;

	bool operator<(huffmanNode<T>t2) {
		return weight < t2.weight;
	}
};

template<class T>
class cmp {
public:
	bool operator()(T t1, T t2) {
		return t1< t2;
	}
};
template<class T>
class pQueue {
public:
	list<T> l;
	void pop() {
		l.pop_front();
	}
	T top() {
		return l.front();
	}
	void push(T t) {
		l.push_back(t);
		l.sort(cmp<T>());
		//cout << l.front().tree->root->element << l.front().weight<< endl;//test
	}
	void initial(T* t, int n) {
		for (int i = 1; i <= n; i++) {
			l.push_back(t[i]);
		}
	}
};


class huffmanTree
{
public:
	linkedBinaryTree<char>* func(int weight[], int n,char elemnet[]) {
		huffmanNode<char>* hNode = new huffmanNode<char>[n + 1];
		linkedBinaryTree<char> emptyTree;
		//单节点树
		for (int i = 1; i <= n; i++) {
			hNode[i].weight = weight[i];
			hNode[i].tree = new linkedBinaryTree<char>;
			hNode[i].tree->makeTree(elemnet[i], emptyTree, emptyTree);
		}
		pQueue<huffmanNode<char>> heap;
		heap.initial(hNode, n);

		huffmanNode<char> w, x, y;
		linkedBinaryTree<char>* z;
		for (int i = 1; i < n; i++) {
			//取出最小两颗树
			x = heap.top();
			heap.pop();
			y = heap.top();
			heap.pop();

			if (x.weight > y.weight)swap(x, y);//让左边的小
			//合并
			z = new linkedBinaryTree<char>;
			z->makeTree('$', *x.tree, *y.tree);
			w.weight = x.weight + y.weight;
			w.tree = z;
			heap.push(w);
			delete x.tree;
			delete y.tree;
		}
		return heap.top().tree;
	}

	//simplified
	linkedBinaryTree<char>* func(int weight[], int n) {
		//只关心权，并不关心Huffman树上到底挂的是什么，char比int省啊

		huffmanNode<char>* hNode = new huffmanNode<char>[n + 1];
		linkedBinaryTree<char> emptyTree;
		//单节点树
		for (int i = 1; i <= n; i++) {
			hNode[i].weight = weight[i];
			hNode[i].tree = new linkedBinaryTree<char>;
			hNode[i].tree->makeTree('a', emptyTree, emptyTree);
		}
		pQueue<huffmanNode<char>> heap;
		heap.initial(hNode, n);

		huffmanNode<char> w, x, y;
		linkedBinaryTree<char>* z;
		for (int i = 1; i < n; i++) {
			//取出最小两颗树
			x = heap.top();
			heap.pop();
			y = heap.top();
			heap.pop();

			if (x.weight > y.weight)swap(x, y);//让左边的小

			//合并
			z = new linkedBinaryTree<char>;
			z->makeTree('0', *x.tree, *y.tree);
			w.weight = x.weight + y.weight;
			w.tree = z;
			heap.push(w);
			delete x.tree;
			delete y.tree;
		}
		return heap.top().tree;
	}
};

