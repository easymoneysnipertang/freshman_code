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

class InvertedIndex {// ���������ṹ
public:
	int length = 0;
	vector<unsigned int> docIdList;
};
// ���رȽϷ����Գ��������������
bool operator<(const InvertedIndex& i1, const InvertedIndex& i2) {
	return i1.length < i2.length;
}

// �ѵ����б���������
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
	InvertedIndex s = index[queryList[0]];// ȡ��̵��б�

	// ��ʣ���б���
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
	// ��ȡ�������ļ�
	fstream file;
	file.open("ExpIndex", ios::binary | ios::in);
	if (!file.is_open()) {
		cout << "Wrong in opening file!";
		return;
	}
	static vector<InvertedIndex>* invertedLists = new vector<InvertedIndex>();
	for (int i = 0; i < 2000; i++)		//�ܹ���ȡ2000����������
	{
		InvertedIndex* t = new InvertedIndex();				//һ����������
		file.read((char*)&t->length, sizeof(t->length));
		for (int j = 0; j < t->length; j++)
		{
			unsigned int docId;			//�ļ�id
			file.read((char*)&docId, sizeof(docId));
			t->docIdList.push_back(docId);//���������ű�
		}
		sort(t->docIdList.begin(), t->docIdList.end());//���ĵ��������
		invertedLists->push_back(*t);		//����һ�����ű�
	}
	file.close();
	cout << "here" << endl;

	// ��ȡ��ѯ����
	file.open("ExpQuery", ios::in);
	static int query[1000][5] = { 0 };// ������ѯ���5��docId,ȫ����ȡ
	string line;
	int count = 0;

	while (getline(file, line)) {// ��ȡһ��
		stringstream ss; // �ַ���������
		ss << line; // ������һ��
		int i = 0;
		int temp;
		ss >> temp;
		while (!ss.eof()) {
			query[count][i] = temp;
			i++;
			ss >> temp;
		}
		count++;// �ܲ�ѯ��
	}

	//------��------
	long long head, tail, freq;

	QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
	QueryPerformanceCounter((LARGE_INTEGER*)&head);// Start Time

	//preprocessing(*invertedLists, 2000);

	for (int i = 0; i < count; i++) {// count����ѯ
		int num = 0;// query��ѯ����
		for (int j = 0; j < 5; j++) {
			if (query[i][j] != 0) {
				num++;
			}
		}
		int* list = new int[num];// Ҫ�����list
		for (int j = 0; j < num; j++) {
			list[j] = query[i][j];
		}
		sorted(list, *(invertedLists), num);// ����������
		InvertedIndex res = STL_intersection(list, *invertedLists, num);
		cout << i << endl;
	}
	QueryPerformanceCounter((LARGE_INTEGER*)&tail);// End Time
	cout << (tail - head) * 1000.0 / freq;
}

void testCompress() {
	// ��ȡ�������ļ�
	fstream file;
	file.open("ExpIndex", ios::binary | ios::in);
	if (!file.is_open()) {
		cout << "Wrong in opening file!";
		return;
	}
	static vector<InvertedIndex>* invertedLists = new vector<InvertedIndex>();
	for (int i = 0; i < 100; i++)	{	//�ܹ���ȡ2000����������
		InvertedIndex* t = new InvertedIndex();	//һ����������
		file.read((char*)&t->length, sizeof(t->length));
		for (int j = 0; j < t->length; j++){
			unsigned int docId;			//�ļ�id
			file.read((char*)&docId, sizeof(docId));
			t->docIdList.push_back(docId);//���������ű�
		}
		sort(t->docIdList.begin(), t->docIdList.end());//���ĵ��������
		invertedLists->push_back(*t);		//����һ�����ű�
	}
	file.close();
	cout << "here" << endl;

	//for (unsigned i : (*invertedLists)[1].docIdList)
	//	cout << i << " ";
	//cout << endl;
	PForCompressor pfd;
	fstream testOut;
	testOut.open("testOut.bin", ios::binary | ios::out);// ����ѹ������

	for (int i = 0; i < 200; i++) {
		PForBlock pfdResult = pfd.compress((*invertedLists)[i].docIdList);
		cout << (*invertedLists)[i].length << " " << pfdResult.data.size() << endl;
		cout << i << endl;

	}
	testOut.close();


	//PForBlock pfdResult = pfd.compress((*invertedLists)[1].docIdList);
	//cout << pfdResult.a << " " << pfdResult.b << endl;

	//fstream testOut;
	//testOut.open("testOut.bin", ios::binary | ios::out);// ����ѹ������
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