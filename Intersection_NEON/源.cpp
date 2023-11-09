#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include <algorithm>
#include <arm_neon.h>
#include <stdio.h>
#include <time.h>
#include <string>
#include <sstream>
#include<sys/time.h>
#include<stdlib.h>//��Ϊlinux���õļ�ʱ��
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
// svsʵ��
InvertedIndex SVS(int* queryList, vector<InvertedIndex>& index, int num) {
	InvertedIndex s = index[queryList[0]];// ȡ��̵��б�

	// ��ʣ���б���
	for (int i = 1; i < num; i++) {
		int count = 0;// s��ͷ�������һ��
		int t = 0;
		// s�б��е�ÿ��Ԫ�ض��ó����Ƚ�
		for (int j = 0; j < s.length; j++) {// ����Ԫ�ض��÷���һ��
			bool isFind = false;// ��־���жϵ�ǰcountλ�Ƿ�����

			for (; t < index[queryList[i]].length; t++) {
				// ����i�б�������Ԫ��
				if (s.docIdList[j] == index[queryList[i]].docIdList[t]) {
					isFind = true;
					break;
				}
				else if (s.docIdList[j] < index[queryList[i]].docIdList[t])// ��������
					break;
			}
			if (isFind)// ����
				s.docIdList[count++] = s.docIdList[j];
		}
		if (count < s.length)// ������ɾ��
			s.docIdList.erase(s.docIdList.begin() + count, s.docIdList.end());
		s.length = count;
	}
	return s;
}

InvertedIndex SVS_NEON(int* queryList, vector<InvertedIndex>& index, int num) {
	InvertedIndex s = index[queryList[0]];// ȡ��̵��б�

	// ��ʣ���б���
	for (int i = 1; i < num; i++) {
		int count = 0;// s��ͷ�������һ��
		int t = 0;
		// s�б��е�ÿ��Ԫ�ض��ó����Ƚ�
		for (int j = 0; j < s.length; j++) {// ����Ԫ�ض��÷���һ��
			bool isFind = false;// ��־���жϵ�ǰcountλ�Ƿ�����

			// ����������4�ı���������λ�ò�0�������ٴ�������
			int length = ceil(index[queryList[i]].length / 4) * 4;
			for (int l = index[queryList[i]].length; l < length; l++) {// ĩβ��0�����Ը��õĶ���
				index[queryList[i]].docIdList[l] = 0;
			}
			uint32x4_t ss = vmovq_n_u32(s.docIdList[j]);// ����ĸ�λ��

			for (; t < length; t+=4) {
				uint32x4_t ii = vld1q_u32(&index[i].docIdList[t]);// ��ȡ�ĸ�����
				uint32x4_t temp = vceqq_u32(ss, ii);
				//uint32_t mask = vmovemaskq_u32(temp);// �ò���
				unsigned int result[4] = {0};
				vst1q_u32(result, temp);
				if (result[0] == 1 || result[1] == 1 || result[2] == 1 || result[3] == 1) {
					isFind = true;
					break;
				}
				else if (s.docIdList[j] < index[queryList[i]].docIdList[t])// ��ʱbreak�����ⳬ������һԪ������ͷ����
					break;

			}
			if (isFind)// ����
				s.docIdList[count++] = s.docIdList[j];
		}
		if (count < s.length)// ������ɾ��
			s.docIdList.erase(s.docIdList.begin() + count, s.docIdList.end());
		s.length = count;
	}
	return s;
}

