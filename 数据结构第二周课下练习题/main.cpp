#include<iostream>
#include<list>
using namespace std;

list<int>l;
void reverseOutput(list<int>::iterator it) {
	list<int>::iterator temp = it;
	temp++;
	if (temp == l.end()) {
		cout << *it << " ";
		return;
	}
	else reverseOutput(temp);
	cout << *it << " ";
}
void test01() {
	
}
int main() {
	l.push_back(1);
	l.push_back(2);
	l.push_back(3);
	reverseOutput(l.begin());

	return 0;
}