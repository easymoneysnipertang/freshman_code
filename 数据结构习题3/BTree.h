//BTREE类

#ifndef BTREE_H
#define BTREE_H

#include "BNode.h"
#include <list>
#include <queue>
#include <set>
#include <map>
#include <vector>
#include <algorithm>
#include <iostream>
using namespace std;

template<class T>
class BTree
{
protected:
	BNode <T> *root;//根节点指针
	int k;	//B树为k阶
	int minKey, maxKey; //最少键值和最多键值
	int nsize;  //B数结构元素（键值）总数
	void maintainAfterInsert(BNode<T> *);	//插入元素后保持树形结构平衡
	void maintainAfterErase(BNode<T>*);  //删出元素后保持树形结构平衡
	void mergeNode(BNode<T>*, BNode<T>*, T, bool flag);//合并两个节点，flag为0时，表示合并左节点；否则合并右节点
	void deleteNode(BNode<T> *);	//删除节点空间，用于析构
	void print(BNode<T>*);	//输出节点
	BNode<T>* splitLeftNode(BNode<T>*, int);	//节点分裂得到左节点
	BNode<T>* splitRightNode(BNode<T>*, int);	//节点分裂得到右节点
	BNode<T>* findBrotherNode(BNode<T>*,T&,bool flag);//查找兄弟节点,flag为0时，查找左节点；否则为右节点
	BNode<T>* findNode(T);	//获取存在该键值的节点
    
public:
	BTree();
	~BTree();
	BTree(int k);	//构造，传入B树阶数
	bool insert(T data);	//插入
	bool erase(T key);  //删除
	void printRoot();	//输出根节点
	void printArray();	//输出排序结果
	void printTree();	//输出树型
	void printWholeTree();	//输出完整树型
	BNode<T>* findNextNode(BNode<T>*p, T, T&);  //获取后继节点及键值
	BNode<T>* findPreNode(BNode<T>*p, T, T&);	//获取前驱节点及键值
	int size();	//获取树元素总数
	int height();	//获取树高度
	BNode<T>* getRoot();	//获取根节点指针

private:
	int getLen(int);	//辅助函数，获取数字长度
	void GetDFS(BNode<T>*p, int d, int* dep, int *tab, map<T, int>&mp, int& cnt);	//辅助函数，用于递归获取输出树型的信息
};


template<class T>
bool BTree<T>::insert(T key)
{
	BNode<T>* p = root;
	if (root == NULL) {
		root = new BNode<T>(key);
		nsize++;
		return 1;
	}
	while (!p->isLeaf()) {//只在叶节点插入
		if (p->hasKey(key))return false;
		p = p->next(key);
	}
	if (p->hasKey(key))return false;
	p->insertKey(key, NULL, NULL);
	maintainAfterInsert(p);
	nsize++;
	return 1;
}

template<class T>
bool BTree<T>::erase(T key) {
	BNode<T>* p = findNode(key);
	if (!p)return false; //未找到key
	if (!p->isLeaf()) {
		T next_key;
		BNode<T>* next_node = findNextNode(p, key, next_key);
		p->replace(key, next_key);
		key = next_key;
		p = next_node;
	}
	p->eraseKey(key);//叶节点删key，child随便删
	p->eraseChild(NULL);
	this->maintainAfterErase(p);
	nsize--;
	return true;
}

template<class T>
void BTree<T>::maintainAfterErase(BNode<T>* p) {
	BNode<T>* fa = p->fa;
	if (p == root) {
		if (p->size() == 0) {
			root = *(root->child.begin());
			if (root) root->fa = NULL;
		}
		return;
	}
	if (p->size() >= minKey)return;//删完还够
	T father_key;
	BNode<T>* brother_node;
	brother_node = findBrotherNode(p, father_key, 0);//0左，1右
	//向兄弟借
	if (brother_node)mergeNode(p, brother_node, father_key, 0);//存在左节点则合并左节点
	else {
		brother_node = findBrotherNode(p, father_key, 1);//0左，1右
		mergeNode(p, brother_node, father_key, 1);
	}
	this->maintainAfterErase(fa);//往上调整
}

template<class T>
void BTree<T>::mergeNode(BNode<T>* p1, BNode<T>* p2, T father_key, bool flag) {
	BNode<T>* fa = p1->fa;
	if (p2->size() > minKey) {
		int replace_key = flag ? *(p2->keys.begin()) : *(p2->keys.rbegin());//从兄弟拿一个key放父节点
		BNode<T>* replace_child = flag ? *(p2->child.begin()) : *(p2->child.rbegin());//把父节点和兄弟的孩子移过来
		p2->eraseKey(replace_key);
		p2->eraseChild(replace_child);
		fa->replace(father_key, replace_key);
		if (flag == 0) {
			p1->keys.push_front(father_key);
			p1->child.push_front(replace_child);
		}
		else {
			p1->keys.push_back(father_key);
			p1->child.push_back(replace_child);
		}
		p1->updateChild();//更新兄弟节点拿过来的孩子的父节点指针信息
	}
	else {//没得借了
		fa->eraseKey(father_key);
		fa->eraseChild(p1);
		if (flag == 0) {
			p2->keys.push_back(father_key);
			p2->keys.splice(p2->keys.end(), p1->keys);//整体剪切过去
			p2->child.splice(p2->child.end(), p1->child);
		}
		else {
			p2->keys.push_front(father_key);
			p2->keys.splice(p2->keys.begin(), p1->keys);
			p2->child.splice(p2->child.begin(), p1->child);
		}
		fa->updateChild();
		p2->updateChild();
		delete p1;
	}
}

