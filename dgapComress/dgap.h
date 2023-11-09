#pragma once
#ifndef DGAP_H
#define  DGAP_H

#include<vector>
using namespace std;

// invertedIndex: 排好序的倒排索引
// result: 压缩后的索引
// return: 压缩后大小（bit）
long long dgapCompress(const vector<unsigned>& invertedIndex, vector<unsigned>& result, int& idx, int usedBit);

// invertedIndex: dgap压缩好的倒排索引
// result：解缩后的索引
// return：解压后大小（bit)
int dgapDecompress(const vector<unsigned>& compressedLists, vector<unsigned>& result, int& idx);

void dgapDecompressAll(const vector<unsigned>& compressedLists, vector<vector<unsigned>>& result);

// 实现一次读取多个链表
int dgapDecompress(const vector<unsigned>& compressedLists, vector<unsigned>& result, int& idx);

#endif