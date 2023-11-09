#include<iostream>
#include"BTree.h"
#include<string>
#include<sstream>
using namespace std;

bool cinFail() {
	if (!cin.fail())return 0;
	cout << "��ʽ����" << endl;
	cin.clear();
	cin.ignore(1024, '\n');
	return 0;
}
bool wantContinue() {
	cout << "�Ƿ������Y or N" << endl;
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
		cout << "����ؼ��ָ�����";
		cin >> keynum;
		if (cin.fail()) {
			cinFail();
			cout << "����������" << endl;
			continue;
		}
		cout << "����B��������";
		cin >> m;
		if (cin.fail()) {
			cinFail();
			cout << "����������" << endl;
		}
		else break;
	}
	
	BTree<int>* tree = new BTree<int>(m);
	cout << endl << "�����ʼ���ؼ��֣�" << endl;

	string input;
	cin >> input;
	int num = 0, count = 0;
	for (int i = 0; i <= input.length(); i++) {
		if (input[i] == ',' || input[i] == '\0')//��������
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
		cout << "����Ҫ���еĲ����� 1.���� I  2.ɾ�� D  3.�������� MI  4.����ɾ�� MD  5.�˳� E\n�����������Զ��ż����#������" << endl;
		int tempKey;
		string order;
		cin >> order;
		if (order.compare("I") == 0) {
			while (true) {
				cin >> tempKey;
				if (cin.fail()) {//������
					cinFail();
					cout << "����������" << endl;
				}
				else break;
			}
			if (!tree->insert(tempKey))cout << "��ֵ�Ѵ��ڣ�" << endl;
		}
		else if (order.compare("D") == 0) {
			while (true) {
				cin >> tempKey;
				if (cin.fail()) {//������
					cinFail();
					cout << "����������" << endl;
				}
				else break;
			}
			if (!tree->erase(tempKey))cout << "δ�ҵ���ֵ��" << endl;
		}
		else if (order.compare("MI") == 0) {
			while (true) {//�������
				cin >> tempKey;//��̫�������������ˣ�������Ѿ�ֱ�Ӳ���
				if (!tree->insert(tempKey))cout << "��ֵ�Ѵ��ڣ�" << endl;
				char ch;
				cin >> ch;
				if (ch == '#')break;
			}
		}
		else if (order.compare("MD") == 0) {
			while (true) {//�������
				cin >> tempKey;
				if (!tree->erase(tempKey))cout << "δ�ҵ���ֵ��" << endl;
				char ch;
				cin >> ch;
				if (ch == '#')break;
			}
		}
		else if (order.compare("E") == 0)break;
		else {
			cout << "ָ�����"<< endl;
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