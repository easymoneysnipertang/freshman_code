#include "List.h"
//test
#include<iostream>
using namespace std;

void list::insert(float co,int index)
{
	node* temp = new node(co, index);
	//cout << temp->co << " " << temp->index << endl;//test
	if (head==nullptr) {//空
		head = temp;
		tail = temp;
		count++;
		return;
	}
	//其他的项开始进入
	if (temp->index > head->index) {//指数最大
		temp->next = head;
		head = temp;
		count++;
		return;
	}
	if (temp->index < tail->index) {//指数最小
		tail->next = temp;
		tail = temp;
		count++;
		return;
	}
	//其他指数
	node* cur = head;
	node* pre = head;
	while (cur != NULL) {//!=tail?
		if (cur->index == temp->index) {//已经出现过的指数
			cur->co += temp->co;//系数相加
			//为0
			if (cur->co == 0) {
				if (cur == tail)tail = pre;
				if (cur == head)head = pre->next;//如果删去的是head，cur和pre是同一个

				pre->next = cur->next;
				
				delete cur;
				//todo:万一是tail？head? √已改
				//todo:删原来？ √在外面释放
				count--;
			}
			//不为0的话count不动
			return;
		}
		
		if (cur->index>temp->index && cur->next->index<temp->index) {//夹住
			count++;
			temp->next = cur->next;
			cur->next = temp;
			return;

		}
		if (cur != head)pre = pre->next;//前缀
		cur = cur->next;
		
	}

}

void list::erase(int index)
{
	node* cur = head;
	if (cur->index == index) {//要删的就是头节点
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
		cout << cur->co;//todo:就是0的话他也会输出?
		if (cur->index != 0)
			cout << "x" << cur->index;
	}
}

list& list::operator+(list& l2)
{
	while (l2.head != nullptr) {
		this->insert(l2.head->co, l2.head->index);
		node* temp = l2.head;
		l2.head = l2.head->next;//释放原空间
		delete temp;
		l2.count--;
	}//你是放舒服了，乘除不方便了啊 √调换一下加减顺序

	/*for (node* cur = l2.head; cur != nullptr; cur = cur->next) {
		this->insert(cur->co,cur->index);
	}*/

	// TODO: 在此处插入 return 语句
	return *this;
}

list& list::operator-(list& l2)
{
	while(l2.head != nullptr) {
		this->insert(-l2.head->co, l2.head->index);
		node* temp = l2.head;
		l2.head = l2.head->next;//释放原空间
		delete temp;
		l2.count--;
	}

	/*for (node* cur = l2.head; cur != nullptr; cur = cur->next) {
		this->insert(-cur->co, cur->index);
	}*/

	// TODO: 在此处插入 return 语句
	return *this;
}

list& list::diff()
{
	erase(0);//常数求导为0
	for (node* cur = head; cur != nullptr; cur = cur->next) {
		cur->co *= cur->index;
		cur->index--;		
	}
	// TODO: 在此处插入 return 语句
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
	// TODO: 在此处插入 return 语句
}

list list::operator/(list& l2)
{
	list l;//ans

	//l.insert(0, 0);//初始化
	while (this->head != nullptr&&this->head->index >= l2.head->index) {//当前被除数最高次会改变，但必须大于除数
		
		float div = head->co / l2.head->co;
		
		list l1, temp;
		l1.insert(div, head->index - l2.head->index);
		temp.insert(div, head->index - l2.head->index);

		l = temp+l;//每位放入答案中	

		l1 = l1 * l2;//除法减下去		

		*this = *this - l1;//todo：减了有问题!√已改
		

				
	}
	// TODO: 在此处插入 return 语句
	return l;
}
