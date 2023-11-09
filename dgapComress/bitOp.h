#pragma once
#ifndef BITOP_H
#define  BITOP_H

#include<vector>
#include<string>
using namespace std;

//return: a��bit����
int countBit(const vector<unsigned>& a);

// ��nλ����д�뵽unsigned int��������
void writeBitData(vector<unsigned int>& array, unsigned int index, unsigned int data, int n);

// ��unsigned int�������ж�ȡnλ����
unsigned int readBitData(const vector<unsigned int>& array, unsigned int index, int n);

//��ȡ��������
void getIndex(vector<vector<unsigned>>& invertedLists);
void getIndexOPT(vector<vector<unsigned>>& invertedLists);


void vectorToBin(const vector<unsigned int>& data, const string& filename);

void binToVector(const string& filename, vector<unsigned int>& data);
#endif