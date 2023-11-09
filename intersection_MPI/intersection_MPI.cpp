#include <stdio.h>
#include <mpi.h>
#include<fstream>
#include<string>
#include<vector>
#include<sstream>
#include<Windows.h>
#include<algorithm>
#include<omp.h>
#include <xmmintrin.h>  //sse
#include <emmintrin.h>  //sse2
#include <pmmintrin.h>  //sse3
#include <immintrin.h>

using namespace std;

#define NUM_THREADS 4

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

// 不提前预取，用0标志，最后取出来通信
InvertedIndex SVS_MPI1(int* queryList, vector<InvertedIndex>& index, int num,int rank,int size){
	InvertedIndex s = index[queryList[0]];// 取最短的列表
	int initialLength = s.length;

	// 获取工作范围
	int begin = rank * (initialLength / size),
		end = rank != size - 1 ? (initialLength / size) * (rank + 1) : initialLength;

	// 与剩余列表求交
	for (int i = 1; i < num; i++) {
		int t = 0;

		// 预处理加速大概率事件
		t = rank * (index[queryList[i]].length / size);
		for (int j = 2*rank; j > 0; j--) {
			if (s.docIdList[begin] < index[queryList[i]].docIdList[t])
				t -= (index[queryList[i]].length / size) / 2;
			else break;
		}

		// s列表中的每个元素都拿出来比较
		for (int j = begin; j < end; j++) {// 所有元素都得访问一遍
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
			if (!isFind)// 没找到合适的，该位置置为0
			{
				s.docIdList[j] = 26000000;
			}
		}
	}

	InvertedIndex result;
	// 用0号进程做收集
	if (rank == 0) {
		// 把该部分求交有效的拿出来->为了通信
		for (int i = begin; i < end; i++) {
			if (s.docIdList[i] != 26000000) {
				result.docIdList.push_back(s.docIdList[i]);
				result.length++;
			}
		}

		for (int i = 1; i < size; i++) {
			int count;
			MPI_Recv(&count, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			unsigned int* temp;// 转成int数组发送
			temp = new unsigned int[count];

			MPI_Recv(temp, count, MPI_UNSIGNED, i, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			for (int j = 0; j < count; j++) {
				result.docIdList.push_back(temp[j]);
			}
			delete []temp;
		}
		result.length = result.docIdList.size();
	}
	// 非0号线程则把vector发送过去
	else {
		unsigned int* temp;// 转成int数组发送
		temp = new unsigned[end-begin];
		int count = 0;
		for (int i = begin; i < end; i++) {
			if (s.docIdList[i] != 26000000) {
				temp[count++] = s.docIdList[i];
			}
		}
		MPI_Send(&count, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);// 先发一个长度过去
		MPI_Send(temp, count, MPI_UNSIGNED, 0, 1, MPI_COMM_WORLD);// 再把数据发过去
		delete []temp;
	}

	return s;
}

// 提前预取，保留原始svs算法
InvertedIndex SVS_MPI2(int* queryList, vector<InvertedIndex>& index, int num, int rank, int size) {
	// 取最短的列表
	int initialLength = index[queryList[0]].length;
	InvertedIndex s;// 取工作范围1/4
	int start = (initialLength / size) * rank,
		end = rank != size - 1 ? (initialLength / size) * (rank + 1) : initialLength;// 末尾特殊处理
	s.docIdList.assign(index[queryList[0]].docIdList.begin() + start, index[queryList[0]].docIdList.begin() + end);
	s.length = s.docIdList.size();

	// 与剩余列表求交
	for (int i = 1; i < num; i++) {
		int count = 0;// s从头往后遍历一遍
		int t = 0;

		if (s.length == 0)break;
		// 预处理加速大概率事件
		t = rank * (index[queryList[i]].length / size);
		for (int j = 2 * rank; j > 0; j--) {
			if (s.docIdList[0] < index[queryList[i]].docIdList[t])
				t -= (index[queryList[i]].length / size) / 2;
			else break;
		}

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

	// 进程通信
	// 用0号进程做收集
	//if (rank == 0) {
	//	for (int i = 1; i < size; i++) {
	//		int count;
	//		MPI_Recv(&count, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	//		unsigned int* temp;// 转成int数组发送
	//		temp = new unsigned int[count];
	//		// 接受数据
	//		MPI_Recv(temp, count, MPI_UNSIGNED, i, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	//		s.docIdList.insert(s.docIdList.end(), temp, temp + count);
	//		delete []temp;
	//	}
	//	s.length = s.docIdList.size();
	//}
	//// 非0号线程则把vector发送过去
	//else {
	//	unsigned int* temp;// 转成int数组发送
	//	temp = new unsigned[s.length];
	//	copy(s.docIdList.begin(), s.docIdList.end(), temp);
	//	MPI_Send(&s.length, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);// 先发一个长度过去
	//	MPI_Send(temp, s.length, MPI_UNSIGNED, 0, 1, MPI_COMM_WORLD);// 再把数据发过去
	//	delete []temp;
	//}

	// 修改通信机制，将长度放入数组0号位置
	// 用0号进程做收集
	MPI_Barrier(MPI_COMM_WORLD);// 保证在同一个函数步内
	if (rank == 0) {
		unsigned int* temp;// 转成int数组发送
		//int maxLen = max(initialLength / size, initialLength - initialLength / size * (size - 1));
		temp = new unsigned int[initialLength];// 节省空间实测比开个大的还慢？
		for (int i = 1; i < size; i++) {
			// 接受数据
			MPI_Recv(temp, initialLength, MPI_UNSIGNED, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			int len = temp[0];
			s.docIdList.insert(s.docIdList.end(), temp+1, temp +1+len);
		}
		delete[]temp;
		s.length = s.docIdList.size();
	}
	// 非0号线程则把vector发送过去
	else {
		unsigned int* temp;// 转成int数组发送
		temp = new unsigned[s.length+1];
		temp[0] = s.length;
		copy(s.docIdList.begin(), s.docIdList.end(), temp+1);
		// 直接一起发过去
		MPI_Send(temp, s.length+1, MPI_UNSIGNED, 0, 1, MPI_COMM_WORLD);// 再把数据发过去
		delete []temp;
	}

	return s;
}


// 加强加速大概率事件，提前确定每个进程工作范围
class QueryItem {
public:
	int cursor;// 当前读到哪了
	int end;// 结束的下标
	int key;// 关键字值
};
//返回第一个大于等于目标元素的下标
int find1stGreaterEqual(const std::vector<unsigned int>& arr, unsigned int target) {
	int left = 0;
	int right = arr.size();
	while (left < right)
	{
		int mid = left + (right - left) / 2;
		if (arr[mid] < target)
		{
			left = mid + 1;
		}
		else
			right = mid;
	}
	return left;
}
InvertedIndex SVS_MPI_Precise(int* queryList, vector<InvertedIndex>& index, int num, int rank, int size) {
	//每个进程的最短倒排链表的开始位置和结束位置
	int initialLength = index[queryList[0]].docIdList.size();
	InvertedIndex s;// 取工作范围1/4
	int start = (initialLength / size) * rank,
		end = rank != size - 1 ? (initialLength / size) * (rank + 1) : initialLength;// 末尾特殊处理
	s.docIdList.assign(index[queryList[0]].docIdList.begin() + start, index[queryList[0]].docIdList.begin() + end);
	s.length = s.docIdList.size();

	// 构造数据结构保存其余链表求交范围
	QueryItem* list = new QueryItem[num]();
	for (int i = 0; i < num; i++)// 预处理,关键字赋值
		list[i].key = queryList[i];
	//找到剩余倒排链表的起始位置和结束位置
	int* from = new int[num - 1];
	int* to = new int[num - 1];
	for (int i = 1; i < num; i++) {
		//找到自己的开始位置
		from[i - 1] = find1stGreaterEqual(index[list[i].key].docIdList, index[list[0].key].docIdList[start]);
		list[i].cursor = from[i - 1];

	}
	if (rank != size - 1) {
		//最后一个进程不需要知道自己何时结束，因为它直接扫描到最后
		MPI_Recv(to, num - 1, MPI_INT, rank + 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);//从下一个进程接受该进程的起始位置
		for (int i = 1; i < num; i++) {
			list[i].end = to[i - 1];//下一个进程的开始正是这个进程的结束位置
		}
	}
	else {
		for (int i = 1; i < num; i++) {
			//最后一个进程做到底
			list[i].end = index[list[i].key].docIdList.size();
		}
	}
	if (rank != 0) {
		//第一个进程不用发送信息，其余发送给前一个进程
		MPI_Send(from, num - 1, MPI_INT, rank - 1, 1, MPI_COMM_WORLD);
	}


	// 与剩余列表求交
	for (int i = 1; i < num; i++) {
		int count = 0;// s从头往后遍历一遍

		// s列表中的每个元素都拿出来比较
		for (int j = 0; j < s.length; j++) {// 所有元素都得访问一遍
			bool isFind = false;// 标志，判断当前count位是否能求交

			for (; list[i].cursor < list[i].end; list[i].cursor++) {
				// 遍历i列表中所有元素
				if (s.docIdList[j] == index[queryList[i]].docIdList[list[i].cursor]) {
					isFind = true;
					break;
				}
				else if (s.docIdList[j] < index[queryList[i]].docIdList[list[i].cursor])// 升序排列
					break;
			}
			if (isFind)// 覆盖
				s.docIdList[count++] = s.docIdList[j];
		}
		if (count < s.length)// 最后才做删除
			s.docIdList.erase(s.docIdList.begin() + count, s.docIdList.end());
		s.length = count;
	}

	// 进程通信
	// 用0号进程做收集
	if (rank == 0) {
		for (int i = 1; i < size; i++) {
			int count;
			MPI_Recv(&count, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			unsigned int* temp;// 转成int数组发送
			temp = new unsigned int[count];
			// 接受数据
			MPI_Recv(temp, count, MPI_UNSIGNED, i, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			s.docIdList.insert(s.docIdList.end(), temp, temp + count);
			delete[]temp;
		}
		s.length = s.docIdList.size();
	}
	// 非0号线程则把vector发送过去
	else {
		unsigned int* temp;// 转成int数组发送
		temp = new unsigned[s.length];
		copy(s.docIdList.begin(), s.docIdList.end(), temp);
		MPI_Send(&s.length, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);// 先发一个长度过去
		MPI_Send(temp, s.length, MPI_UNSIGNED, 0, 1, MPI_COMM_WORLD);// 再把数据发过去
		delete[]temp;
	}

	return s;
}

InvertedIndex SVS_MPI_PreCom(int* queryList, vector<InvertedIndex>& index, int num, int rank, int size) {
	//每个进程的最短倒排链表的开始位置和结束位置
	int initialLength = index[queryList[0]].docIdList.size();
	InvertedIndex s;// 取工作范围1/4
	int start = (initialLength / size) * rank,
		end = rank != size - 1 ? (initialLength / size) * (rank + 1) : initialLength;// 末尾特殊处理
	s.docIdList.assign(index[queryList[0]].docIdList.begin() + start, index[queryList[0]].docIdList.begin() + end);
	s.length = s.docIdList.size();

	// 构造数据结构保存其余链表求交范围
	QueryItem* list = new QueryItem[num]();
	for (int i = 0; i < num; i++)// 预处理,关键字赋值
		list[i].key = queryList[i];
	//找到剩余倒排链表的起始位置和结束位置
	int* from = new int[num - 1];
	int* to = new int[num - 1];
	for (int i = 1; i < num; i++) {
		//找到自己的开始位置
		from[i - 1] = find1stGreaterEqual(index[list[i].key].docIdList, index[list[0].key].docIdList[start]);
		list[i].cursor = from[i - 1];

	}
	if (rank != size - 1) {
		//最后一个进程不需要知道自己何时结束，因为它直接扫描到最后
		MPI_Recv(to, num - 1, MPI_INT, rank + 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);//从下一个进程接受该进程的起始位置
		for (int i = 1; i < num; i++) {
			list[i].end = to[i - 1];//下一个进程的开始正是这个进程的结束位置
		}
	}
	else {
		for (int i = 1; i < num; i++) {
			//最后一个进程做到底
			list[i].end = index[list[i].key].docIdList.size();
		}
	}
	if (rank != 0) {
		//第一个进程不用发送信息，其余发送给前一个进程
		MPI_Send(from, num - 1, MPI_INT, rank - 1, 1, MPI_COMM_WORLD);
	}


	// 与剩余列表求交
	for (int i = 1; i < num; i++) {
		int count = 0;// s从头往后遍历一遍

		// s列表中的每个元素都拿出来比较
		for (int j = 0; j < s.length; j++) {// 所有元素都得访问一遍
			bool isFind = false;// 标志，判断当前count位是否能求交

			for (; list[i].cursor < list[i].end; list[i].cursor++) {
				// 遍历i列表中所有元素
				if (s.docIdList[j] == index[queryList[i]].docIdList[list[i].cursor]) {
					isFind = true;
					break;
				}
				else if (s.docIdList[j] < index[queryList[i]].docIdList[list[i].cursor])// 升序排列
					break;
			}
			if (isFind)// 覆盖
				s.docIdList[count++] = s.docIdList[j];
		}
		if (count < s.length)// 最后才做删除
			s.docIdList.erase(s.docIdList.begin() + count, s.docIdList.end());
		s.length = count;
	}

	// 进程通信
	// 用0号进程做收集
	if (rank == 0) {
		int* recvCounts = new int[size]; // 存储每个进程发送的数据数量
		int totalCount = 0; // 所有进程发送的总数据数量

		int count = s.length;
		// 接收每个进程发送的数据数量
		MPI_Gather(&count, 1, MPI_INT, recvCounts, 1, MPI_INT, 0, MPI_COMM_WORLD);

		// 计算总数据数量
		for (int i = 0; i < size; i++)
			totalCount += recvCounts[i];

		unsigned int* temp = new unsigned int[totalCount]; // 存储接收到的数据
		int* displacements = new int [size] {0}; // 接收缓冲区中每个进程数据的位移量

		// 计算位移量
		for (int i = 1; i < size; i++)
			displacements[i] = displacements[i - 1] + recvCounts[i - 1];

		// 接收每个进程发送的数据
		MPI_Gatherv(NULL, 0, MPI_UNSIGNED, temp, recvCounts, displacements, MPI_UNSIGNED, 0, MPI_COMM_WORLD);
		s.docIdList.insert(s.docIdList.end(), temp, temp + totalCount);
	}
	else {
		unsigned int* temp;// 转成int数组发送
		temp = new unsigned[s.length];
		copy(s.docIdList.begin(), s.docIdList.end(), temp);
		// 发送数据数量给进程0
		MPI_Gather(&s.length, 1, MPI_INT, NULL, 0, MPI_INT, 0, MPI_COMM_WORLD);

		// 发送数据给进程0
		MPI_Gatherv(temp, s.length, MPI_UNSIGNED, NULL, NULL, NULL, MPI_UNSIGNED, 0, MPI_COMM_WORLD);
		delete[]temp;
	}

	return s;
}

InvertedIndex SVS_MPI_Precise_SSE(int* queryList, vector<InvertedIndex>& index, int num, int rank, int size) {
	//每个进程的最短倒排链表的开始位置和结束位置
	int initialLength = index[queryList[0]].docIdList.size();
	InvertedIndex s;// 取工作范围1/4
	int start = (initialLength / size) * rank,
		end = rank != size - 1 ? (initialLength / size) * (rank + 1) : initialLength;// 末尾特殊处理
	s.docIdList.assign(index[queryList[0]].docIdList.begin() + start, index[queryList[0]].docIdList.begin() + end);
	s.length = s.docIdList.size();

	// 构造数据结构保存其余链表求交范围
	QueryItem* list = new QueryItem[num]();
	for (int i = 0; i < num; i++)// 预处理,关键字赋值
		list[i].key = queryList[i];
	//找到剩余倒排链表的起始位置和结束位置
	int* from = new int[num - 1];
	int* to = new int[num - 1];
	for (int i = 1; i < num; i++) {
		//找到自己的开始位置
		from[i - 1] = find1stGreaterEqual(index[list[i].key].docIdList, index[list[0].key].docIdList[start]);
		list[i].cursor = from[i - 1];

	}
	if (rank != size - 1) {
		//最后一个进程不需要知道自己何时结束，因为它直接扫描到最后
		MPI_Recv(to, num - 1, MPI_INT, rank + 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);//从下一个进程接受该进程的起始位置
		for (int i = 1; i < num; i++) {
			list[i].end = to[i - 1];//下一个进程的开始正是这个进程的结束位置
		}
	}
	else {
		for (int i = 1; i < num; i++) {
			//最后一个进程做到底
			list[i].end = index[list[i].key].docIdList.size();
		}
	}
	if (rank != 0) {
		//第一个进程不用发送信息，其余发送给前一个进程
		MPI_Send(from, num - 1, MPI_INT, rank - 1, 1, MPI_COMM_WORLD);
	}


	// 与剩余列表求交
	for (int i = 1; i < num; i++) {
		int count = 0;// s从头往后遍历一遍

		// s列表中的每个元素都拿出来比较
		for (int j = 0; j < s.length; j++) {// 所有元素都得访问一遍
			bool isFind = false;// 标志，判断当前count位是否能求交

			__m128i ss = _mm_set1_epi32(s.docIdList[j]);// 填充32位

			for (; list[i].cursor < list[i].end - 3; list[i].cursor += 4) {// 同时与四个元素比较
				// 遍历i列表中所有元素
				__m128i ii;
				ii = _mm_load_si128((__m128i*) & index[queryList[i]].docIdList[list[i].cursor]);// 一次取四个
				__m128i tmp = _mm_set1_epi32(0);
				tmp = _mm_cmpeq_epi32(ss, ii);// 比较向量每一位
				int mask = _mm_movemask_epi8(tmp);// 转为掩码，如果有一个数相等,就不会是全0

				if (mask != 0) {// 查看比较结果
					isFind = true;
					break;
				}
				else if (s.docIdList[j] < index[queryList[i]].docIdList[list[i].cursor])// 及时break，避免超过，下一元素需重头再来
					break;
			}
			if (!isFind && (list[i].cursor >= list[i].end - 3)) {// 处理剩余元素
				for (; list[i].cursor < list[i].end; list[i].cursor++)
				{
					if (s.docIdList[j] == index[queryList[i]].docIdList[list[i].cursor]) {
						isFind = true;
						break;
					}
					else if (s.docIdList[j] < index[queryList[i]].docIdList[list[i].cursor])// 升序排列
						break;
				}
			}
			if (isFind)// 覆盖
				s.docIdList[count++] = s.docIdList[j];
		}
		if (count < s.length)// 最后才做删除
			s.docIdList.erase(s.docIdList.begin() + count, s.docIdList.end());
		s.length = count;
	}

	// 进程通信
	// 用0号进程做收集
	if (rank == 0) {
		for (int i = 1; i < size; i++) {
			int count;
			MPI_Recv(&count, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			unsigned int* temp;// 转成int数组发送
			temp = new unsigned int[count];
			// 接受数据
			MPI_Recv(temp, count, MPI_UNSIGNED, i, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			s.docIdList.insert(s.docIdList.end(), temp, temp + count);
			delete[]temp;
		}
		s.length = s.docIdList.size();
	}
	// 非0号线程则把vector发送过去
	else {
		unsigned int* temp;// 转成int数组发送
		temp = new unsigned[s.length];
		copy(s.docIdList.begin(), s.docIdList.end(), temp);
		MPI_Send(&s.length, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);// 先发一个长度过去
		MPI_Send(temp, s.length, MPI_UNSIGNED, 0, 1, MPI_COMM_WORLD);// 再把数据发过去
		delete[]temp;
	}

	return s;
}

// query内mpi+query内omp
// 差点，mpi已经将链表切成1/4了，再omp切成了1/16，越细越负载不均
InvertedIndex SVS_MPI_OMP(int* queryList, vector<InvertedIndex>& index, int num, int rank, int size) {
	// 取最短的列表
	int initialLength = index[queryList[0]].length;
	InvertedIndex s;// 取工作范围1/4
	int start = (initialLength / size) * rank,
		end = rank != size - 1 ? (initialLength / size) * (rank + 1) : initialLength;// 末尾特殊处理
	s.docIdList.assign(index[queryList[0]].docIdList.begin() + start, index[queryList[0]].docIdList.begin() + end);
	s.length = s.docIdList.size();

	int count = 0;
	// 与剩余列表求交
#pragma omp parallel num_threads(NUM_THREADS),shared(count)
	for (int i = 1; i < num; i++) {
		count = 0;// s从头往后遍历一遍
		int t = 0;

		if (s.length == 0)break;
		// 预处理加速大概率事件
		t = rank * (index[queryList[i]].length / size);
		for (int j = 2 * rank; j > 0; j--) {
			if (s.docIdList[0] < index[queryList[i]].docIdList[t])
				t -= (index[queryList[i]].length / size) / 2;
			else break;
		}

		// s列表中的每个元素都拿出来比较
#pragma omp for
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
#pragma omp critical
			if (isFind)// 覆盖
				s.docIdList[count++] = s.docIdList[j];
		}
#pragma omp single
		if (count < s.length)// 最后才做删除
		{
			s.docIdList.erase(s.docIdList.begin() + count, s.docIdList.end());
			s.length = count;
		}
	}

	// 修改通信机制，将长度放入数组0号位置
	// 用0号进程做收集
	if (rank == 0) {
		unsigned int* temp;// 转成int数组发送
		//int maxLen = max(initialLength / size, initialLength - initialLength / size * (size - 1));
		temp = new unsigned int[initialLength];// 节省空间实测比开个大的还慢？
		for (int i = 1; i < size; i++) {
			// 接受数据
			MPI_Recv(temp, initialLength, MPI_UNSIGNED, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			int len = temp[0];
			s.docIdList.insert(s.docIdList.end(), temp + 1, temp + 1 + len);
		}
		delete[]temp;
		s.length = s.docIdList.size();
	}
	// 非0号线程则把vector发送过去
	else {
		unsigned int* temp;// 转成int数组发送
		temp = new unsigned[s.length + 1];
		temp[0] = s.length;
		copy(s.docIdList.begin(), s.docIdList.end(), temp + 1);
		// 直接一起发过去
		MPI_Send(temp, s.length + 1, MPI_UNSIGNED, 0, 1, MPI_COMM_WORLD);// 再把数据发过去
		delete[]temp;
	}

	return s;
}

// query间mpi+query内omp
InvertedIndex SVS_omp(int* queryList, vector<InvertedIndex>& index, int num)
{
	InvertedIndex s = index[queryList[0]];// 取最短的列表
	int count = 0;
	// 与剩余列表求交
#pragma omp parallel num_threads(NUM_THREADS),shared(count)
	for (int i = 1; i < num; i++) {
		count = 0;// s从头往后遍历一遍
		int t = 0;
		// s列表中的每个元素都拿出来比较
#pragma omp for
		for (int j = 0; j < s.docIdList.size(); j++) {// 所有元素都得访问一遍
			bool isFind = false;// 标志，判断当前count位是否能求交
			
			for (; t < index[queryList[i]].docIdList.size(); t++) {
				// 遍历i列表中所有元素
				if (s.docIdList[j] == index[queryList[i]].docIdList[t]) {
					isFind = true;
					break;
				}
				else if (s.docIdList[j] < index[queryList[i]].docIdList[t])// 升序排列
					break;
			}
#pragma omp critical
			if (isFind)
				s.docIdList[count++] = s.docIdList[j];
		}
#pragma omp single
		if (count < s.docIdList.size())
			s.docIdList.erase(s.docIdList.begin() + count, s.docIdList.end());
	}
	return s;
}


// hash分段实现
class HashBucket {// 一个hash段，记录他在倒排列表中的起始位置
public:
	int begin = -1;
	int end = -1;
};
HashBucket** hashBucket;
// 预处理，将倒排列表放入hash段里
void preprocessing(vector<InvertedIndex>& index, int count) {
	hashBucket = new HashBucket * [count];
	for (int i = 0; i < count; i++)
		hashBucket[i] = new HashBucket[50800];// 26000000/512

	for (int i = 0; i < count; i++) {
		for (int j = 0; j < index[i].length; j++) {
			int value = index[i].docIdList[j];// 拿出当前列表第j个值
			int hashValue = value / 512;// TODO:check
			if (hashBucket[i][hashValue].begin == -1) // 该段内还没有元素
				hashBucket[i][hashValue].begin = j;
			hashBucket[i][hashValue].end = j;// 添加到该段尾部
		}
	}
}
InvertedIndex HASH(int* queryList, vector<InvertedIndex>& index, int num) {
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
			else {
				for (begin; begin <= end; begin++) {
					if (s.docIdList[j] == index[queryList[i]].docIdList[begin]) {
						// 在该段中找到了当前值
						isFind = true;
						break;
					}
					else if (s.docIdList[j] < index[queryList[i]].docIdList[begin]) {
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
InvertedIndex HASH_MPI(int* queryList, vector<InvertedIndex>& index, int num,int rank,int size) {
	// 取最短的列表
	int initialLength = index[queryList[0]].length;
	InvertedIndex s;// 取工作范围1/4
	int start = (initialLength / size) * rank,
		end = rank != size - 1 ? (initialLength / size) * (rank + 1) : initialLength;// 末尾特殊处理
	s.docIdList.assign(index[queryList[0]].docIdList.begin() + start, index[queryList[0]].docIdList.begin() + end);
	s.length = s.docIdList.size();

	for (int i = 1; i < num; i++) {// 与剩余列表求交
		// SVS的思想，将s挨个按表求交
		int count = 0;
		int length = s.length;

		for (int j = 0; j < length; j++) {
			bool isFind = false;
			int hashValue = s.docIdList[j] / 512;
			// 找到该hash值在当前待求交列表中对应的段
			int begin = hashBucket[queryList[i]][hashValue].begin;
			int end1 = hashBucket[queryList[i]][hashValue].end;
			if (begin < 0 || end1 < 0) {// 该值肯定找不到，不用往后做了
				continue;
			}
			else {
				for (begin; begin <= end1; begin++) {
					if (s.docIdList[j] == index[queryList[i]].docIdList[begin]) {
						// 在该段中找到了当前值
						isFind = true;
						break;
					}
					else if (s.docIdList[j] < index[queryList[i]].docIdList[begin]) {
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

	// 进程通信
	// 用0号进程做收集
	//if (rank == 0) {
	//	for (int i = 1; i < size; i++) {
	//		int count;
	//		MPI_Recv(&count, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	//		unsigned int* temp;// 转成int数组发送
	//		temp = new unsigned int[count];
	//		// 接受数据
	//		MPI_Recv(temp, count, MPI_UNSIGNED, i, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	//		s.docIdList.insert(s.docIdList.end(), temp, temp + count);
	//		delete[]temp;
	//	}
	//	s.length = s.docIdList.size();
	//}
	//// 非0号线程则把vector发送过去
	//else {
	//	unsigned int* temp;// 转成int数组发送
	//	temp = new unsigned[s.length];
	//	copy(s.docIdList.begin(), s.docIdList.end(), temp);
	//	MPI_Send(&s.length, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);// 先发一个长度过去
	//	MPI_Send(temp, s.length, MPI_UNSIGNED, 0, 1, MPI_COMM_WORLD);// 再把数据发过去
	//	delete[]temp;
	//}

	MPI_Barrier(MPI_COMM_WORLD);// 保证在同一个函数步内
	if (rank == 0) {
		unsigned int* temp;// 转成int数组发送
		//int maxLen = max(initialLength / size, initialLength - initialLength / size * (size - 1));
		temp = new unsigned int[initialLength];// 节省空间实测比开个大的还慢？
		for (int i = 1; i < size; i++) {
			// 接受数据
			MPI_Recv(temp, initialLength, MPI_UNSIGNED, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			int len = temp[0];
			s.docIdList.insert(s.docIdList.end(), temp + 1, temp + 1 + len);
		}
		delete[]temp;
		s.length = s.docIdList.size();
	}
	// 非0号线程则把vector发送过去
	else {
		unsigned int* temp;// 转成int数组发送
		temp = new unsigned[s.length + 1];
		temp[0] = s.length;
		copy(s.docIdList.begin(), s.docIdList.end(), temp + 1);
		// 直接一起发过去
		MPI_Send(temp, s.length + 1, MPI_UNSIGNED, 0, 1, MPI_COMM_WORLD);// 再把数据发过去
		delete[]temp;
	}

	return s;
}
InvertedIndex HASH_MPI_SSE(int* queryList, vector<InvertedIndex>& index, int num, int rank, int size) {
	// 取最短的列表
	int initialLength = index[queryList[0]].length;
	InvertedIndex s;// 取工作范围1/4
	int start = (initialLength / size) * rank,
		end = rank != size - 1 ? (initialLength / size) * (rank + 1) : initialLength;// 末尾特殊处理
	s.docIdList.assign(index[queryList[0]].docIdList.begin() + start, index[queryList[0]].docIdList.begin() + end);
	s.length = s.docIdList.size();

	for (int i = 1; i < num; i++) {// 与剩余列表求交
		// SVS的思想，将s挨个按表求交
		int count = 0;
		int length = s.length;

		for (int j = 0; j < length; j++) {
			bool isFind = false;
			int hashValue = s.docIdList[j] / 512;
			// 找到该hash值在当前待求交列表中对应的段
			int begin = hashBucket[queryList[i]][hashValue].begin;
			int end1 = hashBucket[queryList[i]][hashValue].end;
			if (begin < 0 || end1 < 0) {// 该值肯定找不到，不用往后做了
				continue;
			}
			else {
				__m128i ss = _mm_set1_epi32(s.docIdList[j]);// 填充32位

				for (begin; begin <= end1 - 3; begin += 4) {
					__m128i ii;
					ii = _mm_loadu_epi32(&index[queryList[i]].docIdList[begin]);// 一次取四个
					__m128i tmp = _mm_set1_epi32(0);
					tmp = _mm_cmpeq_epi32(ss, ii);// 比较向量每一位
					int mask = _mm_movemask_epi8(tmp);// 转为掩码，如果有一个数相等,就不会是全0

					if (mask != 0) {// 查看比较结果
						isFind = true;
						break;
					}
					else if (s.docIdList[j] < index[queryList[i]].docIdList[begin])// 及时break，避免超过，下一元素需重头再来
						break;
				}
				if (!isFind && (begin > end1 - 3)) {// 处理剩余元素
					for (; begin <= end1; begin++)
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

	// 进程通信
	// 用0号进程做收集
	if (rank == 0) {
		for (int i = 1; i < size; i++) {
			int count;
			MPI_Recv(&count, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			unsigned int* temp;// 转成int数组发送
			temp = new unsigned int[count];
			// 接受数据
			MPI_Recv(temp, count, MPI_UNSIGNED, i, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			s.docIdList.insert(s.docIdList.end(), temp, temp + count);
			delete[]temp;
		}
		s.length = s.docIdList.size();
	}
	// 非0号线程则把vector发送过去
	else {
		unsigned int* temp;// 转成int数组发送
		temp = new unsigned[s.length];
		copy(s.docIdList.begin(), s.docIdList.end(), temp);
		MPI_Send(&s.length, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);// 先发一个长度过去
		MPI_Send(temp, s.length, MPI_UNSIGNED, 0, 1, MPI_COMM_WORLD);// 再把数据发过去
		delete[]temp;
	}

	return s;
}

bool flag = false;
InvertedIndex HASH_MPI_i(int* queryList, vector<InvertedIndex>& index, int num, int rank, int size) {
	// 取最短的列表
	int initialLength = index[queryList[0]].length;
	InvertedIndex s;// 取工作范围1/4
	int start = (initialLength / size) * rank,
		end = rank != size - 1 ? (initialLength / size) * (rank + 1) : initialLength;// 末尾特殊处理
	s.docIdList.assign(index[queryList[0]].docIdList.begin() + start, index[queryList[0]].docIdList.begin() + end);
	s.length = s.docIdList.size();

	for (int i = 1; i < num; i++) {// 与剩余列表求交
		// SVS的思想，将s挨个按表求交
		int count = 0;
		int length = s.length;

		for (int j = 0; j < length; j++) {
			bool isFind = false;
			int hashValue = s.docIdList[j] / 512;
			// 找到该hash值在当前待求交列表中对应的段
			int begin = hashBucket[queryList[i]][hashValue].begin;
			int end1 = hashBucket[queryList[i]][hashValue].end;
			if (begin < 0 || end1 < 0) {// 该值肯定找不到，不用往后做了
				continue;
			}
			else {
				for (begin; begin <= end1; begin++) {
					if (s.docIdList[j] == index[queryList[i]].docIdList[begin]) {
						// 在该段中找到了当前值
						isFind = true;
						break;
					}
					else if (s.docIdList[j] < index[queryList[i]].docIdList[begin]) {
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

	// 修改通信机制，将长度放入数组0号位置
	// 用0号进程做收集
	MPI_Barrier(MPI_COMM_WORLD);// 保证在同一个函数步内
	if (rank == 0) {
		unsigned int* temp;// 转成int数组发送
		temp = new unsigned int[initialLength];// 节省空间实测比开个大的还慢？
		for (int i = 1; i < size; i++) {
			// 接受数据
			MPI_Request request;
			MPI_Irecv(temp, initialLength, MPI_UNSIGNED, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &request);
			MPI_Wait(&request, MPI_STATUS_IGNORE);

			int len = temp[0];
			s.docIdList.insert(s.docIdList.end(), temp+1, temp +1+len);
		}
		delete[]temp;
		s.length = s.docIdList.size();
	}
	// 非0号线程则把vector发送过去
	else {
		unsigned int* temp;// 转成int数组发送
		temp = new unsigned[s.length+1];
		temp[0] = s.length;
		copy(s.docIdList.begin(), s.docIdList.end(), temp+1);
		// 直接一起发过去
		MPI_Request request;
		MPI_Isend(temp, s.length+1, MPI_UNSIGNED, 0, 1,MPI_COMM_WORLD,&request);// 再把数据发过去
		flag = true;

		delete []temp;
	}

	return s;
}

void testMPI(int argc, char* argv[]) {
	int myid, numprocs, namelen;
	char processor_name[MPI_MAX_PROCESSOR_NAME];

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Get_processor_name(processor_name, &namelen);
	vector<int> myData;
	for (int i = 0; i < 101; i++) {
		myData.push_back(i * myid);
	}
	if (myid == 0)
	{
	    //printf("number of processes: %d\n", numprocs);
		vector<int> temp;
		temp.resize(100);
		for (int i = 1; i < numprocs; i++) {
			MPI_Recv(&temp, 100, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			myData.insert(myData.end(), temp.begin(), temp.end());
		}
	}
	else {
		MPI_Send(&myData, 99, MPI_INT, 0, myid, MPI_COMM_WORLD);
	}
	printf("%s: Hello world from process %d \n", processor_name, myid);
	if (myid == 0) {
		for (int i = 0; i < 400; i++)
			printf("%d ", myData[i]);
	}

	MPI_Finalize();
}

int main(int argc, char* argv[])
{
	// 读取二进制文件
	fstream file;
	file.open("ExpIndex", ios::binary | ios::in);
	if (!file.is_open()) {
		printf("Wrong in opening file!\n");
		return 0;

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
	printf("here\n");

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

	//------初始化MPI------
	int provided;// 多线程
	MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);
	if (provided < MPI_THREAD_FUNNELED)// 提供的等级不够
		MPI_Abort(MPI_COMM_WORLD, 1);

	//MPI_Init(&argc, &argv);
	int rank, size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	printf("-----process%d start-----\n", rank);

	//------求交------
	long long head, tail, freq;
	if (rank == 0) {// 一个进程输出
		QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
		QueryPerformanceCounter((LARGE_INTEGER*)&head);// Start Time
	}

	int length = floor(count / size);// 每个进程处理的length
	int start = length * rank;
	int end = rank != size - 1 ? length * (rank + 1) : count;// 考虑末尾

	//preprocessing(*invertedLists, 2000);
#pragma omp parallel for num_threads(NUM_THREADS)
	for (int i = start; i < end; i++) {// count个查询
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
		InvertedIndex res = SVS(list, *invertedLists, num);
		//printf("%d\n", i);
	}

	MPI_Barrier(MPI_COMM_WORLD);// 进程同步

	printf("-----process%d finished-----\n", rank);

	// 让一个进程输出
	if (rank == 0) {
		QueryPerformanceCounter((LARGE_INTEGER*)&tail);// End Time
		printf("%f\n", (tail - head) * 1000.0 / freq);
	}

	MPI_Finalize();
}