// adpʵ��
class QueryItem {
public:
	int cursor;// ��ǰ��������
	int length;// ���������ܳ���
	int key;// �ؼ���ֵ
};
bool operator<(const QueryItem& q1, const QueryItem& q2) {// ѡʣ��Ԫ�����ٵ�Ԫ��
	return (q1.length - q1.cursor) < (q2.length - q2.cursor);
}
InvertedIndex ADP(int* queryList, vector<InvertedIndex>& index, int num){
	InvertedIndex S;
	QueryItem* list = new QueryItem[num]();
	for (int i = 0; i < num; i++)// Ԥ����
	{
		list[i].cursor = 0;
		list[i].key = queryList[i];
		list[i].length = index[queryList[i]].length;
	}

	while (list[0].cursor < list[0].length) {// ��̵��б�ǿ�
		bool isFind = true;
		int s = 1;
		unsigned int e = index[list[0].key].docIdList[list[0].cursor];
		while (s != num && isFind == true) {
			isFind = false;
			while (list[s].cursor < list[s].length) {// ���s�б�
				if (e == index[list[s].key].docIdList[list[s].cursor]) {
					isFind = true;
					break;
				}
				else if (e < index[list[s].key].docIdList[list[s].cursor])
					break;
				list[s].cursor++;// ��ǰ���ʹ�����û�ҵ����ʵģ�������
			}
			s++;// ��һ������
		}
		list[0].cursor++;// ��ǰԪ���ѱ����ʹ�
		if (s == num && isFind) {
			S.docIdList.push_back(e);
			S.length++;
		}
		//sort(list, list + num);// ���ţ���δ̽��Ԫ���ٵ��б�ǰ��
	}
	return S;
}
// adp_noen
InvertedIndex ADP_NEON(int* queryList, vector<InvertedIndex>& index, int num) {
	InvertedIndex S;
	QueryItem* list = new QueryItem[num]();
	for (int i = 0; i < num; i++)// Ԥ����
	{
		list[i].cursor = 0;
		list[i].key = queryList[i];
		list[i].length = index[queryList[i]].length;
	}

	while (list[0].cursor < list[0].length) {// ��̵��б�ǿ�
		bool isFind = true;
		int s = 1;
		unsigned int e = index[list[0].key].docIdList[list[0].cursor];
		uint32x4_t ss = vmovq_n_u32(e);// ����ĸ�λ��

		// ����������4�ı���������λ�ò�0�������ٴ�������
		int length = ceil(list[s].length / 4) * 4;
		for (int l = list[s].length; l < length; l++) {// ĩβ��0�����Ը��õĶ���
			index[list[s].key].docIdList[l] = 0;
		}

		while (s != num && isFind == true) {
			isFind = false;
			while (list[s].cursor < length) {// ���s�б�
				// ��������
				uint32x4_t ii = vld1q_u32(&index[list[s].key].docIdList[list[s].cursor]);// ��ȡ�ĸ�����
				uint32x4_t temp = vceqq_u32(ss, ii);
				//uint32_t mask = vmovemaskq_u32(temp);// �ò���
				unsigned int result[4] = { 0 };
				vst1q_u32(result, temp);
				if (result[0] == 1 || result[1] == 1 || result[2] == 1 || result[3] == 1) {
					isFind = true;
					break;
				}
				else if (e < index[list[s].key].docIdList[list[s].cursor])// ��ʱbreak����������cursor
					break;
				list[s].cursor += 4;
			}
			s++;// ��һ������
		}
		list[0].cursor++;// ��ǰԪ���ѱ����ʹ�
		if (s == num && isFind) {
			S.docIdList.push_back(e);
			S.length++;
		}
		//sort(list, list + num);// ���ţ���δ̽��Ԫ���ٵ��б�ǰ��
	}
	return S;
}