template<class T>
BNode<T>* BTree<T>::findBrotherNode(BNode<T>* p, T& key, bool flag) {
	BNode<T>* fa = p->fa;
	typename std::list<T>::iterator it_key = fa->keys.begin();
	typename std::list<BNode<T>*>::iterator it_child = fa->child.begin();
	while (*it_child != p) {
		it_child++; it_key++;
	}
	if (flag == 0) {//左兄弟
		if (it_child == fa->child.begin())return NULL;//没有左兄弟
		key = *(--it_key);
		return *(--it_child);
	}
	else {
		if ((++it_child) == fa->child.end())return NULL;
		key = *it_key;//前面++了
		return  *it_child;
	}
}

template<class T>
BNode<T>* BTree<T>::findNextNode(BNode<T>* p, T key, T& next_key) {
	if (!p)return NULL;
	typename std::list<T>::iterator it_key;
	typename std::list<BNode<T>*>::iterator it_child;
	do {
		it_key = p->keys.begin();
		it_child = p->child.begin();
		while (it_key != p->keys.end()) {
			if (key < *it_key)break;
			it_child++; it_key++;
		}
		if (it_key != p->keys.end())next_key = *it_key;//以引用的方式返回后驱key值
		if (*it_child == NULL) //直接前驱后驱只会在叶节点
			return p;//返回后驱节点
	} while (p = *it_child);//到孩子节点那一层去
}

template<class T>
BNode<T>* BTree<T>::findPreNode(BNode<T>* p, T key, T& pre_key)
{
	if (!p)return NULL;
	typename std::list<T>::reverse_iterator it_key;
	typename std::list<BNode<T>*>::reverse_iterator it_child;
	do {
		it_key = p->keys.rbegin();//从后往前走，找直接前驱
		it_child = p->child.rbegin();
		while (it_key != p->keys.rend()) {
			if (*it_key < key)break;
			it_child++; it_key++;
		}
		if (it_key != p->keys.rend())pre_key = *it_key;
		if (*it_child == NULL)
			return p;
	} while (p = *it_child);
}


template<class T>
BNode<T>* BTree<T>::findNode(T key) {
	BNode<T>* p = root;
	while (p && !(p->hasKey(key))) {
		p = p->next(key);
	}
	return p;
}

template<class T>
void BTree<T>::maintainAfterInsert(BNode<T>* p)
{
	while (p->size() >= k) {//插满分裂
		BNode<T>* left_node = splitLeftNode(p, (k - 1) / 2);
		BNode<T>* right_node = splitRightNode(p, k - (k - 1) / 2 - 1);
		T key = p->getKey((k - 1) / 2);	//上移的值
		delete p;//当前节点已经完全分成两半与留作一个上移的key
		p = left_node->fa;	//父节点
		if (!p) {//树长高
			root = p = new BNode<T>();
			left_node->fa = right_node->fa = root;
		}
		p->insertKey(key, left_node, right_node);
	}//p已经指向父节点，继续向上判断
}

template<class T>
BNode<T>* BTree<T>::splitLeftNode(BNode<T>* p, int pos)
{
	BNode<T>* node = new BNode<T>();
	node->fa = p->fa;
	typename std::list<T>::iterator it_key = p->keys.begin();
	typename std::list<BNode<T>*>::iterator it_child = p->child.begin();
	node->child.push_back(*it_child);
	int i = 0;
	while (i++ < pos) {
		it_child++;
		node->keys.push_back(*it_key);
		node->child.push_back(*it_child);
		it_key++;
	}
	node->updateChild();
	return node;
}

template<class T>
BNode<T>* BTree<T>::splitRightNode(BNode<T>* p, int pos)
{
	BNode<T>* node = new BNode<T>();
	node->fa = p->fa;
	typename std::list<T>::reverse_iterator it_key = p->keys.rbegin();
	typename std::list<BNode<T>*>::reverse_iterator it_child = p->child.rbegin();
	node->child.push_front(*it_child);
	int i = 0;
	while (i++ < pos) {
		it_child++;
		node->keys.push_front(*it_key);
		node->child.push_front(*it_child);
		it_key++;
	}
	node->updateChild();
	return node;
}

template <class T>
int BTree<T>::size() {
	return nsize;
}

template<class T>
int BTree<T>::height()
{
	int i = 0;
	BNode<T>* p = root;
	while (p) {
		p = *(p->child.begin());
		++i;
	}
	return i;
}

template<class T>
BNode<T>* BTree<T>::getRoot()
{
	return this->root;
}

