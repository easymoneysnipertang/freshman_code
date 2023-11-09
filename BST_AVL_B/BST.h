#pragma once

#include"linkedBinaryTree.h"
using namespace std;

template<class K,class E>
class BST: public linkedBinaryTree<pair<const K,E>>
{
public:
	bool empty()const { return this->treeSize == 0; }
	int size() const { return this->treeSize; }
	void ascend() { this->inOrder(linkedBinaryTree<pair<const K, E>>::output); }//升序输出
	pair<const K, E>* find(const K& theKey)const;
	void insert(const pair<const K, E>& thePair);
	void erase(const K& theKey);
	
};
template<class K, class E>
pair<const K, E>* BST<K, E>::find(const K& theKey) const
{
	//返回一个匹配pair的指针，没有就返回null
	//p从根部开始移动，寻找key匹配的元素
	binaryTreeNode<pair<const K, E>>* p = this->root;
	while (p != nullptr) {
		if (theKey < p->element.first)
			p = p->leftChild;
		else
			if (theKey > p->element.first)
				p = p->rightChild;
			else
				return &p->element;//找到匹配的了，返回指针
	}
	return nullptr;
}
template<class K, class E>
void BST<K, E>::insert(const pair<const K, E>& thePair) 
{
	//插入thePair,如果存在关键字相同的数对则覆盖
	binaryTreeNode<pair<const K, E>>* p = this->root, * pp = nullptr;
	while (p != nullptr) {
		pp = p;
		//p向下移动
		if (thePair.first < p->element.first)
			p = p->leftChild;
		else
			if (thePair.first > p->element.first)
				p = p->rightChild;
			else {
				p->element.second = thePair.second;//重复key值
				return;
			}
	}
	//找到了合适的位置，p到了null，但pp未到
	//为thePair建立一个节点，然后作为pp的子节点，与pp连接
	binaryTreeNode<pair<const K, E>>* newNode = new binaryTreeNode<pair<const K, E>>(thePair);
	if (this->root != nullptr) {
		if (thePair.first < pp->element.first)//确定左右孩子
			pp->leftChild = newNode;
		else
			pp->rightChild = newNode;
	}
	else this->root = newNode;//树是空的
	this->treeSize++;

}

template<class K, class E>
void BST<K, E>::erase(const K& theKey) {
	//删除关键字为theKey的数对
	//查找
	binaryTreeNode<pair<const K, E>>* p = this->root, * pp = nullptr;
	while (p != nullptr && p->element.first != theKey)
	{
		pp = p;
		//p移向子节点
		if (theKey < p->element.first)
			p = p->leftChild;
		else
			p = p->rightChild;
	}
	if (p == nullptr)return;//没有匹配的

	//重新组织树的结构
	//当p有两个孩子时
	if (p->leftChild != nullptr && p->rightChild != nullptr) {
		//转换成空或者只有一个孩子
		//用p的直接前驱来替代
		binaryTreeNode<pair<const K, E>>* s = p->leftChild, * ps = p;
		while (s->rightChild != nullptr)
		{//左子树中最大的元素
			ps = p;
			s = s->rightChild;
		}
		//把s移动到p去，不能直接p->element=s->element，是const
		//把p的子树全部做出来
		binaryTreeNode<pair<const K, E>>* q = new binaryTreeNode<pair<const K, E>>(s->element, p->leftChild, p->rightChild);
		if (pp == nullptr)this->root = q;//就是根节点
		else
		{//把p的父节点连上
			if (p == pp->leftChild)
				pp->leftChild = q;
			else
				pp->rightChild = q;
		}
		
		//s移过去了s得删除
		if (ps == p)pp = q;//就是p下面的一个叶子
		else pp = ps;//转到s的父节点
		delete p;
		p = s;
	}

	//统一地做度为1和0的节点
	//要删除的p的孩子节点存在c中
	binaryTreeNode<pair<const K, E>>* c;
	if (p->leftChild != nullptr)
		c = p->leftChild;
	else
		c = p->rightChild;
	
	//删除p
	if (p == this->root)this->root = c;
	else {
		//确定p是pp的左孩子还是右孩子
		if (p == pp->leftChild)
			pp->leftChild = c;
		else pp->rightChild = c;
	}
	this->treeSize--;
	delete p;
}

////给pair重载<<
//template<class K,class E>
//ostream& operator<<(ostream& cout, const pair<K, E>& x) {
//	cout << x.first << ' ' << x.second;
//	return cout;
//}