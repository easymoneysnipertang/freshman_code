#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<sstream>
#include<Windows.h>
#include<algorithm>
#include"PFDCompress.h"
#include"PFDDecompress.h"

using namespace std;

class InvertedIndex {// 倒排索引结构
public:
	int length = 0;
	vector<unsigned int> docIdList;
};
// 重载比较符，以长度排序各个索引
bool operator<(const InvertedIndex& i1, const InvertedIndex& i2) {
	return i1.length < i2.length;
}

// 把倒排列表按长度排序
void sorted(int* list, vector<InvertedIndex>& idx, int num) {
	for (int i = 0; i < num - 1; i++) {
		for (int j = 0; j < num - i - 1; j++) {
			if (idx[list[j]].length > idx[list[j + 1]].length) {
				int tmp = list[j];
				list[j] = list[j + 1];
				list[j + 1] = tmp;
			}
		}
	}
}

InvertedIndex STL_intersection(int* queryList, vector<InvertedIndex>& index, int num) {
	InvertedIndex s = index[queryList[0]];// 取最短的列表

	// 与剩余列表求交
	for (int i = 1; i < num; i++) {
		vector<int> temp;
		set_intersection(s.docIdList.begin(), s.docIdList.end(),
			index[queryList[i]].docIdList.begin(), index[queryList[i]].docIdList.end(),
			inserter(temp, temp.begin())
		);
		s.docIdList.clear();
		s.docIdList.assign(temp.begin(), temp.end());
	}
	s.length = s.docIdList.size();
	return s;
}

void mainFunc() {
	// 读取二进制文件
	fstream file;
	file.open("ExpIndex", ios::binary | ios::in);
	if (!file.is_open()) {
		cout << "Wrong in opening file!";
		return;
	}
	static vector<InvertedIndex>* invertedLists = new vector<InvertedIndex>();
	for (int i = 0; i < 2000; i++)		//总共读取2000个倒排链表
	{
		InvertedIndex* t = new InvertedIndex();				//一个倒排链表
		file.read((char*)&t->length, sizeof(t->length));
		for (int j = 0; j < t->length; j++)
		{
			unsigned int docId;			//文件id
			file.read((char*)&docId, sizeof(docId));
			t->docIdList.push_back(docId);//加入至倒排表
		}
		sort(t->docIdList.begin(), t->docIdList.end());//对文档编号排序
		invertedLists->push_back(*t);		//加入一个倒排表
	}
	file.close();
	cout << "here" << endl;

	// 读取查询数据
	file.open("ExpQuery", ios::in);
	static int query[1000][5] = { 0 };// 单个查询最多5个docId,全部读取
	string line;
	int count = 0;

	while (getline(file, line)) {// 读取一行
		stringstream ss; // 字符串输入流
		ss << line; // 传入这一行
		int i = 0;
		int temp;
		ss >> temp;
		while (!ss.eof()) {
			query[count][i] = temp;
			i++;
			ss >> temp;
		}
		count++;// 总查询数
	}

	//------求交------
	long long head, tail, freq;

	QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
	QueryPerformanceCounter((LARGE_INTEGER*)&head);// Start Time

	//preprocessing(*invertedLists, 2000);

	for (int i = 0; i < count; i++) {// count个查询
		int num = 0;// query查询项数
		for (int j = 0; j < 5; j++) {
			if (query[i][j] != 0) {
				num++;
			}
		}
		int* list = new int[num];// 要传入的list
		for (int j = 0; j < num; j++) {
			list[j] = query[i][j];
		}
		sorted(list, *(invertedLists), num);// 按长度排序
		InvertedIndex res = STL_intersection(list, *invertedLists, num);
		cout << i << endl;
	}
	QueryPerformanceCounter((LARGE_INTEGER*)&tail);// End Time
	cout << (tail - head) * 1000.0 / freq;
}

void testCompress() {
	// 读取二进制文件
	fstream file;
	file.open("ExpIndex", ios::binary | ios::in);
	if (!file.is_open()) {
		cout << "Wrong in opening file!";
		return;
	}
	static vector<InvertedIndex>* invertedLists = new vector<InvertedIndex>();
	for (int i = 0; i < 100; i++)	{	//总共读取2000个倒排链表
		InvertedIndex* t = new InvertedIndex();	//一个倒排链表
		file.read((char*)&t->length, sizeof(t->length));
		for (int j = 0; j < t->length; j++){
			unsigned int docId;			//文件id
			file.read((char*)&docId, sizeof(docId));
			t->docIdList.push_back(docId);//加入至倒排表
		}
		sort(t->docIdList.begin(), t->docIdList.end());//对文档编号排序
		invertedLists->push_back(*t);		//加入一个倒排表
	}
	file.close();
	cout << "here" << endl;

	//for (unsigned i : (*invertedLists)[1].docIdList)
	//	cout << i << " ";
	//cout << endl;
	PForCompressor pfd;
	fstream testOut;
	testOut.open("testOut.bin", ios::binary | ios::out);// 保存压缩数据

	for (int i = 0; i < 200; i++) {
		PForBlock pfdResult = pfd.compress((*invertedLists)[i].docIdList);
		cout << (*invertedLists)[i].length << " " << pfdResult.data.size() << endl;
		cout << i << endl;

	}
	testOut.close();


	//PForBlock pfdResult = pfd.compress((*invertedLists)[1].docIdList);
	//cout << pfdResult.a << " " << pfdResult.b << endl;

	//fstream testOut;
	//testOut.open("testOut.bin", ios::binary | ios::out);// 保存压缩数据
	//for (int i=0; i< pfdResult.data.size();i++){
	//		testOut.write((char*)&pfdResult.data[i], sizeof(unsigned));
	//}
	//testOut.close();

	//unsigned* result, * data = new unsigned[pfdResult.data.size()];
	//fstream testIn;
	//testIn.open("testOut.bin", ios::binary | ios::in);
	//for (int i = 0; i < pfdResult.data.size(); i++) {
	//	testIn.read((char*)&data[i], sizeof(unsigned));
	//}
	//testIn.close();


	//PForDecompressor pfdD;
	//result = new unsigned[(*invertedLists)[1].length];
	//pfdD.Decompress(pfdResult.a, pfdResult.b, (*invertedLists)[1].length, data,pfdResult.data.size(), result);

	//for (int i = 0; i < (*invertedLists)[1].length; i++)
	//	cout << result[i] << " ";
}

int main() {
	//mainFunc();
	testCompress();
	return 0;
}