#include<iostream>
#include"List.h"
#include<fstream>
#include<string>
using namespace std;

//TODO：用全局函数实现
void ADD(list& poly1, list& poly2) {
	node* pre = nullptr;
	//p1是首项大的那一条
	node* p1 = poly1.head, * p2 = poly2.head;

	//node* p1 = (poly1.head->index >= poly2.head->index) ? poly1.head : poly2.head;
	//node* p2 = (poly1.head->index < poly2.head->index) ? poly1.head : poly2.head;
	////如果这样的话函数外面不知道谁是主链，改变一下，在传参阶段决定主链
	////在外面判断谁是NULL然后再来获取主链也可以

	pre = p1;
	
	while (p1 != nullptr && p2 != nullptr) {//直接把两个链表连接到一起
		if (p1->index > p2->index) {
			pre = p1;//以p1为主链
			p1 = p1->next;
			
		}
		else if (p1->index < p2->index) {
			node* temp = p2;
			p2 = p2->next;
			pre->next = temp;//连接到主链上去
			temp->next = p1;
			pre = temp;
			
		}
		else if (p1->index == p2->index) {
			float tempC;
			tempC = p1->co + p2->co;
			if (tempC != 0) {
				p1->co = tempC;
				pre = p1;
			}
			else {
				pre->next = p1->next;
				delete p1;//为0 p1也得删除
			}
			p1 = pre->next;
			node* temp = p2;
			p2 = p2->next;
			delete temp;//加过去了，无论如何都会删除

		}
		
	}
	
	//如果是p2到头了，那么所有东西都连接到主链上去了
	//如果是p1到头了，那么p2还剩有一些node
	if (p2 != nullptr) {
		pre->next = p2;
	}
}
void SUB(list &poly1, list &poly2) {
	node* pre = nullptr;
	//p1是首项大的那一条
	node* p1 = poly1.head, * p2 = poly2.head;
	pre = p1;

	while (p1 != nullptr && p2 != nullptr) {//直接把两个链表连接到一起
		if (p1->index > p2->index) {
			pre = p1;//以p1为主链
			p1 = p1->next;

		}
		else if (p1->index < p2->index) {
			node* temp = p2;
			p2 = p2->next;
			pre->next = temp;//连接到主链上去
			temp->next = p1;

			temp->co = -temp->co;
			pre = temp;

		}
		else if (p1->index == p2->index) {
			float tempC;
			tempC = p1->co - p2->co;
			if (tempC != 0) {
				p1->co = tempC;
				pre = p1;
			}
			else {
				pre->next = p1->next;
				delete p1;//为0 p1也得删除
			}
			p1 = pre->next;
			node* temp = p2;
			p2 = p2->next;
			delete temp;//加过去了，无论如何都会删除

		}

	}
	if (p2 != nullptr) {
		pre->next = p2;
	}
}
void MUL(list &poly1, list &poly2) {
	//不好改，你始终得那啥，新建一个list
}
void DIV(list &poly1, list &poly2) {
	//除法已经做的很好了！
}
void DIFF(list &poly1) {
	//这也没啥好说的了
	poly1.erase(0);//去除常数项
	for (node* temp = poly1.head; temp != NULL; temp = temp->next) {
		temp->co = temp->co * temp->index;
		temp->index--;
	}

}

