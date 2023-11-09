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
#include<stdlib.h>//改为linux所用的计时包
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
// svs实现
InvertedIndex SVS(int* queryList, vector<InvertedIndex>& index, int num) {
	InvertedIndex s = index[queryList[0]];// 取最短的列表

	// 与剩余列表求交
	for (int i = 1; i < num; i++) {
		int count = 0;// s从头往后遍历一遍
		int t = 0;
		// s列表中的每个元素都拿出来比较
		for (int j = 0; j < s.length; j++) {// 所有元素都得访问一遍
			bool isFind = false;// 标志，判断当前count位是否能求交

			for (; t < index[queryList[i]].length; t++) {
				// 遍历i列表中所有元素
				if (s.docIdList[j] == index[queryList[i]].docIdList[t]) {
					isFind = true;
					break;
				}
				else if (s.docIdList[j] < index[queryList[i]].docIdList[t])// 升序排列
					break;
			}
			if (isFind)// 覆盖
				s.docIdList[count++] = s.docIdList[j];
		}
		if (count < s.length)// 最后才做删除
			s.docIdList.erase(s.docIdList.begin() + count, s.docIdList.end());
		s.length = count;
	}
	return s;
}

InvertedIndex SVS_NEON(int* queryList, vector<InvertedIndex>& index, int num) {
	InvertedIndex s = index[queryList[0]];// 取最短的列表

	// 与剩余列表求交
	for (int i = 1; i < num; i++) {
		int count = 0;// s从头往后遍历一遍
		int t = 0;
		// s列表中的每个元素都拿出来比较
		for (int j = 0; j < s.length; j++) {// 所有元素都得访问一遍
			bool isFind = false;// 标志，判断当前count位是否能求交

			// 将长度做成4的倍数，多余位置补0，不用再处理余数
			int length = ceil(index[queryList[i]].length / 4) * 4;
			for (int l = index[queryList[i]].length; l < length; l++) {// 末尾补0，可以更好的对齐
				index[queryList[i]].docIdList[l] = 0;
			}
			uint32x4_t ss = vmovq_n_u32(s.docIdList[j]);// 填充四个位置

			for (; t < length; t+=4) {
				uint32x4_t ii = vld1q_u32(&index[i].docIdList[t]);// 读取四个数据
				uint32x4_t temp = vceqq_u32(ss, ii);
				//uint32_t mask = vmovemaskq_u32(temp);// 用不了
				unsigned int result[4] = {0};
				vst1q_u32(result, temp);
				if (result[0] == 1 || result[1] == 1 || result[2] == 1 || result[3] == 1) {
					isFind = true;
					break;
				}
				else if (s.docIdList[j] < index[queryList[i]].docIdList[t])// 及时break，避免超过，下一元素需重头再来
					break;

			}
			if (isFind)// 覆盖
				s.docIdList[count++] = s.docIdList[j];
		}
		if (count < s.length)// 最后才做删除
			s.docIdList.erase(s.docIdList.begin() + count, s.docIdList.end());
		s.length = count;
	}
	return s;
}

