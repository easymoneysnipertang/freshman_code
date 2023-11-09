#include "List.h"
//test
#include<iostream>
using namespace std;

void list::insert(float co,int index)
{
	node* temp = new node(co, index);
	//cout << temp->co << " " << temp->index << endl;//test
	if (head==nullptr) {//��
		head = temp;
		tail = temp;
		count++;
		return;
	}
	//�������ʼ����
	if (temp->index > head->index) {//ָ�����
		temp->next = head;
		head = temp;
		count++;
		return;
	}
	if (temp->index < tail->index) {//ָ����С
		tail->next = temp;
		tail = temp;
		count++;
		return;
	}
	//����ָ��
	node* cur = head;
	node* pre = head;
	while (cur != NULL) {//!=tail?
		if (cur->index == temp->index) {//�Ѿ����ֹ���ָ��
			cur->co += temp->co;//ϵ�����
			//Ϊ0
			if (cur->co == 0) {
				if (cur == tail)tail = pre;
				if (cur == head)head = pre->next;//���ɾȥ����head��cur��pre��ͬһ��

				pre->next = cur->next;
				
				delete cur;
				//todo:��һ��tail��head? ���Ѹ�
				//todo:ɾԭ���� ���������ͷ�
				count--;
			}
			//��Ϊ0�Ļ�count����
			return;
		}
		
		if (cur->index>temp->index && cur->next->index<temp->index) {//��ס
			count++;
			temp->next = cur->next;
			cur->next = temp;
			return;

		}
		if (cur != head)pre = pre->next;//ǰ׺
		cur = cur->next;
		
	}

}

void list::erase(int index)
{
	node* cur = head;
	if (cur->index == index) {//Ҫɾ�ľ���ͷ�ڵ�
		head = cur->next;
		delete cur;
		count--;
		return;
	}
	while (cur->next != nullptr) {
		if (cur->next->index == index) {
			node* temp = cur->next;
			cur->next = cur->next->next;
			delete temp;
			count--;
			return;
		}
		cur = cur->next;
	}
	//test
	cout << "nothing is deleted";
}

void list::show()
{
	for (node* cur = head; cur != nullptr; cur = cur->next) {
		if (cur->co > 0 && cur != head)cout << "+";
		cout << cur->co;//todo:����0�Ļ���Ҳ�����?
		if (cur->index != 0)
			cout << "x" << cur->index;
	}
}

list& list::operator+(list& l2)
{
	while (l2.head != nullptr) {
		this->insert(l2.head->co, l2.head->index);
		node* temp = l2.head;
		l2.head = l2.head->next;//�ͷ�ԭ�ռ�
		delete temp;
		l2.count--;
	}//���Ƿ�����ˣ��˳��������˰� �̵���һ�¼Ӽ�˳��

	/*for (node* cur = l2.head; cur != nullptr; cur = cur->next) {
		this->insert(cur->co,cur->index);
	}*/

	// TODO: �ڴ˴����� return ���
	return *this;
}

list& list::operator-(list& l2)
{
	while(l2.head != nullptr) {
		this->insert(-l2.head->co, l2.head->index);
		node* temp = l2.head;
		l2.head = l2.head->next;//�ͷ�ԭ�ռ�
		delete temp;
		l2.count--;
	}

	/*for (node* cur = l2.head; cur != nullptr; cur = cur->next) {
		this->insert(-cur->co, cur->index);
	}*/

	// TODO: �ڴ˴����� return ���
	return *this;
}

list& list::diff()
{
	erase(0);//������Ϊ0
	for (node* cur = head; cur != nullptr; cur = cur->next) {
		cur->co *= cur->index;
		cur->index--;		
	}
	// TODO: �ڴ˴����� return ���
	return *this;
}

list list::operator*(list& l2)
{
	list l;
	for (node* cur = head; cur != nullptr; cur = cur->next) {
		for (node* temp = l2.head; temp != nullptr; temp = temp->next) {
			l.insert(cur->co * temp->co, cur->index + temp->index);
		}
	}

	return l;
	// TODO: �ڴ˴����� return ���
}

list list::operator/(list& l2)
{
	list l;//ans

	//l.insert(0, 0);//��ʼ��
	while (this->head != nullptr&&this->head->index >= l2.head->index) {//��ǰ��������ߴλ�ı䣬��������ڳ���
		
		float div = head->co / l2.head->co;
		
		list l1, temp;
		l1.insert(div, head->index - l2.head->index);
		temp.insert(div, head->index - l2.head->index);

		l = temp+l;//ÿλ�������	

		l1 = l1 * l2;//��������ȥ		

		*this = *this - l1;//todo������������!���Ѹ�
		

				
	}
	// TODO: �ڴ˴����� return ���
	return l;
}
