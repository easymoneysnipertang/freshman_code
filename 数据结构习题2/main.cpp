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
vector<element>v;//储存所有编码
list<int>code;//获取编码时当栈用，读取编码时当队列用


void getCoding(binaryTreeNode<char>* root) {
	//通过Huffman顶点向下遍历，获得编码

	if (root->leftChild != NULL) {//不会有度为1的点
		code.push_back(0);
		getCoding(root->leftChild);
		code.pop_back();
		code.push_back(1);
		getCoding(root->rightChild);
		code.pop_back();
	}
	else {//返回到叶节点了
		element e;
		e.code = code;
		e.ch = root->element;
		v.push_back(e);
	}
}
void decoding(linkedBinaryTree<char>* hftree) {
	//遍历Huffman译码
	ifstream in("压缩结果.dat");//读入编码串
	ofstream out("解码结果.txt");
	binaryTreeNode<char>* root = hftree->root;
	char t;
	while (in >> t && !in.eof()) {
		//总不能就一棵树一个根结点吧
		if (t == '0')root = root->leftChild;
		else if (t == '1')root = root->rightChild;
		if (root->element != '$') {
			
			//访问到非叶节点
			out << root->element;
			root = hftree->root;
		}
	}
	in.close();
	out.close();
}

int frequency[128] = {};//记录128个字符频率
int weight[129] = {};//将频率转换成权
char letter[129];//记录出现的字母
int num = 0;
int getIndex(char ch) {//查找字符下标
	//letter里面存的顺序不是v里面的顺序！
	for (int i = 0; i < v.size(); i++) {
		if (v[i].ch == ch)return i;
	}
}
void inOutFile() {	
	char ch;
	//获取字符出现频率
	ifstream infile1("原始字符集.txt");
	infile1.get(ch);
	while (!infile1.eof()) {	
		int t = ch;
		frequency[t]++;
		infile1.get(ch);
	}
	infile1.close();

	//将字符和weight存在数组里
	for (int i = 0; i <= 127; i++) {
		//frequency从0开始计，一共128个字符
		//weight从1开始数
		if (frequency[i] != 0) {
			weight[num+1] = frequency[i];
			
			letter[num+1] = i + '\0';
			num++;
		}
	}
	//将字母按照weight排序
	for (int i = 1; i <= num; i++) {//TODO:还有必要排序吗，排了也没用啊√排呗，后面按权输出嘛
		for (int j =num; j >i; j--) {
			if (weight[j] < weight[i]) {
				swap(weight[j], weight[i]);
				swap(letter[j], letter[i]);
			}
		}
	}

	//调用Huffman函数进行编码
	linkedBinaryTree<char>* hftree = huffmanTree().func(weight,num,letter);
	getCoding(hftree->root);
	//hftree->postOrder(linkedBinaryTree<char>::output);

	//编码完毕
	//输出编码结果
	ofstream outfile1("编码结果.txt");
	for (int i = 1; i <= num; i++) {
		outfile1 << letter[i] << " " << weight[i] << " ";
		int index = getIndex(letter[i]);
		int size = v[index].code.size();
		int count = 0;
		while (count++<size) {//todo:链表整没了后面咋办√
			int t = v[index].code.front();
			outfile1 << t;
			v[index].code.pop_front();

			v[index].code.push_back(t);//放回去，编码始终保存着
		}
		outfile1 << endl;
	}
	outfile1.close();

	//对实验文本进行编码输出，并计算压缩比
	ifstream infile2("待压缩文本.txt");
	ofstream outfile("压缩结果.dat");
	//为了计算压缩比，统计该文件字符总个数
	int numOfchar = 0, bitsOfFile = 0;

	infile2.get(ch);
	while (!infile2.eof()) {
		numOfchar++;//字符总数+1
		int index = getIndex(ch);
		int size = v[index].code.size();
		bitsOfFile += size;//压缩的size

		int count = 0;
		while (count++ < size) {
			int t = v[index].code.front();
			outfile << t;
			v[index].code.pop_front();
			v[index].code.push_back(t);//放回去，编码始终保存着
		}
		infile2.get(ch);
	}
	//得出压缩比
	//cout << numOfchar << " " << bitsOfFile << endl;//test
	cout << "文件压缩比：" << bitsOfFile / ((float)8 * numOfchar) << endl;
	infile2.close();
	outfile.close();


	//译码
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