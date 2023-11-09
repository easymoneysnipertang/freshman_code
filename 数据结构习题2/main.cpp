#include<iostream>
#include"linkedBinaryTree.h"
#include"huffmanTree.h"
#include<list>
#include<vector>
#include<fstream>

using namespace std;


struct element {
	list<int>code;
	char ch;
};
vector<element>v;//�������б���
list<int>code;//��ȡ����ʱ��ջ�ã���ȡ����ʱ��������


void getCoding(binaryTreeNode<char>* root) {
	//ͨ��Huffman�������±�������ñ���

	if (root->leftChild != NULL) {//�����ж�Ϊ1�ĵ�
		code.push_back(0);
		getCoding(root->leftChild);
		code.pop_back();
		code.push_back(1);
		getCoding(root->rightChild);
		code.pop_back();
	}
	else {//���ص�Ҷ�ڵ���
		element e;
		e.code = code;
		e.ch = root->element;
		v.push_back(e);
	}
}
void decoding(linkedBinaryTree<char>* hftree) {
	//����Huffman����
	ifstream in("ѹ�����.dat");//������봮
	ofstream out("������.txt");
	binaryTreeNode<char>* root = hftree->root;
	char t;
	while (in >> t && !in.eof()) {
		//�ܲ��ܾ�һ����һ��������
		if (t == '0')root = root->leftChild;
		else if (t == '1')root = root->rightChild;
		if (root->element != '$') {
			
			//���ʵ���Ҷ�ڵ�
			out << root->element;
			root = hftree->root;
		}
	}
	in.close();
	out.close();
}

int frequency[128] = {};//��¼128���ַ�Ƶ��
int weight[129] = {};//��Ƶ��ת����Ȩ
char letter[129];//��¼���ֵ���ĸ
int num = 0;
int getIndex(char ch) {//�����ַ��±�
	//letter������˳����v�����˳��
	for (int i = 0; i < v.size(); i++) {
		if (v[i].ch == ch)return i;
	}
}
void inOutFile() {	
	char ch;
	//��ȡ�ַ�����Ƶ��
	ifstream infile1("ԭʼ�ַ���.txt");
	infile1.get(ch);
	while (!infile1.eof()) {	
		int t = ch;
		frequency[t]++;
		infile1.get(ch);
	}
	infile1.close();

	//���ַ���weight����������
	for (int i = 0; i <= 127; i++) {
		//frequency��0��ʼ�ƣ�һ��128���ַ�
		//weight��1��ʼ��
		if (frequency[i] != 0) {
			weight[num+1] = frequency[i];
			
			letter[num+1] = i + '\0';
			num++;
		}
	}
	//����ĸ����weight����
	for (int i = 1; i <= num; i++) {//TODO:���б�Ҫ����������Ҳû�ð������£����水Ȩ�����
		for (int j =num; j >i; j--) {
			if (weight[j] < weight[i]) {
				swap(weight[j], weight[i]);
				swap(letter[j], letter[i]);
			}
		}
	}

	//����Huffman�������б���
	linkedBinaryTree<char>* hftree = huffmanTree().func(weight,num,letter);
	getCoding(hftree->root);
	//hftree->postOrder(linkedBinaryTree<char>::output);

	//�������
	//���������
	ofstream outfile1("������.txt");
	for (int i = 1; i <= num; i++) {
		outfile1 << letter[i] << " " << weight[i] << " ";
		int index = getIndex(letter[i]);
		int size = v[index].code.size();
		int count = 0;
		while (count++<size) {//todo:������û�˺���զ���
			int t = v[index].code.front();
			outfile1 << t;
			v[index].code.pop_front();

			v[index].code.push_back(t);//�Ż�ȥ������ʼ�ձ�����
		}
		outfile1 << endl;
	}
	outfile1.close();

	//��ʵ���ı����б��������������ѹ����
	ifstream infile2("��ѹ���ı�.txt");
	ofstream outfile("ѹ�����.dat");
	//Ϊ�˼���ѹ���ȣ�ͳ�Ƹ��ļ��ַ��ܸ���
	int numOfchar = 0, bitsOfFile = 0;

	infile2.get(ch);
	while (!infile2.eof()) {
		numOfchar++;//�ַ�����+1
		int index = getIndex(ch);
		int size = v[index].code.size();
		bitsOfFile += size;//ѹ����size

		int count = 0;
		while (count++ < size) {
			int t = v[index].code.front();
			outfile << t;
			v[index].code.pop_front();
			v[index].code.push_back(t);//�Ż�ȥ������ʼ�ձ�����
		}
		infile2.get(ch);
	}
	//�ó�ѹ����
	//cout << numOfchar << " " << bitsOfFile << endl;//test
	cout << "�ļ�ѹ���ȣ�" << bitsOfFile / ((float)8 * numOfchar) << endl;
	infile2.close();
	outfile.close();


	//����
	decoding(hftree);
	
}


//test
void test01() {
	linkedBinaryTree<char> a, x, y, z;
	y.makeTree('a', a, a);
	z.makeTree('b', a, a);
	x.makeTree('c', y, z);
	y.makeTree('d', x, a);
	cout << y.size() << endl;
	cout << y.height() << endl;
	y.postOrder(linkedBinaryTree<char>::output);
}
void test02() {
	int a[6];
	int n = 5;
	char c[6];
	for (int i = 1; i <= n; i++) {
		a[i] = i;
		c[i] = 'a' + i;
	}
	linkedBinaryTree<char>* x = huffmanTree().func(a, n, c);
	/*x->postOrder(linkedBinaryTree<char>::output);*/
	getCoding(x->root);
	//test
	for (int i = 0; i < v.size(); i++) {
		int t;
		
		while (!v[i].code.empty()) {
			t = v[i].code.front();
			cout << t;
			v[i].code.pop_front();
		}
		cout << endl;
	}
}
void test03() {
	char ch;
	ifstream in("in.txt");
	in.get(ch);
	while (!in.eof()) {
		cout << ch << endl;
		int t = ch;
		cout << t;
		char te = '\0' + t;
		cout << te << endl;
		in.get(ch);
	}
	in.close();

	in.open("outfile.dat");
	int t;
	while (in >> ch && !in.eof()) {
		cout <<ch;
	}

}

int main() {
	//test03();
	inOutFile();
	return 0;
}