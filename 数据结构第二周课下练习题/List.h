#pragma once
#include<iostream>
using namespace std;

template<class T>
class node {
public:
	T element;
	node<T>* next;
	node(T temp) {
		element = temp;
		next = nullptr;
	}
};
template<class T>
class List
{
public:
	node<T>* head;
	int count;

	List() {
		count = 0;
		head = nullptr;
	}
	~List() {
		while (head != nullptr) {
			node<T>* next = head->next;
			delete head;
			head = next;
		}
	}
	bool isEmpty() {
		return count == 0 ? true : false;
	}
	bool checkIndex(int index) {
		if (index<0 || index>count)return false;
		return true;
	}
	void insert(int index, T theElement);
	void erase(int index);
	T& get(int index);
	void show();

	//µ¹ÐòÊä³ö
	void reverseOutput(node<T>*p);
};
