#pragma once
//用数组做的静态链表

template<class le>
class Node {
public:
	le entry;
	int next;
};

template<class le>
class listInArrays
{
	int count;
	int max_size;
	Node<le>* element;
	int available;
	int last_used;
	int head;

	listInArrays(int max) {
		max_size = max;
		element = new Node<le>[max];
		available = last_used = head = 0;
		count = 0;
	}
	bool empty()const {
		return count == 0 ? 1 : 0;
	}
	bool full()const {
		return count == max_size ? 1 : 0;
	}
	int size()const {
		return count;
	}
	void clear();

	bool insert();
	void pop(int n);

};

