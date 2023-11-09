#pragma once

#include"linkedBinaryTree.h"
using namespace std;

template<class K,class E>
class BST: public linkedBinaryTree<pair<const K,E>>
{
public:
	bool empty()const { return this->treeSize == 0; }
	int size() const { return this->treeSize; }
	void ascend() { this->inOrder(linkedBinaryTree<pair<const K, E>>::output); }//�������
	pair<const K, E>* find(const K& theKey)const;
	void insert(const pair<const K, E>& thePair);
	void erase(const K& theKey);
	
};
template<class K, class E>
pair<const K, E>* BST<K, E>::find(const K& theKey) const
{
	//����һ��ƥ��pair��ָ�룬û�оͷ���null
	//p�Ӹ�����ʼ�ƶ���Ѱ��keyƥ���Ԫ��
	binaryTreeNode<pair<const K, E>>* p = this->root;
	while (p != nullptr) {
		if (theKey < p->element.first)
			p = p->leftChild;
		else
			if (theKey > p->element.first)
				p = p->rightChild;
			else
				return &p->element;//�ҵ�ƥ����ˣ�����ָ��
	}
	return nullptr;
}
template<class K, class E>
void BST<K, E>::insert(const pair<const K, E>& thePair) 
{
	//����thePair,������ڹؼ�����ͬ�������򸲸�
	binaryTreeNode<pair<const K, E>>* p = this->root, * pp = nullptr;
	while (p != nullptr) {
		pp = p;
		//p�����ƶ�
		if (thePair.first < p->element.first)
			p = p->leftChild;
		else
			if (thePair.first > p->element.first)
				p = p->rightChild;
			else {
				p->element.second = thePair.second;//�ظ�keyֵ
				return;
			}
	}
	//�ҵ��˺��ʵ�λ�ã�p����null����ppδ��
	//ΪthePair����һ���ڵ㣬Ȼ����Ϊpp���ӽڵ㣬��pp����
	binaryTreeNode<pair<const K, E>>* newNode = new binaryTreeNode<pair<const K, E>>(thePair);
	if (this->root != nullptr) {
		if (thePair.first < pp->element.first)//ȷ�����Һ���
			pp->leftChild = newNode;
		else
			pp->rightChild = newNode;
	}
	else this->root = newNode;//���ǿյ�
	this->treeSize++;

}

template<class K, class E>
void BST<K, E>::erase(const K& theKey) {
	//ɾ���ؼ���ΪtheKey������
	//����
	binaryTreeNode<pair<const K, E>>* p = this->root, * pp = nullptr;
	while (p != nullptr && p->element.first != theKey)
	{
		pp = p;
		//p�����ӽڵ�
		if (theKey < p->element.first)
			p = p->leftChild;
		else
			p = p->rightChild;
	}
	if (p == nullptr)return;//û��ƥ���

	//������֯���Ľṹ
	//��p����������ʱ
	if (p->leftChild != nullptr && p->rightChild != nullptr) {
		//ת���ɿջ���ֻ��һ������
		//��p��ֱ��ǰ�������
		binaryTreeNode<pair<const K, E>>* s = p->leftChild, * ps = p;
		while (s->rightChild != nullptr)
		{//������������Ԫ��
			ps = p;
			s = s->rightChild;
		}
		//��s�ƶ���pȥ������ֱ��p->element=s->element����const
		//��p������ȫ��������
		binaryTreeNode<pair<const K, E>>* q = new binaryTreeNode<pair<const K, E>>(s->element, p->leftChild, p->rightChild);
		if (pp == nullptr)this->root = q;//���Ǹ��ڵ�
		else
		{//��p�ĸ��ڵ�����
			if (p == pp->leftChild)
				pp->leftChild = q;
			else
				pp->rightChild = q;
		}
		
		//s�ƹ�ȥ��s��ɾ��
		if (ps == p)pp = q;//����p�����һ��Ҷ��
		else pp = ps;//ת��s�ĸ��ڵ�
		delete p;
		p = s;
	}

	//ͳһ������Ϊ1��0�Ľڵ�
	//Ҫɾ����p�ĺ��ӽڵ����c��
	binaryTreeNode<pair<const K, E>>* c;
	if (p->leftChild != nullptr)
		c = p->leftChild;
	else
		c = p->rightChild;
	
	//ɾ��p
	if (p == this->root)this->root = c;
	else {
		//ȷ��p��pp�����ӻ����Һ���
		if (p == pp->leftChild)
			pp->leftChild = c;
		else pp->rightChild = c;
	}
	this->treeSize--;
	delete p;
}

////��pair����<<
//template<class K,class E>
//ostream& operator<<(ostream& cout, const pair<K, E>& x) {
//	cout << x.first << ' ' << x.second;
//	return cout;
//}