#ifndef BNODE_H
#define BNODE_H

#include <list>
#include <iostream>
using namespace std;

template <class T>
class BNode
{
public:
	list <T> keys;	//键值表
	list <BNode<T>*> child;	//子节点指针表
	BNode<T>* fa;	//父节点指针

	BNode();
	BNode(T);
	~BNode();
	//针对结点的操作，还未涉及到树
	bool isLeaf();	//判断是否是叶节点
	bool hasKey(T key);  //判断是否存在某一键值
	bool replace(T old_key, T new_key);  //替换键值
	bool eraseKey(T key);  //删除键值
	bool eraseChild(BNode<T>*); //删除子节点指针
	void updateChild(); //子节点重定向
	void insertKey(T data, BNode<T>* child1, BNode<T>* child2);	//向节点插入某键值，并更新子节点指针
	void print();	//输出节点键值
	BNode<T>* next(T key);	//获取key应该指向子节点的方向，返回具体子节点
	T getKey(int pos);	//获取节点第pos个键值
	int size();	//获取节点所存储键值数量
};

template<class T>
bool BNode<T>::isLeaf()
{
	return child.empty() || (*child.begin()) == NULL;
}

template<class T>
bool BNode<T>::hasKey(T key) {
	typename std::list<T>::iterator it_key;
	for (it_key = keys.begin(); it_key != keys.end(); it_key++)
		if (*it_key == key)return true;
	return false;
}

template<class T>
bool BNode<T>::replace(T old_key, T new_key) {
	typename std::list<T>::iterator it_key;
	for (it_key = keys.begin(); it_key != keys.end(); it_key++)
		if (*it_key == old_key) {
			*it_key = new_key;
			return true;
		}
	return false;
}

template<class T>
bool BNode<T>::eraseKey(T key) {
	if (!hasKey(key))return false;
	typename std::list<T>::iterator it_key;
	for (it_key = keys.begin(); it_key != keys.end(); it_key++)
		if (*it_key == key) {
			keys.erase(it_key);
			break;
		}
	return true;
}

template<class T>
bool BNode<T>::eraseChild(BNode<T>* p) {
	typename std::list<BNode<T>*>::iterator it_child;
	for (it_child = child.begin(); it_child != child.end(); it_child++)
		if (*it_child == p) {
			child.erase(it_child);
			return true;
		}
	return false;
}

template<class T>
BNode<T>* BNode<T>::next(T data)
{
	typename std::list<T>::iterator it_key = keys.begin();
	typename std::list<BNode<T>*>::iterator it_child = child.begin();
	while (it_key != keys.end() && data >= *it_key) {//孩子指针随关键字指针一起动
		it_child++;
		it_key++;
	}
	return *it_child;
}

template<class T>
void BNode<T>::insertKey(T key, BNode<T>* child1, BNode<T>* child2)
{
	typename std::list<T>::iterator it_key = keys.begin();
	typename std::list<BNode*>::iterator it_child = child.begin();
	while (it_key != keys.end() && key > *it_key) {
		it_child++;
		it_key++;
	}//找到合适的位置
	keys.insert(it_key, key);
	//两种场景：往叶节点中插入，删除补充的都是null；插入后的maintain，原子节点已经分裂，子节点中间值拎包入住
	if (!child.empty())it_child = child.erase(it_child);
	it_child = child.insert(it_child, child2);
	it_child = child.insert(it_child, child1);
}

template<class T>
void BNode<T>::updateChild() {
	typename std::list<BNode*>::iterator it_child;
	for (it_child = child.begin(); it_child != child.end(); it_child++)
		if (*it_child)//把孩子节点归过来
			(*it_child)->fa = this;
}

template<class T>
void BNode<T>::print()
{
	typename std::list<T>::iterator it;
	for (it = keys.begin(); it != keys.end(); it++)cout << *it << ' ';
}

template<class T>
T BNode<T>::getKey(int pos)
{
	typename std::list<T>::iterator it = keys.begin();
	for (int i = 0; i < pos; ++i) {	//异常处理
		++it;
	}
	return *it;
}

template<class T>
int BNode<T>::size()
{
	return keys.size();
	return 0;
}

template<class T>
BNode<T>::BNode()
{
	keys.clear();
	child.clear();
	fa = NULL;
}

template<class T>
BNode<T>::BNode(T data)
{
	keys.push_back(data);
	child.push_back(NULL);
	child.push_back(NULL);
	fa = NULL;
}

template<class T>
BNode<T>::~BNode()
{
	child.clear();
	keys.clear();
}

#endif // BNODE_H
