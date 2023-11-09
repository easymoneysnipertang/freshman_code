#pragma once
#include<iostream>
using namespace std;

class bNode {
	friend class bTree;
public:
	bool isleaf;//�ж��Ƿ���Ҷ�ڵ�
	int keyNum;//key����
	bNode* parent;
	bNode** child;//ָ������
	int* element;//�ؼ�������
	bNode(int m) {
		keyNum = 0;
		parent = 0;
		isleaf = true;

		child = new bNode * [2 * m];
		for (int i = 0; i < 2 * m; i++) {//����ָ�������ʼ��
			child[i] = nullptr;			
		}

		element = new int[2 * m - 1];
		for (int i = 0; i < 2 * m - 1; i++) {//�ؼ��������ʼ��
			element[i] = 0;
		}
	}
};

class bTree
{
public:
	bNode* root;
	int m;//��С��,�����������
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
		//�����Ƿ����Ԫ��
		if (node == nullptr)return nullptr;
		else
		{
			int i;
			for (i = 0; i < node->keyNum; i++)
				if (key <= node->element[i])break;
			
			if (i < node->keyNum && key == node->element[i])
				return node;//�ҵ������
			else
			{
				if (node->isleaf)//�ҵ���Ҷ�ڵ㶼û�ҵ�
					return nullptr;
				else//��������
					return find(node->child[i], key);
			}
		}
	}
	bool contain(int key) {
		//�ж��Ƿ����key
		if (find(root, key) != nullptr)return true;
		return false;
	}

	void notFull_insert(bNode* node, int key) {
		//��û�����Ľ���в���key
		int tempKeyNum = node->keyNum;
		if (node->isleaf)
		{//node��Ҷ�ڵ�
			while (tempKeyNum > 0 && key < node->element[tempKeyNum - 1])
			{
				//��Ԫ�������ƶ��ڳ�λ��
				node->element[tempKeyNum] = node->element[tempKeyNum - 1];
				tempKeyNum--;
			}
			node->element[tempKeyNum] = key;
			node->keyNum++;
		}
		else
		{//�ڲ����
			while (tempKeyNum > 0 && key < node->element[tempKeyNum - 1])
				tempKeyNum--;
			//�ҵ����ʵ�λ�ü�ס
			bNode* tchild = node->child[tempKeyNum];

			if (tchild->keyNum == key_max)
			{//���ӽڵ�������������
				splitBlock(node, tempKeyNum, tchild);
				if (key > node->element[tempKeyNum])//�ٱȽ��ӽڵ��м�ֵ
					tchild = node->child[tempKeyNum + 1];
			}
			notFull_insert(tchild, key);//��������Ѱ�Һ��ʵ�λ��
		}
	}
	void splitBlock(bNode* theParent, int child_index, bNode* theChild) {
		//�ѽڵ���ѿ�
		bNode* node = new bNode(m);//���ѿ����Ұ벿��
		node->isleaf = theChild->isleaf;
		node->keyNum = key_min;
		//����Ԫ��ֵ
		for (int i = 0; i < key_min; i++)
			node->element[i] = theChild->element[i+child_min];

		//�����ѵĽڵ��������Ҷ�ڵ㣬����Ҫ�����ĺ���ת����
		if (!theChild->isleaf) {
			for (int i = 0; i < child_min; i++) {
				node->child[i] = theChild->child[i + child_min];
				theChild->child[i + child_min]->parent = node->child[i];//��
			}
		}
		theChild->keyNum = key_min;//���½ڵ����
		//���ڵ�Ԫ�أ������������
		for (int i = theParent->keyNum; i > child_index; i--)
		{
			theParent->element[i] = theParent->element[i - 1];
			theParent->child[i + 1] = theParent->child[i];
			theChild->child[i]->parent = theParent->child[i+1];//
		}
		theParent->keyNum++;
		theParent->child[child_index + 1] = node;
		node->parent = theParent->child[child_index+1];//?
		//�м�ֵ����
		theParent->element[child_index] = theChild->element[key_min];
	}
	bool Binsert(int key) {
		if (contain(key))return false;//����ͬ�Ĺؼ���
		else
		{
			if (root == nullptr)
				root = new bNode(m);
			if (root->keyNum == key_max)//��Ҫ���ѣ�������
			{
				bNode* newnode = new bNode(m);
				newnode->child[0] = root;
				newnode->isleaf = false;
				splitBlock(newnode, 0, root);
				root = newnode;
			}
			notFull_insert(root, key);//δ������
			return true;
		}
	}

	void print(bNode* node, int count) {
		if (node != nullptr) {
			for (int i = 0; i < node->keyNum; i++)
			{
				if (!node->isleaf)//����Ҷ�ڵ㻹�����´�ӡ
				{
					print(node->child[i], count - 3);
				}
				for (int j = count; j >= 0; j--)cout << '-';
				cout << "|" << node->element[i] << "|" << endl;
			}
			if (!node->isleaf)print(node->child[node->keyNum], count - 3);//�ӽڵ����ȹؼ�������һ��
		}
	}
	void print() {
		print(root, key_max * 5);
	}

	int getpre(bNode* node) {
		//��һ�����ֵ
		while (!node->isleaf)
			node = node->child[node->keyNum];
		//node->keyNum--;//?
		return node->element[node->keyNum - 1];
	}
	int getnext(bNode* node) {
		//�����Һ��
		while (!node->isleaf)
			node = node->child[0];
		return node->element[0];
	}
	void mergeBlock(bNode* theParent, int child_index) {
		bNode* child1 = theParent->child[child_index];
		bNode* child2 = theParent->child[child_index + 1];
		//���ڵ�������
		child1->element[key_min] = theParent->element[child_index];
		child1->keyNum = key_max;
		for (int i = 0; i < key_min; i++) {
			child1->element[child_min + i] = child2->element[i];
		}
		//�ж�child1�Ƿ���Ҷ�ڵ㣬���ǻ��ð�child2���ӽڵ�ת����
		if (!child1->isleaf)
			for (int i = 0; i < child_min; i++)
				child1->child[i + child_min] = child2->child[i];
		//���ڵ�������
		theParent->keyNum--;
		for (int i = child_index; i < theParent->keyNum; i++) {
			theParent->element[i] = theParent->element[i + 1];
			//��ǰ��һλ
			theParent->child[i + 1] = theParent->child[i + 2];//�����أ����˳�����
		}
		delete child2;
		child2 = nullptr;
	}
	//�ݹ�ɾ�������޸�
	void deleteBalance(bNode* node, int key) {
		int i = 0;
		//ΪkeyѰ��һ����������
		while (i<node->keyNum && key>node->element[i])i++;

		if (i < node->keyNum && key == node->element[i]) 
		{//��ǰi�Ĺؼ��ֵ���key
			if (node->isleaf)
			{//Ҷ�ڵ�ֱ��ɾ����Ҷ�ڵ���ôһ����>=childmin
				node->keyNum--;
				while (i < node->keyNum) {
					//��ǰ�ƶ�
					node->element[i] = node->element[i + 1];//keynum�Ѿ���һ�����Բ��ᳬ��
					i++;
				}
				return;
			}
			else
			{//�ڽڵ�
				bNode* node_left = node->child[i];//���
				bNode* node_right = node->child[i + 1];//�Ҳ�
				if (node_left->keyNum >= child_min)
				{
					int pre = getpre(node_left);
					deleteBalance(node_left, pre);
					node->element[i] = pre;//��ǰ�����滻
					return;
				}
				else if (node_right->keyNum >= child_min)
				{
					int next = getnext(node_right);
					deleteBalance(node_right, next);
					node->element[i] = next;//�ú�������
					return;
				}
				else
				{//���߶����ܼ���
					mergeBlock(node, i);
					//�ϲ�����ɾ��
					deleteBalance(node_left, key);
				}
			}
		}
		else
		{//�ؼ��ֲ��ڵ�ǰ�ڵ�
			bNode* tchild = node->child[i];
			bNode* node_left = nullptr;
			bNode* node_right = nullptr;
			if (tchild->keyNum == key_min)//ɾ�˹ؼ��ֲ���
			{
				//�ж��Ƿ��������ֵܽڵ�
				if (i > 0)node_left = node->child[i - 1];
				if (i <= node->keyNum - 1)node_right = node->child[i + 1];
				int j;
				if (node_left && node_left->keyNum >= child_min)//���ֵܹؼ��ֶ���child_min
				{
					//�Ѹ��ڵ�i-1��Ӧ�ؼ�������
					for (j = tchild->keyNum; j > 0; j--)
						tchild->element[j] = tchild->element[j - 1];
					tchild->element[0] = node->element[i - 1];
					
					//�����ֵܲ���Ҷ�ڵ�
					if (!node_left->isleaf)
					{//�����ֵ����ұߵ��ӽڵ�Ź�ȥ
						for (j = tchild->keyNum + 1; j > 0; j--)//?���º��ӽڵ�ĺ����븸��
						{
							tchild->child[j - 1]->parent = tchild->child[j]->parent;
							tchild->child[j] = tchild->child[j - 1];
						}
						//���ֵܵ����ұߺ����ƹ�ȥ
						node_left->child[node_left->keyNum]->parent = tchild->child[0];
						tchild->child[0] = node_left->child[node_left->keyNum];
					}
					tchild->keyNum++;
					//���ֵ�����ȥ
					node->element[i - 1] = node_left->element[node_left->keyNum - 1];
					node_left->keyNum--;
				}
				else if(node_right&&node_right->keyNum>=child_min)//�����ֵ�
				{
					//���ڵ�����
					tchild->element[tchild->keyNum] = node->element[i];
					tchild->keyNum++;
					//���ֵ�����
					node->element[i] = node_right->element[0];
					node_right->keyNum--;
					//���ֵܹؼ�����ǰ��
					for (j = 0; j < node_right->keyNum; j++)
						node_right->element[j] = node_right->element[j + 1];

					//���ֵܲ���Ҷ�ڵ㻹�ð�������ߵ��ӽڵ㸳��ȥ
					if (!node_right->isleaf)
					{
						node_right->child[0]->parent = tchild->child[tchild->keyNum]->parent;
						tchild->child[tchild->keyNum] = node_right->child[0];
						for (j = 0; j < node_right->keyNum + 1; j++)//֮ǰkeynum--��
						{
							node_right->child[j + 1]->parent = node_right->child[j]->parent;
							node_right->child[j] = node_right->child[j + 1];
						}
					}
				}
				else if (node_left)//leftֻ��childmin-1���ؼ���
				{
					//�����ֵܺϲ�
					mergeBlock(node, i - 1);
					tchild = node_left;
				}
				else if (node_right)
				{
					//�����ֵܺϲ����ӽڵ��������
					mergeBlock(node, i);
				}
			}
			deleteBalance(tchild, key);
		}
		
	}
	bool Bdelete(int key) {
		if (!contain(key))return false;
		if (root->keyNum == 1)//��ֻ��һ�������ܻ�Ӱ�쵽����
		{
			if (root->isleaf)//�Ǿ���һ��key
			{
				delete root;
				root = nullptr;
				return true;
			}
			else
			{
				bNode* child1 = root->child[0];
				bNode* child2 = root->child[1];
				//���߼���
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

