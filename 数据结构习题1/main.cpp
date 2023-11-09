#include<iostream>
#include"List.h"
#include<fstream>
#include<string>
using namespace std;

//TODO����ȫ�ֺ���ʵ��
void ADD(list& poly1, list& poly2) {
	node* pre = nullptr;
	//p1����������һ��
	node* p1 = poly1.head, * p2 = poly2.head;

	//node* p1 = (poly1.head->index >= poly2.head->index) ? poly1.head : poly2.head;
	//node* p2 = (poly1.head->index < poly2.head->index) ? poly1.head : poly2.head;
	////��������Ļ��������治֪��˭���������ı�һ�£��ڴ��ν׶ξ�������
	////�������ж�˭��NULLȻ��������ȡ����Ҳ����

	pre = p1;
	
	while (p1 != nullptr && p2 != nullptr) {//ֱ�Ӱ������������ӵ�һ��
		if (p1->index > p2->index) {
			pre = p1;//��p1Ϊ����
			p1 = p1->next;
			
		}
		else if (p1->index < p2->index) {
			node* temp = p2;
			p2 = p2->next;
			pre->next = temp;//���ӵ�������ȥ
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
				delete p1;//Ϊ0 p1Ҳ��ɾ��
			}
			p1 = pre->next;
			node* temp = p2;
			p2 = p2->next;
			delete temp;//�ӹ�ȥ�ˣ�������ζ���ɾ��

		}
		
	}
	
	//�����p2��ͷ�ˣ���ô���ж��������ӵ�������ȥ��
	//�����p1��ͷ�ˣ���ôp2��ʣ��һЩnode
	if (p2 != nullptr) {
		pre->next = p2;
	}
}
void SUB(list &poly1, list &poly2) {
	node* pre = nullptr;
	//p1����������һ��
	node* p1 = poly1.head, * p2 = poly2.head;
	pre = p1;

	while (p1 != nullptr && p2 != nullptr) {//ֱ�Ӱ������������ӵ�һ��
		if (p1->index > p2->index) {
			pre = p1;//��p1Ϊ����
			p1 = p1->next;

		}
		else if (p1->index < p2->index) {
			node* temp = p2;
			p2 = p2->next;
			pre->next = temp;//���ӵ�������ȥ
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
				delete p1;//Ϊ0 p1Ҳ��ɾ��
			}
			p1 = pre->next;
			node* temp = p2;
			p2 = p2->next;
			delete temp;//�ӹ�ȥ�ˣ�������ζ���ɾ��

		}

	}
	if (p2 != nullptr) {
		pre->next = p2;
	}
}
void MUL(list &poly1, list &poly2) {
	//���øģ���ʼ�յ���ɶ���½�һ��list
}
void DIV(list &poly1, list &poly2) {
	//�����Ѿ����ĺܺ��ˣ�
}
void DIFF(list &poly1) {
	//��Ҳûɶ��˵����
	poly1.erase(0);//ȥ��������
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
	int index;//ÿһ�ζ�Ӧ����
	char leftBracket, rightBracket, comma;
	string operate;//������
	list poly1, poly2;
	list ansPoly;//list��Ҫ���涼��ָ��

	infile >> leftBracket >> co >> comma >> index >> rightBracket;//����ʽ����
	while (co != 0) {//��ϵ��Ϊ0˵����һ������ʽ�Ѿ��������
		poly1.insert(co, index);//ǰ�ö�һ�Σ�ֱ��д��
		infile >> leftBracket >> co >> comma >> index >> rightBracket;
	}
	//΢�ֲ���
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

	//��΢�ֲ���
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

	//show����ֱ���ù�����
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
	if (!operate.compare("DIV(#,#)")) {//������������
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
	cout << "all right��";
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