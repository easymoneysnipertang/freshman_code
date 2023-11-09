#include<iostream>
#include"BTree.h"
#include<string>
#include<sstream>
using namespace std;

bool cinFail() {
	if (!cin.fail())return 0;
	cout << "格式有误！" << endl;
	cin.clear();
	cin.ignore(1024, '\n');
	return 0;
}
bool wantContinue() {
	cout << "是否继续？Y or N" << endl;
	string t;
	cin >> t;
	if (t.compare("Y") == 0) {
		system("cls");
		return 1;
	}
	else {
		return 0;
	}
}
void test() {
	int keynum, m;
	while(true){
		cout << "输入关键字个数：";
		cin >> keynum;
		if (cin.fail()) {
			cinFail();
			cout << "请重新输入" << endl;
			continue;
		}
		cout << "输入B树阶数：";
		cin >> m;
		if (cin.fail()) {
			cinFail();
			cout << "请重新输入" << endl;
		}
		else break;
	}
	
	BTree<int>* tree = new BTree<int>(m);
	cout << endl << "输入初始化关键字：" << endl;

	string input;
	cin >> input;
	int num = 0, count = 0;
	for (int i = 0; i <= input.length(); i++) {
		if (input[i] == ',' || input[i] == '\0')//读到逗号
		{
			tree->insert(num);
			num = 0;
			count++;
			if (count == keynum)break;
		}
		else {
			num *= 10;
			num += input[i] - '0';
		}
	}
	/*for (int i = 0; i < keynum; i++){
		int tempKey;
		cin >> tempKey;
		if (cin.fail())
			cinFail();
		tree->insert(tempKey);
	}*/
	cout << "----------BTree----------" << endl;
	tree->printWholeTree();

	while (true) {
		cout << "输入要进行的操作： 1.插入 I  2.删除 D  3.连续插入 MI  4.连续删除 MD  5.退出 E\n（连续输入以逗号间隔，#结束）" << endl;
		int tempKey;
		string order;
		cin >> order;
		if (order.compare("I") == 0) {
			while (true) {
				cin >> tempKey;
				if (cin.fail()) {//输入检查
					cinFail();
					cout << "请重新输入" << endl;
				}
				else break;
			}
			if (!tree->insert(tempKey))cout << "键值已存在！" << endl;
		}
		else if (order.compare("D") == 0) {
			while (true) {
				cin >> tempKey;
				if (cin.fail()) {//输入检查
					cinFail();
					cout << "请重新输入" << endl;
				}
				else break;
			}
			if (!tree->erase(tempKey))cout << "未找到键值！" << endl;
		}
		else if (order.compare("MI") == 0) {
			while (true) {//多次输入
				cin >> tempKey;//不太好再做输入检查了，输入后已经直接插入
				if (!tree->insert(tempKey))cout << "键值已存在！" << endl;
				char ch;
				cin >> ch;
				if (ch == '#')break;
			}
		}
		else if (order.compare("MD") == 0) {
			while (true) {//多次输入
				cin >> tempKey;
				if (!tree->erase(tempKey))cout << "未找到键值！" << endl;
				char ch;
				cin >> ch;
				if (ch == '#')break;
			}
		}
		else if (order.compare("E") == 0)break;
		else {
			cout << "指令错误！"<< endl;
		}
		cout << "----------BTree----------" << endl;
		tree->printWholeTree();
		if (!wantContinue())break;
	}
	delete tree;
}

 
int main() {
	test();
	return 0;
}