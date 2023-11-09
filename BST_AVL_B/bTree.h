#pragma once
#include<iostream>
using namespace std;

class bNode {
	friend class bTree;
public:
	bool isleaf;//判断是否是叶节点
	int keyNum;//key个数
	bNode* parent;
	bNode** child;//指针数组
	int* element;//关键字数组
	bNode(int m) {
		keyNum = 0;
		parent = 0;
		isleaf = true;

		child = new bNode * [2 * m];
		for (int i = 0; i < 2 * m; i++) {//子树指针数组初始化
			child[i] = nullptr;			
		}

		element = new int[2 * m - 1];
		for (int i = 0; i < 2 * m - 1; i++) {//关键字数组初始化
			element[i] = 0;
		}
	}
};

class bTree
{
public:
	bNode* root;
	int m;//最小度,经过计算后传入
	int key_max;
	int key_min;
	int child_max;
	int child_min;
	bTree(int _m) {
		m = _m;
		child_max = 2 * m;
		child_min = m;
		key_max = child_max - 1;
		key_min = child_min - 1;
		root = nullptr;
	}

	bNode* find(bNode* node, int key) {
		//查找是否存在元素
		if (node == nullptr)return nullptr;
		else
		{
			int i;
			for (i = 0; i < node->keyNum; i++)
				if (key <= node->element[i])break;
			
			if (i < node->keyNum && key == node->element[i])
				return node;//找到结点了
			else
			{
				if (node->isleaf)//找到了叶节点都没找到
					return nullptr;
				else//往下搜索
					return find(node->child[i], key);
			}
		}
	}
	bool contain(int key) {
		//判断是否存在key
		if (find(root, key) != nullptr)return true;
		return false;
	}

	void notFull_insert(bNode* node, int key) {
		//往没有满的结点中插入key
		int tempKeyNum = node->keyNum;
		if (node->isleaf)
		{//node是叶节点
			while (tempKeyNum > 0 && key < node->element[tempKeyNum - 1])
			{
				//把元素往后移动腾出位置
				node->element[tempKeyNum] = node->element[tempKeyNum - 1];
				tempKeyNum--;
			}
			node->element[tempKeyNum] = key;
			node->keyNum++;
		}
		else
		{//内部结点
			while (tempKeyNum > 0 && key < node->element[tempKeyNum - 1])
				tempKeyNum--;
			//找到合适的位置夹住
			bNode* tchild = node->child[tempKeyNum];

			if (tchild->keyNum == key_max)
			{//若子节点已满，作提升
				splitBlock(node, tempKeyNum, tchild);
				if (key > node->element[tempKeyNum])//再比较子节点中间值
					tchild = node->child[tempKeyNum + 1];
			}
			notFull_insert(tchild, key);//继续向下寻找合适的位置
		}
	}
	void splitBlock(bNode* theParent, int child_index, bNode* theChild) {
		//把节点分裂开
		bNode* node = new bNode(m);//分裂开的右半部分
		node->isleaf = theChild->isleaf;
		node->keyNum = key_min;
		//拷贝元素值
		for (int i = 0; i < key_min; i++)
			node->element[i] = theChild->element[i+child_min];

		//被分裂的节点如果不是叶节点，还需要把他的孩子转过来
		if (!theChild->isleaf) {
			for (int i = 0; i < child_min; i++) {
				node->child[i] = theChild->child[i + child_min];
				theChild->child[i + child_min]->parent = node->child[i];//？
			}
		}
		theChild->keyNum = key_min;//更新节点个数
		//父节点元素，孩子数组后移
		for (int i = theParent->keyNum; i > child_index; i--)
		{
			theParent->element[i] = theParent->element[i - 1];
			theParent->child[i + 1] = theParent->child[i];
			theChild->child[i]->parent = theParent->child[i+1];//
		}
		theParent->keyNum++;
		theParent->child[child_index + 1] = node;
		node->parent = theParent->child[child_index+1];//?
		//中间值上移
		theParent->element[child_index] = theChild->element[key_min];
	}
	bool Binsert(int key) {
		if (contain(key))return false;//有相同的关键字
		else
		{
			if (root == nullptr)
				root = new bNode(m);
			if (root->keyNum == key_max)//需要分裂，树长高
			{
				bNode* newnode = new bNode(m);
				newnode->child[0] = root;
				newnode->isleaf = false;
				splitBlock(newnode, 0, root);
				root = newnode;
			}
			notFull_insert(root, key);//未满插入
			return true;
		}
	}

	void print(bNode* node, int count) {
		if (node != nullptr) {
			for (int i = 0; i < node->keyNum; i++)
			{
				if (!node->isleaf)//不是叶节点还得往下打印
				{
					print(node->child[i], count - 3);
				}
				for (int j = count; j >= 0; j--)cout << '-';
				cout << "|" << node->element[i] << "|" << endl;
			}
			if (!node->isleaf)print(node->child[node->keyNum], count - 3);//子节点数比关键字数多一个
		}
	}
	void print() {
		print(root, key_max * 5);
	}