InvertedIndex HASH_NEON(int* queryList, vector<InvertedIndex>& index, int num) {
	InvertedIndex s = index[queryList[0]];// ѡ����̵ĵ����б�
	for (int i = 1; i < num; i++) {// ��ʣ���б���
		// SVS��˼�룬��s����������
		int count = 0;
		int length = s.length;

		for (int j = 0; j < length; j++) {
			bool isFind = false;
			int hashValue = s.docIdList[j] / 512;
			// �ҵ���hashֵ�ڵ�ǰ�����б��ж�Ӧ�Ķ�
			int begin = hashBucket[queryList[i]][hashValue].begin;
			int end = hashBucket[queryList[i]][hashValue].end;
			if (begin < 0 || end < 0) {// ��ֵ�϶��Ҳ�����������������
				continue;
			}
			else {// �ڸö�����в��ң�һ�β�4��
				uint32x4_t ss = vmovq_n_u32(s.docIdList[j]);// ����ĸ�λ��

				for (begin; begin <= end - 3; begin += 4) {
					uint32x4_t ii = vld1q_u32(&index[i].docIdList[begin]);// ��ȡ�ĸ�����

					uint32x4_t temp = vceqq_u32(ss, ii);
					unsigned int result[4] = { 0 };
					vst1q_u32(result, temp);
					if (result[0] == 1 || result[1] == 1 || result[2] == 1 || result[3] == 1) {
						isFind = true;
						break;
					}
					else if (s.docIdList[j] < index[queryList[i]].docIdList[begin])// ��ʱbreak�����ⳬ������һԪ������ͷ����
						break;
				}
				if (!isFind && (begin > end - 3)) {// ����ʣ��Ԫ��
					for (; begin <= end; begin++)
					{
						if (s.docIdList[j] == index[queryList[i]].docIdList[begin]) {
							isFind = true;
							break;
						}
						else if (s.docIdList[j] < index[queryList[i]].docIdList[begin])// ��������
							break;
					}
				}
				if (isFind) {
					// ����
					s.docIdList[count++] = s.docIdList[j];
				}
			}
		}
		if (count < length)// ������ɾ��
			s.docIdList.erase(s.docIdList.begin() + count, s.docIdList.end());
		s.length = count;
	}

	return s;
}

void test01() {
	vector<InvertedIndex> testInvertedLists;
	int testQuery[3] = { 0,1,2 };
	for (int i = 0; i < 3; i++)
	{
		InvertedIndex t;
		cin >> t.length;
		for (int j = 0; j < t.length; j++)
		{
			unsigned int docId;
			cin >> docId;
			t.docIdList.push_back(docId);
		}
		sort(t.docIdList.begin(), t.docIdList.end());//���ĵ��������
		testInvertedLists.push_back(t);
	}
	sorted(testQuery, testInvertedLists, 3);
	//preprocessing(testInvertedLists, 3);
	InvertedIndex res = SVS_NEON(testQuery, testInvertedLists, 3);
	for (auto i : res.docIdList)
		cout << i << " ";
}
void test02() {
	srand(time(0));

	vector<InvertedIndex> testInvertedLists;

	for (int i = 0; i < 500; i++)
	{
		InvertedIndex t;// ����500�������б�
		t.length = rand() % (100 - 30 + 1) + 30;
		//cout << t.length << " :";

		vector<int> forRandom;
		for (int j = 0; j < t.length * 5; j++)//Ϊ�˲��ظ�
			forRandom.push_back(j);
		random_shuffle(forRandom.begin(), forRandom.end());//�������

		for (int j = 0; j < t.length; j++)
		{
			int docId = forRandom[j];
			//cout << docId << " ";
			t.docIdList.push_back(docId);
		}
		sort(t.docIdList.begin(), t.docIdList.end());//���ĵ��������
		testInvertedLists.push_back(t);
		//cout << endl;
	}

	timeval start, finish;
	gettimeofday(&start, NULL);// Start Time

	for (int i = 0; i < 200; i++) {// ��200�β�ѯ
		int testQuery[5];
		for (int i = 0; i < 5; i++)testQuery[i] = rand() % 500;

		sorted(testQuery, testInvertedLists, 5);
		InvertedIndex res = SVS_NEON(testQuery, testInvertedLists, 5);
		for (auto i : res.docIdList)
			cout << i << " ";
		if (res.docIdList.size() != 0)
			cout << endl;
	}

	gettimeofday(&finish, NULL);// End Time
	cout << ((finish.tv_sec - start.tv_sec) * 1000000.0 + finish.tv_usec - start.tv_usec) / 1000.0 << endl;
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
	timeval start, finish;
	gettimeofday(&start, NULL);// Start Time

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
		InvertedIndex res = SVS_NEON(list, *invertedLists, num);
		cout << i << endl;
	}

	gettimeofday(&finish, NULL);// End Time
	cout << ((finish.tv_sec - start.tv_sec) * 1000000.0 + finish.tv_usec - start.tv_usec) / 1000.0 << endl;
}



int main() {
	test01();
	//test02();

	//mainFunc();

	return 0;
}