#pragma once
#ifndef BITOP_H
#define  BITOP_H

#include<vector>
#include<string>
using namespace std;

//return: a的bit多少
int countBit(const vector<unsigned>& a);

// 将n位数据写入到unsigned int型数组中
void writeBitData(vector<unsigned int>& array, unsigned int index, unsigned int data, int n);

// 从unsigned int型数组中读取n位数据
unsigned int readBitData(const vector<unsigned int>& array, unsigned int index, int n);

//读取倒排链表
void getIndex(vector<vector<unsigned>>& invertedLists);
void getIndexOPT(vector<vector<unsigned>>& invertedLists);


void vectorToBin(const vector<unsigned int>& data, const string& filename);

void binToVector(const string& filename, vector<unsigned int>& data);
#endif