	int getpre(bNode* node) {
		//找一个最大值
		while (!node->isleaf)
			node = node->child[node->keyNum];
		//node->keyNum--;//?
		return node->element[node->keyNum - 1];
	}
	int getnext(bNode* node) {
		//用来找后继
		while (!node->isleaf)
			node = node->child[0];
		return node->element[0];
	}
	void mergeBlock(bNode* theParent, int child_index) {
		bNode* child1 = theParent->child[child_index];
		bNode* child2 = theParent->child[child_index + 1];
		//父节点拿下来
		child1->element[key_min] = theParent->element[child_index];
		child1->keyNum = key_max;
		for (int i = 0; i < key_min; i++) {
			child1->element[child_min + i] = child2->element[i];
		}
		//判断child1是否是叶节点，不是还得把child2的子节点转过来
		if (!child1->isleaf)
			for (int i = 0; i < child_min; i++)
				child1->child[i + child_min] = child2->child[i];
		//父节点下移了
		theParent->keyNum--;
		for (int i = child_index; i < theParent->keyNum; i++) {
			theParent->element[i] = theParent->element[i + 1];
			//往前移一位
			theParent->child[i + 1] = theParent->child[i + 2];//超了呢？减了超不了
		}
		delete child2;
		child2 = nullptr;
	}
	//递归删除，做修复
	void deleteBalance(bNode* node, int key) {
		int i = 0;
		//为key寻找一个合适区间
		while (i<node->keyNum && key>node->element[i])i++;

		if (i < node->keyNum && key == node->element[i]) 
		{//当前i的关键字等于key
			if (node->isleaf)
			{//叶节点直接删，在叶节点那么一定是>=childmin
				node->keyNum--;
				while (i < node->keyNum) {
					//往前移动
					node->element[i] = node->element[i + 1];//keynum已经减一了所以不会超界
					i++;
				}
				return;
			}
			else
			{//内节点
				bNode* node_left = node->child[i];//左侧
				bNode* node_right = node->child[i + 1];//右侧
				if (node_left->keyNum >= child_min)
				{
					int pre = getpre(node_left);
					deleteBalance(node_left, pre);
					node->element[i] = pre;//用前驱来替换
					return;
				}
				else if (node_right->keyNum >= child_min)
				{
					int next = getnext(node_right);
					deleteBalance(node_right, next);
					node->element[i] = next;//用后继来替代
					return;
				}
				else
				{//两边都不能减了
					mergeBlock(node, i);
					//合并后再删除
					deleteBalance(node_left, key);
				}
			}
		}
		else
		{//关键字不在当前节点
			bNode* tchild = node->child[i];
			bNode* node_left = nullptr;
			bNode* node_right = nullptr;
			if (tchild->keyNum == key_min)//删了关键字不够
			{
				//判断是否有左右兄弟节点
				if (i > 0)node_left = node->child[i - 1];
				if (i <= node->keyNum - 1)node_right = node->child[i + 1];
				int j;
				if (node_left && node_left->keyNum >= child_min)//左兄弟关键字多于child_min
				{
					//把父节点i-1对应关键字下移
					for (j = tchild->keyNum; j > 0; j--)
						tchild->element[j] = tchild->element[j - 1];
					tchild->element[0] = node->element[i - 1];
					
					//若左兄弟不是叶节点
					if (!node_left->isleaf)
					{//把左兄弟最右边的子节点放过去
						for (j = tchild->keyNum + 1; j > 0; j--)//?更新孩子节点的孩子与父亲
						{
							tchild->child[j - 1]->parent = tchild->child[j]->parent;
							tchild->child[j] = tchild->child[j - 1];
						}
						//左兄弟的最右边孩子移过去
						node_left->child[node_left->keyNum]->parent = tchild->child[0];
						tchild->child[0] = node_left->child[node_left->keyNum];
					}
					tchild->keyNum++;
					//左兄弟移上去
					node->element[i - 1] = node_left->element[node_left->keyNum - 1];
					node_left->keyNum--;
				}
				else if(node_right&&node_right->keyNum>=child_min)//看右兄弟
				{
					//父节点下移
					tchild->element[tchild->keyNum] = node->element[i];
					tchild->keyNum++;
					//右兄弟上移
					node->element[i] = node_right->element[0];
					node_right->keyNum--;
					//右兄弟关键字往前移
					for (j = 0; j < node_right->keyNum; j++)
						node_right->element[j] = node_right->element[j + 1];

					//右兄弟不是叶节点还得把其最左边的子节点赋过去
					if (!node_right->isleaf)
					{
						node_right->child[0]->parent = tchild->child[tchild->keyNum]->parent;
						tchild->child[tchild->keyNum] = node_right->child[0];
						for (j = 0; j < node_right->keyNum + 1; j++)//之前keynum--了
						{
							node_right->child[j + 1]->parent = node_right->child[j]->parent;
							node_right->child[j] = node_right->child[j + 1];
						}
					}
				}
				else if (node_left)//left只有childmin-1个关键字
				{
					//把左兄弟合并
					mergeBlock(node, i - 1);
					tchild = node_left;
				}
				else if (node_right)
				{
					//把右兄弟合并，子节点无需更新
					mergeBlock(node, i);
				}
			}
			deleteBalance(tchild, key);
		}
		
	}
	bool Bdelete(int key) {
		if (!contain(key))return false;
		if (root->keyNum == 1)//根只有一个，可能会影响到树高
		{
			if (root->isleaf)//那就这一个key
			{
				delete root;
				root = nullptr;
				return true;
			}
			else
			{
				bNode* child1 = root->child[0];
				bNode* child2 = root->child[1];
				//树高减少
				if(child1->keyNum==key_min&&child2->keyNum==key_min)
				{
					mergeBlock(root, 0);
					delete root;
					root = child1;
				}
			}
		}
		deleteBalance(root, key);
	}

};