void test01() {
	list l1, l2;
	node no1(5, 100), no2(9, 31), no3(-7, 2), no4(1, 1), no5(10, 0);
	l1.insert(no1.co,no1.index);
	l1.insert(no2.co, no2.index); 
	l1.insert(no3.co, no3.index);
	l1.insert(no4.co, no4.index);
	l1.insert(no5.co, no5.index);
	node n1(-60, 81), n2(-9, 31), n3(5, 2), n4(1, 1);
	l2.insert(n1.co,n1.index);
	l2.insert(n2.co, n2.index);
	l2.insert(n3.co, n3.index);
	l2.insert(n4.co, n4.index);
	
	
	SUB(l1, l2);
	
	//l1 = l1 - l2;//sub
	
	//l1 = l1 + l2;//and

	//l1 = l1.diff();//diff

	l1.show();
}
void test02() {
	list l1, l2;
	node no1(1, 2), no2(1, 1), no3(1, 0), no4(1, 1), no5(1, 0);
	l1.insert(no1.co, no1.index);
	l1.insert(no2.co, no2.index);
	l1.insert(no3.co, no3.index);
	l2.insert(no4.co, no4.index);
	l2.insert(no5.co, no5.index);

	list l3 = l1 * l2;
	l3.show();//mul

	//list l3 = l1 / l2;
	//l3.show();//div
	//cout << " left: ";
	//l1.show();//left
}

void inOutFile() {
	ifstream infile("inputfile.txt");
	//ifstream test("inputfile.txt");
	float co;
	int index;//每一次对应接收
	char leftBracket, rightBracket, comma;
	string operate;//操作符
	list poly1, poly2;
	list ansPoly;//list主要保存都是指针

	infile >> leftBracket >> co >> comma >> index >> rightBracket;//按格式读入
	while (co != 0) {//当系数为0说明第一个多项式已经读入完成
		poly1.insert(co, index);//前置读一次，直接写入
		infile >> leftBracket >> co >> comma >> index >> rightBracket;
	}
	//微分操作
	infile >> leftBracket;
	if (leftBracket != '(') {
		infile.close();
		ansPoly=poly1.diff();
		ofstream outfile("outputfile.txt");
		for (node* cur = ansPoly.head; cur != nullptr; cur = cur->next) {
			if (cur->co > 0 && cur != ansPoly.head)outfile << "+";
			if (cur->index != 0) {
				if (cur->co == 1);
				else if (cur->co == -1)outfile << "-";
				else outfile << cur->co;

				outfile << "x" << cur->index;
			}
			else {
				outfile << cur->co;
			}
		}
		outfile.close();

		cout << "allright!";

		return;
	}

	//非微分操作
	infile >> co >> comma >> index >> rightBracket;
	while (co != 0) {
		poly2.insert(co, index);
		infile >> leftBracket >> co >> comma >> index >> rightBracket;
	}
	infile >> operate;
	infile.close();
	
	ofstream outfile("outputfile.txt");
	
	if (!operate.compare("AND(#,#)")) {
		ansPoly = poly1 + poly2;
	}
	else if (!operate.compare("SUB(#,#)")) {
		ansPoly = poly1 - poly2;
	}
	else if (!operate.compare("MUL(#,#)")) {
		ansPoly = poly1 * poly2;
	}
	else if (!operate.compare("DIV(#,#)")) {
		ansPoly = poly1 / poly2;
		
	}

	//show函数直接拿过来的
	for (node* cur = ansPoly.head; cur != nullptr; cur = cur->next) {
		if (cur->co > 0 && cur != ansPoly.head)outfile << "+";
		if (cur->index != 0) {
			if (cur->co == 1);
			else if (cur->co == -1)outfile << "-";
			else outfile << cur->co;

			outfile<< "x" << cur->index;
		}
		else {
			outfile << cur->co;
		}
	}
	if (!operate.compare("DIV(#,#)")) {//除法补充余数
		outfile << "\nleft: ";
		for (node* cur = poly1.head; cur != nullptr; cur = cur->next) {
			if (cur->co > 0 && cur != ansPoly.head)outfile << "+";
			if (cur->index != 0) {
				if (cur->co == 1);
				else if (cur->co == -1)outfile << "-";
				else outfile << cur->co;

				outfile << "x" << cur->index;
			}
			else {
				outfile << cur->co;
			}
		}
	}
	outfile.close();
	cout << "all right！";
	return;
		
}
int main() {
	/*list l1, l2;
	l1.insert(1, 2);
	l2.insert(6, 3);
	l2.insert(1, 0);
	l1 = l1 * l2;
	l1.show();*/
	//test01();
	inOutFile();
	
	return 0;
}