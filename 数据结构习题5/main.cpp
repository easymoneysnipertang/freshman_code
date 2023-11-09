#include<iostream>
#include"myHashTable.h"
#include<fstream>
using  namespace std;

bool isPrime(int n) {
	if (n <= 1)return false;
	for (int i = 2; i < sqrt(2); i++)
		if (n % i == 0)return false;
	return true;
}

void inOutFile() {
	ifstream countFile("inputFile.txt"),inFile("inputFile.txt");
	string str;
	int count = 0;
	while (!countFile.eof())
	{
		countFile >> str;
		count++;
	}
	countFile.close();
	myHashTable my(count);
	while (!inFile.eof()) {
		inFile >> str;
		my.insert(str);
	}
	inFile.close();
	/*my.output();
	cout << "--------------------------" << endl;*/

	ifstream toFind("findFile.txt");
	int strCount = 0;
	my.searchCount = 0;//查找数置零记录查找次数查找
	while (!toFind.eof()) {
		strCount++;
		toFind >> str;
		my.find(str);
	}
	toFind.close();
	/*cout << "strCount: " << strCount << endl;
	cout << "asl: " << (float)my.searchCount / strCount << endl;*/
	cout << "success asl: " << (float)my.sum1 / my.successCount << endl;
	cout << "fail asl: " << (float)my.sum2 / my.failCount << endl;
}

void test01() {
	myHashTable my(7);
	my.insert("absde");
	if (my.find("absde") == nullptr)cout << endl << "no match";
	my.insert("absde");
	my.insert("absdef");
	my.insert("absdeg");
	my.output();
}
int main() {
	//test01();
	inOutFile();
	return 0;
}