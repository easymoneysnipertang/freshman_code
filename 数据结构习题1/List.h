#pragma once

class node {
public:
	int index;
	float co;
	node* next;

	node() {
		next = nullptr;
	}
	node(float initCo,int initIndex) {
		next = nullptr;
		index = initIndex;
		co = initCo;
	}
};
class list
{
public:
	int count;
	node* head;
	node* tail;


	
	list() {
		head = tail = nullptr;
		count = 0;
	}
	bool isEmpty() { return count == 0 ? 1 : 0; }
	

	void insert(float co,int index);
	void erase(int index);
	void show();

	list& operator+(list& l2);
	list& operator-(list& l2);
	list& diff();
	list operator*(list& l2);
	list operator/(list& l2);
	

};

