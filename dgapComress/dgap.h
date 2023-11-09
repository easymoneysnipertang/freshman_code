#pragma once
#ifndef DGAP_H
#define  DGAP_H

#include<vector>
using namespace std;

// invertedIndex: �ź���ĵ�������
// result: ѹ���������
// return: ѹ�����С��bit��
long long dgapCompress(const vector<unsigned>& invertedIndex, vector<unsigned>& result, int& idx, int usedBit);

// invertedIndex: dgapѹ���õĵ�������
// result�������������
// return����ѹ���С��bit)
int dgapDecompress(const vector<unsigned>& compressedLists, vector<unsigned>& result, int& idx);

void dgapDecompressAll(const vector<unsigned>& compressedLists, vector<vector<unsigned>>& result);

// ʵ��һ�ζ�ȡ�������
int dgapDecompress(const vector<unsigned>& compressedLists, vector<unsigned>& result, int& idx);

#endif