#pragma once

#include"linkedBinaryTree.h"

template<class T>
class avlNode {
public:
	int m_height;//高度，用来算不平衡度
	T element;
	avlNode<T>* leftChild, * rightChild;

	avlNode() {
		leftChild = rightChild = NULL;
	}
	avlNode(T& theElement) {
		leftChild = rightChild = NULL;
		element = theElement;
	}
	avlNode(T& theElement, avlNode* leftT, avlNode* rightT) {
		leftChild = leftT;
		rightChild = rightT;
		element = theElement;
	}
};

template<class T>
class AVL
{
public:
	~AVL() { destroy(root); }
	avlNode<T>* root;

	void inOrder() { inOrder(root); }
	void preOrder() { preOrder(root); }
	int height() { return height(root); }
	avlNode<T>* search(T key) { return search(root, key); }
	avlNode<T>* iterativeSearch(T key) { return iterativeSearch(root, key); }
	T minimum() {
		//返回最小结点键值
		avlNode<T>* p = minimum(root);
		if (p != nullptr)return p->element;
		return (T)nullptr;
	}
	T maximum() {
		//返回最大结点键值
		avlNode<T>* p = maximum(root);
		if (p != nullptr)return p->element;
		return (T)nullptr;
	}
	void insert(T key) { insert(root, key); }
	void remove(T key) {
		avlNode<T>* z = search(root, key);
		if (z != nullptr)root = remove(root, z);
	}


private:
	void inOrder(avlNode<T>* theRoot);
	void preOrder(avlNode<T>* theRoot);
	int height(avlNode<T>* node);
	void destroy(avlNode<T>* theRoot);
	avlNode<T>* search(avlNode<T>* theRoot, T key);//递归查找
	avlNode<T>* iterativeSearch(avlNode<T>* theRoot, T key);//非递归查找
	avlNode<T>* minimum(avlNode<T>* theRoot);//查找最小结点
	avlNode<T>* maximum(avlNode<T>* theRoot);//查找最大结点
	
	//旋转
	/* LL：左子树的左边失去平衡(左单旋转)
	 *       k2              k1
	 *      /  \            /  \
	 *     k1   z   ===>   x    k2
	 *    /  \                 /  \
	 *   x    y               y    z
	 */
	avlNode<T>* leftLeftRotation(avlNode<T>*& k2);//指针的引用，改变指针的指向

	/* RR：右子树的右边失去平衡(右单旋转)
	 *       k1              k2
	 *      /  \            /  \
	 *     x   k2   ===>   k1   z
	 *        /  \        /  \
	 *       y    z      x    y
	 */
	avlNode<T>* rightRightRotation(avlNode<T>*& k1);

	/* LR：左子树的右边失去平衡(左双旋转)
	 *       k3               k3               k2
	 *      /  \     RR      /  \     LL      /  \
	 *     k1   D   ===>   k2    D   ===>   k1    k3
	 *    /  \            /  \             /  \  /  \
	 *   A    K2         k1   C           A    B C   D
	 *       /  \       /  \
	 *      B    C     A    B
	 */
	avlNode<T>* leftRightRotation(avlNode<T>*& k3);

	/* RL：右子树的左边失去平衡(右双旋转)
	*     k1               k1                K2
	*    /  \      LL     /  \      RR      /  \
	*   A    k3   ===>   A    k2   ===>   k1    K3
	*       /  \             /  \        /  \  /  \
	*      k2   D           B    k3     A    B C   D
	*     /  \                  /   \
	*    B    D                C     D
	*/
	avlNode<T>* rightLeftRotation(avlNode<T>*& k1);

	avlNode<T>* insert(avlNode<T>*& node, T key);
	avlNode<T>* remove(avlNode<T>*& node, avlNode<T>* z);
};

template<class T>
void AVL<T>::inOrder(avlNode<T>* theRoot)
{
	if (theRoot == nullptr)return;
	inOrder(theRoot->leftChild);
	cout << theRoot->element << " ";
	inOrder(theRoot->rightChild);
}

template<class T>
void AVL<T>::preOrder(avlNode<T>* theRoot)
{
	if (theRoot == nullptr)return;
	cout << theRoot->element << " ";
	preOrder(theRoot->leftChild);
	preOrder(theRoot->rightChild);
}

template<class T>
int AVL<T>::height(avlNode<T>* node)
{
	return node != nullptr ? node->m_height : 0;
}

template<class T>
void AVL<T>::destroy(avlNode<T>* theRoot)
{
	if (theRoot == nullptr)return;
	if (theRoot->leftChild != nullptr)destroy(theRoot->leftChild);
	if (theRoot->rightChild != nullptr)destroy(theRoot->rightChild);
	delete theRoot;
}

template<class T>
avlNode<T>* AVL<T>::search(avlNode<T>* theRoot, T key)
{
	if (theRoot == nullptr || theRoot->element == key)
		return theRoot;//查找到了或者访问到叶节点
	if (key < theRoot->element)
		return search(theRoot->leftChild, key);
	else
		return search(theRoot->rightChild, key);
}

template<class T>
avlNode<T>* AVL<T>::iterativeSearch(avlNode<T>* theRoot, T key)
{
	while (theRoot != nullptr && theRoot->element != key) {
		if (key < theRoot->element)
			theRoot = theRoot->leftChild;
		else
			theRoot = theRoot->rightChild;
	}
	return theRoot;
}