template<class T>
void BTree<T>::deleteNode(BNode<T>* p)
{
	if (!p)return;
	typename std::list<BNode<T>*>::iterator it;
	for (it = p->child.begin(); it != p->child.end(); it++) {
		deleteNode(*it);
	}
	delete p;
}

template <class T>
BTree<T>::BTree()
{
	k = 3;
	nsize = 0;
	maxKey = k - 1;
	minKey = (k - 1) / 2;
	root = NULL;
}

template<class T>
BTree<T>::BTree(int k) :k(k)
{
	nsize = 0;
	maxKey = k - 1;
	minKey = (k - 1) / 2;
	root = NULL;
}

template<class T>
void BTree<T>::printArray() {
	print(root);
	cout << endl;
}

template<class T>
void BTree<T>::print(BNode<T>* p) {//中序遍历
	if (p == NULL)return;
	typename std::list<T>::iterator it_key = p->keys.begin();
	typename std::list<BNode<T>*>::iterator it_child = p->child.begin();
	print(*it_child);
	for (it_child++; it_child != p->child.end(); it_child++, it_key++) {
		cout << *it_key << ' ';
		print(*it_child);
	}
}

template<class T>
void BTree<T>::printRoot()
{
	if (!root)return;
	root->print();
	cout << endl;
}

template<class T>
void BTree<T>::printTree() {//层次遍历
	if (!root) {
		cout << "空树~" << endl;
		return;
	}
	queue<BNode<T>*>q;
	set<BNode<T>*>s; s.clear(); s.insert(root);
	q.push(root);
	while (!q.empty()) {
		BNode<T>* p = q.front(); q.pop();
		cout << '|'; p->print(); cout << '|';
		if (p->isLeaf())continue;
		typename std::list<BNode<T>*>::iterator it;
		for (it = p->child.begin(); it != p->child.end(); it++) {
			q.push(*it);
		}
		if (s.find(p) != s.end()) {//s只记录每一层最后一个元素，遍历完一层输出换行
			s.insert((*p->child.rbegin()));
			cout << endl;
		}
	}
	cout << endl << endl;
}

template<class T>
int BTree<T>::getLen(int x)
{
	int cnt = 0;
	while (x) {
		cnt++; x /= 10;
	}
	return cnt;
	return 0;
}

template<class T>
void BTree<T>::GetDFS(BNode<T>* p, int d, int* dep, int* tab, map<T, int>& mp, int& cnt) {
	++cnt;
	int n = cnt;
	typename std::list<T>::iterator it_key = p->keys.begin();
	typename std::list<BNode<T>*>::iterator it_child = p->child.begin();
	mp[*it_key] = cnt;
	int x = 0;
	if (*it_child != NULL) {
		for (; it_child != p->child.end(); it_child++) {
			if (*it_child == NULL)break;
			GetDFS(*it_child, d + 1, dep, tab, mp, cnt);
			BNode<T>* q = *it_child;
			int num = mp[*q->keys.begin()];//孩子层首先输出的空白
			x += tab[num];
		}
		x /= p->child.size();
		tab[n] = x;

	}
	else {//叶子层不用考虑根据子节点输出空白
		tab[n] = dep[d];
	}
	for (; it_key != p->keys.end(); it_key++) {//当前节点输出完对同一层输出的影响
		dep[d] += 1 + getLen(int(*it_key));
	}
	dep[d] += 2;
}

template<class T>
void BTree<T>::printWholeTree()
{
	if (!root) {
		cout << "空树" << endl << endl;
		return;
	}
	int* dep = new int[this->height() + 1]();
	int* tab = new int[this->size() + 1]();//记录每个节点前面要输出的空白
	map<T, int> mp;//map根据每个节点第一个key确定是第几个节点
	int cnt = 0;
	GetDFS(root, 1, dep, tab, mp, cnt);

	queue<BNode<T>*>q;
	set<BNode<T>*>s; s.clear();
	s.insert(root);
	q.push(root);
	int space = 0;//记录一层已经输出的空间
	while (!q.empty()) {
		BNode<T>* p = q.front();
		q.pop();
		int n = mp[*p->keys.begin()];
		for (int i = 1; i <= tab[n] - space; ++i)cout << ' ';
		space = tab[n];
		typename std::list<T>::iterator it_key = p->keys.begin();
		for (; it_key != p->keys.end(); it_key++) {
			if (it_key == p->keys.begin())cout << '|' << *it_key;
			else cout << " " << *it_key;
			space += 1 + getLen(int(*it_key));
		}
		cout << '|';
		space++;
		if (p->isLeaf())continue;
		typename std::list<BNode<T>*>::iterator it;
		for (it = p->child.begin(); it != p->child.end(); it++) {
			q.push(*it);
		}
		if (s.find(p) != s.end()) {
			s.insert((*p->child.rbegin()));
			cout << endl;
			space = 0;
		}
	}
	cout << endl << endl;


	delete[]dep;
	delete[]tab;
}

template<class T>
BTree<T>::~BTree()
{
	deleteNode(root);
}


#endif // BTREE_H
