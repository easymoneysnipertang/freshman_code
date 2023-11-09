#include "List.h"

template<class T>
void List<T>::insert(int index, T theElement)
{	
	
	node<T>* temp = new node<T>(theElement);
	if (index == 0) {
		//表头插入
		temp->next = head;
		head = temp;
	}
	else {
		node<T>* p = head;
		for (int i = 0; i < index - 1; i++)p = p->next;
		temp->next = p->next;
		p->next = temp;
	}
	count++;
}

template<class T>
void List<T>::erase(int index)
{
	
	node<T>* denode;
	if (index == 0) {
		denode = head;
		head = head->next;
	}
	else {
		node<T>* current = head;
		for (int i = 0; i < index-1; i++)current = current->next;
		denode = current->next;
		current->next = current->next->next;
	}
	count--;
	delete denode;
}

template<class T>
T& List<T>::get(int index)
{
	
	node<T>* current = head;
	for (int i = 0; i < index; i++) {
		current = current->next;
	}
	return current->element;
	// TODO: 在此处插入 return 语句
}

template<class T>
void List<T>::show()
{
	node<T>* p = head;
	while (p != nullptr) {
		cout << p->element << " ";
		p = p->next;
	}
}

template<class T>
void List<T>::reverseOutput(node<T>*p) 
{
	if (p->next == nullptr) {
		cout << p->element << " ";
		return;
	}
	else {
		reverseOutput(p->next);
	}
	cout << p->element << " ";
}
