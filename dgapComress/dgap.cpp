#include "dgap.h"
#include "bitOp.h"
#include<omp.h>
#include<vector>
#include<math.h>
#include<iostream>
#include <emmintrin.h>

using namespace std;

#define NUM_THREADS 4

// invertedIndex: �ź���ĵ�������
// result: ѹ���������
// idx��ָ�����һ��bit��ָ��,���Զ�����
// usedBit��result��ʵ��ʹ�õ�bit�����Ա����˷�
// return: ѹ�����С��bit��
long long dgapCompress(const vector<unsigned>& invertedIndex, vector<unsigned>& result, int& idx, int usedBit)
{
	unsigned indexLen = invertedIndex.size();
	if (indexLen == 0)
		return -1;

	vector<unsigned> deltaId;
	deltaId.push_back(invertedIndex[0]);
	unsigned maxDelta = invertedIndex[0];//�����

	// d-gapǰһ���ȥ��һ��,ͬʱ�������
	for (int i = 1; i < indexLen; i++)
	{
		unsigned delta = invertedIndex[i] - invertedIndex[i - 1];
		deltaId.push_back(delta);
		if (delta > maxDelta)
			maxDelta = delta;
	}

	unsigned maxBitNum = ceil(log2(maxDelta + 1));//�������ʹ��maxBitNumλ�洢

	long long bitCnt = usedBit + 32 + 6 + maxBitNum * indexLen;//�ܹ�ʹ��bit�� = ��ʹ�õ�bit��+�������Ҫʹ�õ�bit��
	result.resize((int)ceil(bitCnt / 32.0));

	writeBitData(result, idx, indexLen, 32);//д��Ԫ�ظ���
	writeBitData(result, idx + 32, maxBitNum, 6);//д��deltaԪ�س���

	idx += 38;//��index+38λ��ʼдѹ�����delta
	for (int i = 0; i < indexLen; i++)
	{
		writeBitData(result, idx, deltaId[i], maxBitNum);
		idx += maxBitNum;
	}
	return bitCnt;
}

// invertedIndex: dgapѹ���õĵ�������
// result�������������
// idx���ӵ�idx��bit��ʼ��ѹ
// return����ѹ���С��bit)
vector<unsigned> dgapDecompress(const vector<unsigned>& compressedLists,  int& idx)
{
	vector<unsigned> result;
	//ǰ32λ�ǳ���
	unsigned len = readBitData(compressedLists, idx, 32);
	result.reserve(len);// ��ͬ����棬resize
	//printf("%d\n", len);
	if (len == 0)
		return result;

	//6λ���õ�Bit��
	int bitNum = (int)readBitData(compressedLists, idx + 32, 6);
	idx += 38;

	unsigned delta = readBitData(compressedLists, idx, bitNum);
	idx += bitNum;
	result.push_back(delta);//��һ��deltaֱ�ӽ�ȥ
	for (unsigned i = 1; i < len; i++)
	{
		delta = readBitData(compressedLists, idx, bitNum);
		idx += bitNum;
		result.push_back(result[i - 1] + delta);//�����Ķ�Ҫ����ǰһ���Ž�ȥ
	}
	return result;
}

vector<unsigned> dgapDecompressOMP(const vector<unsigned>& compressedLists, int& idx)
{
	//ǰ32λ�ǳ���
	unsigned len = readBitData(compressedLists, idx, 32);
	vector<unsigned> result;
	result.resize(len);
	//printf("%d\n", len);
	if (len == 0)
		return result;
	//6λ���õ�Bit��
	int bitNum = (int)readBitData(compressedLists, idx + 32, 6);
	idx += 38;

	int seq_num = len / NUM_THREADS;// ÿ���̴߳���Ԫ����Ŀ
	if (seq_num == 0) {// ���������ÿ��0��
		unsigned delta = readBitData(compressedLists, idx, bitNum);
		idx += bitNum;
		result[0] = delta;//��һ��deltaֱ�ӽ�ȥ
		for (int i = 1; i < len; i++){
			delta = readBitData(compressedLists, idx, bitNum);
			idx += bitNum;
			result[i] = result[i - 1] + delta;//�����Ķ�Ҫ����ǰһ���Ž�ȥ
		}
		return result;
	}

#pragma omp parallel num_threads(NUM_THREADS)
	{
		int tid = omp_get_thread_num();// ��ǰ�߳�tid
		int localIdx = idx + bitNum * tid * seq_num;
		unsigned delta = readBitData(compressedLists, localIdx, bitNum);// �ҳ��öε�һ��Ԫ��
		localIdx += bitNum;// ����һ��delta
		result[tid * seq_num] = delta;
		for (int j = 0; j < seq_num - 1; j++) {// �߳��ڽ���ǰ׺��
			delta = readBitData(compressedLists, localIdx, bitNum);
			localIdx += bitNum;
			result[tid * seq_num + j + 1] = delta + result[tid * seq_num + j];
		}
#pragma omp barrier
	}
#pragma omp single
	// ����߽�λ�ã�������沢��ʹ��
	for (int i = 2; i <= NUM_THREADS; i++)// ��������ʽ·�ϣ�
		result[i * seq_num - 1] += result[(i - 1) * seq_num - 1];

#pragma omp parallel num_threads(NUM_THREADS)
	{
		int tid = omp_get_thread_num();
		if (tid != 0) {// 0���̲߳�����
			int j = 0;
			// ʹ��simd����һ�μ�4��Ԫ�أ�ע�����²��ּ��ص�����ִ��
			__m128i scalarValue = _mm_set1_epi32(result[tid * seq_num - 1]);
			for (; j < seq_num - 1 - 3; j += 4) {// ʹ��simd
				__m128i vector = _mm_loadu_si128((__m128i*)&result[tid * seq_num + j]);
				__m128i sum = _mm_add_epi32(scalarValue, vector);
				_mm_storeu_si128((__m128i*)&result[tid * seq_num + j], sum);
			}
			for (; j < seq_num - 1; j++)// ����ʣ��Ԫ��
				result[tid * seq_num + j] += result[tid * seq_num - 1];
		}
	}
	idx += bitNum * NUM_THREADS * seq_num;// ����idxָ��
	// ���д���ʣ��Ԫ��
#pragma single
	if (len % NUM_THREADS != 0) {
		for (int i = NUM_THREADS * seq_num; i < len; i++) {
			unsigned delta= readBitData(compressedLists, idx, bitNum);
			idx += bitNum;
			result[i] = result[i - 1]+delta;
		}
	}
	return result;
}


void dgapDecompressAll(const vector<unsigned>& compressedLists, vector<vector<unsigned>>& result)
{
	vector<unsigned> curList;
	int idx = 0;

	for (int i = 0; i < 2000; i++) //��ѹ2000������,�������decompressed
	{
		curList = dgapDecompressOMP(compressedLists, idx);

		//��֤��ȷ����
		/*if (curList1.size() != curList2.size())
		{
			printf("wrong length\n");
			cout << curList1.size() << " " << curList2.size() << endl;
		}
		else {
			cout << curList1.size() << endl;
			for (int j = 0; j < curList1.size(); j++)
				if (curList1[j] != curList2[j])
					cout << "wrong" << endl;
		}*/
		
		result.push_back(curList);
		//curList.clear();
	}
}