template<class T>
avlNode<T>* AVL<T>::minimum(avlNode<T>* theRoot)
{
	if (theRoot == nullptr)return nullptr;
	while (theRoot->leftChild != nullptr)
		theRoot = theRoot->leftChild;//左孩子最小
	
	return theRoot;
}

template<class T>
avlNode<T>* AVL<T>::maximum(avlNode<T>* theRoot)
{
	if (theRoot == nullptr)return nullptr;
	while (theRoot->rightChild != nullptr)
		theRoot = theRoot->rightChild;//右孩子最大

	return theRoot;
}

template<class T>
avlNode<T>* AVL<T>::leftLeftRotation(avlNode<T>*& k2)
{
	avlNode<T>* k1 = k2->leftChild;
	k2->leftChild = k1->rightChild;
	k1->rightChild = k2;
	//调整不平衡因子
	k2->m_height = max(height(k2->leftChild), height(k2->rightChild)) + 1;
	k1->m_height = max(height(k1->leftChild), k2->m_height) + 1;

	return k1;
}

template<class T>
avlNode<T>* AVL<T>::rightRightRotation(avlNode<T>*& k1)
{
	avlNode<T>* k2 = k1->rightChild;
	k1->rightChild = k2->leftChild;
	k2->leftChild = k1;

	k1->m_height = max(height(k1->leftChild), height(k1->rightChild)) + 1;
	k2->m_height = max(height(k2->rightChild), k1->m_height) + 1;
	return k2;
}

template<class T>
avlNode<T>* AVL<T>::leftRightRotation(avlNode<T>*& k3)
{
	k3->leftChild = rightRightRotation(k3->leftChild);//左边转完返回根结点
	return leftLeftRotation(k3);//再在k3做左旋
}

template<class T>
avlNode<T>* AVL<T>::rightLeftRotation(avlNode<T>*& k1)
{
	k1->rightChild = leftLeftRotation(k1->rightChild);
	return rightRightRotation(k1);
}

template<class T>
avlNode<T>* AVL<T>::insert(avlNode<T>*& node, T key)
{
	if (node == nullptr) {//从根节点往下找位置插入
		node = new avlNode<T>(key, nullptr, nullptr);
	}
	else if (key < node->element)//key插入node的左子树
	{
		node->leftChild = insert(node->leftChild, key);
		//插入后，avl树失衡，进行调整
		if (height(node->leftChild) - height(node->rightChild) == 2)
		{
			if (key < node->leftChild->element)//插入到左子树左边
				node = leftLeftRotation(node);
			else//插入到左子树右边
				node = leftRightRotation(node);
		}
	}
	else if (key > node->element)//key插入到node的右子树
	{
		node->rightChild = insert(node->rightChild, key);
		//插入后失衡
		if (height(node->leftChild) - height(node->rightChild) == -2)
		{
			if (key > node->rightChild->element)
				node = rightRightRotation(node);
			else
				node = rightLeftRotation(node);
		}
	}
	else
	{//结点已存在
		cout << "结点已存在" << endl;
	}
	node->m_height = max(height(node->leftChild), height(node->rightChild)) + 1;
	return node;
}

template<class T>
avlNode<T>* AVL<T>::remove(avlNode<T>*& node, avlNode<T>* z)
{
	if (node == nullptr || z == nullptr)return nullptr;

	if (z->element < node->element)//在左子树中做删除
	{
		node->leftChild = remove(node->leftChild, z);
		//删除后avl树失衡
		if (height(node->leftChild) - height(node->rightChild) == -2)
		{
			avlNode<T>* temp = node->rightChild;
			if (height(temp->leftChild) > height(temp->rightChild))
				//右子树的左边高
				node = rightLeftRotation(node);
			else
				//右子树的右边高
				node = rightRightRotation(node);
		}
	}
	else if (z->element > node->element)//在右子树做删除
	{
		node->rightChild = remove(node->rightChild, z);
		if (height(node->leftChild) - height(node->rightChild) == 2)
		{
			avlNode<T>* temp = node->leftChild;
			if (height(temp->rightChild) > height(temp->leftChild))
				//左子树的右子树高
				node = leftRightRotation(node);
			else
				//左左
				node = leftLeftRotation(node);
		}
	}
	else//当前node就是要删除的结点
	{
		if (node->leftChild != nullptr && node->rightChild != nullptr)
		{
			if (height(node->leftChild) > height(node->rightChild)) {
				//左子树比右子树高，找直接前驱，赋给node，再删除直接前驱
				avlNode<T>* max = maximum(node->leftChild);
				node->element = max->element;
				node->leftChild = remove(node->leftChild, max);
			}
			else
			{
				//右子树比左子树高，找直接后继
				avlNode<T>* min = minimum(node->rightChild); 
				node->element = min->element;
				node->rightChild = remove(node->rightChild, min);
			}
		}
		else
		{//没有结点高度变吧
			avlNode<T>* temp = node;
			node = node->leftChild != nullptr ? node->leftChild : node->rightChild;
			delete temp;
		}
	}

	if(node!=nullptr)node->m_height = max(height(node->leftChild), height(node->rightChild)) + 1;
	return node;
}