// adp实现
class QueryItem {
public:
	int cursor;// 当前读到哪了
	int length;// 倒排索引总长度
	int key;// 关键字值
};
bool operator<(const QueryItem& q1, const QueryItem& q2) {// 选剩余元素最少的元素
	return (q1.length - q1.cursor) < (q2.length - q2.cursor);
}
InvertedIndex ADP(int* queryList, vector<InvertedIndex>& index, int num){
	InvertedIndex S;
	QueryItem* list = new QueryItem[num]();
	for (int i = 0; i < num; i++)// 预处理
	{
		list[i].cursor = 0;
		list[i].key = queryList[i];
		list[i].length = index[queryList[i]].length;
	}

	while (list[0].cursor < list[0].length) {// 最短的列表非空
		bool isFind = true;
		int s = 1;
		unsigned int e = index[list[0].key].docIdList[list[0].cursor];
		while (s != num && isFind == true) {
			isFind = false;
			while (list[s].cursor < list[s].length) {// 检查s列表
				if (e == index[list[s].key].docIdList[list[s].cursor]) {
					isFind = true;
					break;
				}
				else if (e < index[list[s].key].docIdList[list[s].cursor])
					break;
				list[s].cursor++;// 当前访问过，且没找到合适的，往后移
			}
			s++;// 下一个链表
		}
		list[0].cursor++;// 当前元素已被访问过
		if (s == num && isFind) {
			S.docIdList.push_back(e);
			S.length++;
		}
		//sort(list, list + num);// 重排，将未探查元素少的列表前移
	}
	return S;
}
// adp_noen
InvertedIndex ADP_NEON(int* queryList, vector<InvertedIndex>& index, int num) {
	InvertedIndex S;
	QueryItem* list = new QueryItem[num]();
	for (int i = 0; i < num; i++)// 预处理
	{
		list[i].cursor = 0;
		list[i].key = queryList[i];
		list[i].length = index[queryList[i]].length;
	}

	while (list[0].cursor < list[0].length) {// 最短的列表非空
		bool isFind = true;
		int s = 1;
		unsigned int e = index[list[0].key].docIdList[list[0].cursor];
		uint32x4_t ss = vmovq_n_u32(e);// 填充四个位置

		// 将长度做成4的倍数，多余位置补0，不用再处理余数
		int length = ceil(list[s].length / 4) * 4;
		for (int l = list[s].length; l < length; l++) {// 末尾补0，可以更好的对齐
			index[list[s].key].docIdList[l] = 0;
		}

		while (s != num && isFind == true) {
			isFind = false;
			while (list[s].cursor < length) {// 检查s列表
				// 构造向量
				uint32x4_t ii = vld1q_u32(&index[list[s].key].docIdList[list[s].cursor]);// 读取四个数据
				uint32x4_t temp = vceqq_u32(ss, ii);
				//uint32_t mask = vmovemaskq_u32(temp);// 用不了
				unsigned int result[4] = { 0 };
				vst1q_u32(result, temp);
				if (result[0] == 1 || result[1] == 1 || result[2] == 1 || result[3] == 1) {
					isFind = true;
					break;
				}
				else if (e < index[list[s].key].docIdList[list[s].cursor])// 及时break，避免重置cursor
					break;
				list[s].cursor += 4;
			}
			s++;// 下一个链表
		}
		list[0].cursor++;// 当前元素已被访问过
		if (s == num && isFind) {
			S.docIdList.push_back(e);
			S.length++;
		}
		//sort(list, list + num);// 重排，将未探查元素少的列表前移
	}
	return S;
}

InvertedIndex HASH_NEON(int* queryList, vector<InvertedIndex>& index, int num) {
	InvertedIndex s = index[queryList[0]];// 选出最短的倒排列表
	for (int i = 1; i < num; i++) {// 与剩余列表求交
		// SVS的思想，将s挨个按表求交
		int count = 0;
		int length = s.length;

		for (int j = 0; j < length; j++) {
			bool isFind = false;
			int hashValue = s.docIdList[j] / 512;
			// 找到该hash值在当前待求交列表中对应的段
			int begin = hashBucket[queryList[i]][hashValue].begin;
			int end = hashBucket[queryList[i]][hashValue].end;
			if (begin < 0 || end < 0) {// 该值肯定找不到，不用往后做了
				continue;
			}
			else {// 在该段里进行查找，一次查4个
				uint32x4_t ss = vmovq_n_u32(s.docIdList[j]);// 填充四个位置

				for (begin; begin <= end - 3; begin += 4) {
					uint32x4_t ii = vld1q_u32(&index[i].docIdList[begin]);// 读取四个数据

					uint32x4_t temp = vceqq_u32(ss, ii);
					unsigned int result[4] = { 0 };
					vst1q_u32(result, temp);
					if (result[0] == 1 || result[1] == 1 || result[2] == 1 || result[3] == 1) {
						isFind = true;
						break;
					}
					else if (s.docIdList[j] < index[queryList[i]].docIdList[begin])// 及时break，避免超过，下一元素需重头再来
						break;
				}
				if (!isFind && (begin > end - 3)) {// 处理剩余元素
					for (; begin <= end; begin++)
					{
						if (s.docIdList[j] == index[queryList[i]].docIdList[begin]) {
							isFind = true;
							break;
						}
						else if (s.docIdList[j] < index[queryList[i]].docIdList[begin])// 升序排列
							break;
					}
				}
				if (isFind) {
					// 覆盖
					s.docIdList[count++] = s.docIdList[j];
				}
			}
		}
		if (count < length)// 最后才做删除
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
		sort(t.docIdList.begin(), t.docIdList.end());//对文档编号排序
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
		InvertedIndex t;// 生成500个倒排列表
		t.length = rand() % (100 - 30 + 1) + 30;
		//cout << t.length << " :";

		vector<int> forRandom;
		for (int j = 0; j < t.length * 5; j++)//为了不重复
			forRandom.push_back(j);
		random_shuffle(forRandom.begin(), forRandom.end());//随机打乱

		for (int j = 0; j < t.length; j++)
		{
			int docId = forRandom[j];
			//cout << docId << " ";
			t.docIdList.push_back(docId);
		}
		sort(t.docIdList.begin(), t.docIdList.end());//对文档编号排序
		testInvertedLists.push_back(t);
		//cout << endl;
	}

	timeval start, finish;
	gettimeofday(&start, NULL);// Start Time

	for (int i = 0; i < 200; i++) {// 做200次查询
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
	timeval start, finish;
	gettimeofday(&start, NULL);// Start Time

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