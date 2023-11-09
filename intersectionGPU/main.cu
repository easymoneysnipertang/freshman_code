#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<sstream>
#include<Windows.h>
#include<algorithm>
#include"cuda_runtime.h"
#include"device_launch_parameters.h"
#include <thrust/device_vector.h>

using namespace std;

const int BLOCK_SIZE = 1024;

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

// adp实现
class QueryItem {
public:
	int cursor;// 当前读到哪了
	int end;// 倒排索引总长度
	int key;// 关键字值
};
bool operator<(const QueryItem& q1, const QueryItem& q2) {// 选剩余元素最少的元素
	return (q1.end - q1.cursor) < (q2.end - q2.cursor);
}
InvertedIndex ADP(int* queryList, vector<InvertedIndex>& index, int num)
{
	InvertedIndex S;
	QueryItem* list = new QueryItem[num]();
	for (int i = 0; i < num; i++)// 预处理
	{
		list[i].cursor = 0;
		list[i].key = queryList[i];
		list[i].end = index[queryList[i]].docIdList.size();
	}
	for (int i = list[0].cursor; i < list[0].end; i++) {// 最短的列表非空
		bool isFind = true;
		unsigned int e = index[list[0].key].docIdList[i];
		for (int s = 1; s != num && isFind == true; s++) {
			isFind = false;
			while (list[s].cursor < list[s].end) {// 检查s列表
				if (e == index[list[s].key].docIdList[list[s].cursor]) {
					isFind = true;
					break;
				}
				else if (e < index[list[s].key].docIdList[list[s].cursor])
					break;
				list[s].cursor++;// 当前访问过，且没找到合适的，往后移
			}
			// 下一个链表
		}
		// 当前元素已被访问过
		if (isFind)
			S.docIdList.push_back(e);
		//sort(list, list + num);// 重排，将未探查元素少的列表前移
	}
	return S;
}


__global__ void cmp_kernel(int* index, int cursor,int e,bool* isFind) {
	int tid = blockDim.x * blockIdx.x + threadIdx.x;//计算线程索引
	if (e == index[cursor+tid])
		*isFind = true;
}


//返回第一个大于等于目标元素的下标
__device__ int find1stGreaterEqual(const int* arr, unsigned int target, int begin, int end) {
	int left = begin;
	int right = end;
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
__global__ void ADP_kernel(int* index, int* lengthArr) {
	int tid = blockDim.x * blockIdx.x + threadIdx.x;//计算线程索引
	int totalThreads = gridDim.x * blockDim.x;

	int num = lengthArr[0];

	// 线程起始，结束
	int start = lengthArr[1] / totalThreads * tid, end = min(lengthArr[1] / totalThreads * (tid + 1), lengthArr[1]);// 第一个链表拿来做划分

	QueryItem* list = new QueryItem[num]();
	int count = lengthArr[1];
	for (int i = 1; i < num; i++)// 预处理
	{
		// 起始结束位置
		list[i].cursor = find1stGreaterEqual(index, index[start], count, count + lengthArr[i + 1]);
		list[i].end = count + lengthArr[i + 1];
		count += lengthArr[i + 1];
	}

	int position = start;
	for (int i = start; i < end; i++) {
		bool isFind = true;
		unsigned int e = index[i];
		for (int s = 1; s != num && isFind == true; s++) {
			isFind = false;
			while (list[s].cursor < list[s].end) {// 检查s列表
				if (e == index[list[s].cursor]) {
					isFind = true;
					break;
				}
				else if (e < index[list[s].cursor])
					break;
				list[s].cursor++;// 当前访问过，且没找到合适的，往后移
			}
			// 下一个链表
		}
		// 当前元素已被访问过
		if (isFind)
			index[position++] = e;
		// TODO：先不管，能跑起来再说

	}
	delete list;
}

int main() {
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
	file.close();

	//------求交------
	cout << "------intersection begin-----" << endl;
	cudaError_t ret;// 错误检查

	dim3 dimBlock(BLOCK_SIZE, 1);// 线程块
	dim3 dimGrid(1, 1);// 线程网格

	int* index;
	int* gpuIndex;

	double time = 0;

	for (int i = 0; i < count / 25; i++) {// count个查询
		int num = 0;// query查询项数
		for (int j = 0; j < 5; j++) {
			if (query[i][j] != 0) {
				num++;
			}
		}

		int* lengthArr = new int[num + 1];
		int* gpuLengthArr;
		ret = cudaMalloc((void**)&gpuLengthArr, (num + 1) * sizeof(int));
		if (ret != cudaSuccess) {
			fprintf(stderr, "cudaMalloc failed: %s\n", cudaGetErrorString(ret));
		}

		int totalLength = 0;
		// 获取各段长度
		lengthArr[0] = num;// 用0号位置来保存num
		for (int j = 0; j < num; j++) {
			int length = (*invertedLists)[query[i][j]].length;
			totalLength += length;
			lengthArr[j + 1] = length;
		}

		index = new int[totalLength];// 开辟一维数组
		ret = cudaMalloc((void**)&gpuIndex, totalLength * sizeof(int));
		if (ret != cudaSuccess) {
			fprintf(stderr, "cudaMalloc failed: %s\n", cudaGetErrorString(ret));
		}

		totalLength = 0;
		for (int j = 0; j < num; j++) {// 倒排链表全部放进一个二维数组->一维数组表示二维数组
			// 复制到数组当中去
			copy((*invertedLists)[query[i][j]].docIdList.begin(), (*invertedLists)[query[i][j]].docIdList.end(), index + totalLength);
			// 当前位置
			totalLength += lengthArr[j + 1];
		}

		cudaEvent_t start, stop;// 计时器
		float elapsedTime = 0.0;
		cudaEventCreate(&start);
		cudaEventCreate(&stop);
		cudaEventRecord(start, 0);// 开始计时

		// 传递到GPU端
		ret = cudaMemcpy(gpuLengthArr, lengthArr, (num + 1) * sizeof(int), cudaMemcpyHostToDevice);
		if (ret != cudaSuccess) {
			fprintf(stderr, "cudaMemcpy failed: %s\n", cudaGetErrorString(ret));
		}
		// 复制数据到GPU端
		ret = cudaMemcpy(gpuIndex, index, totalLength * sizeof(int), cudaMemcpyHostToDevice);
		if (ret != cudaSuccess) {
			fprintf(stderr, "cudaMemcpy failed: %s\n", cudaGetErrorString(ret));
		}

		// kernel！启动！
		ADP_kernel << <dimGrid, dimBlock >> > (gpuIndex, gpuLengthArr);
		cudaDeviceSynchronize();

		cudaError_t cudaStatus2 = cudaGetLastError();
		if (cudaStatus2 != cudaSuccess) {
			fprintf(stderr, "Kernel launch failed: %s\n", cudaGetErrorString(cudaStatus2));
		}
		//cout << i << endl;

		cudaEventRecord(stop, 0);
		cudaEventSynchronize(stop);// 停止计时
		cudaEventElapsedTime(&elapsedTime, start, stop);
		time += elapsedTime;

		//销毁计时器
		cudaEventDestroy(start);
		cudaEventDestroy(stop);

		delete index;
		delete lengthArr;
		cudaFree(gpuIndex);
		cudaFree(gpuLengthArr);
	}

	printf("GPU Time: %f ms\n", time);

	return 0;
}

//#include <stdio.h>
//
//// For the CUDA runtime routines (prefixed with "cuda_")
//#include <cuda_runtime.h>
//
//#include <helper_cuda.h>
///**
// * CUDA Kernel Device code
// *
// * Computes the vector addition of A and B into C. The 3 vectors have the same
// * number of elements numElements.
// */
//__global__ void
//vectorAdd(const float* A, const float* B, float* C, int numElements)
//{
//    int i = blockDim.x * blockIdx.x + threadIdx.x;
//
//    if (i < numElements)
//    {
//        C[i] = A[i] + B[i];
//    }
//}
//
///**
// * Host main routine
// */
//int
//main(void)
//{
//    // Error code to check return values for CUDA calls
//    cudaError_t err = cudaSuccess;
//
//    // Print the vector length to be used, and compute its size
//    int numElements = 50000;
//    size_t size = numElements * sizeof(float);
//    printf("[Vector addition of %d elements]\n", numElements);
//
//    // Allocate the host input vector A
//    float* h_A = (float*)malloc(size);
//
//    // Allocate the host input vector B
//    float* h_B = (float*)malloc(size);
//
//    // Allocate the host output vector C
//    float* h_C = (float*)malloc(size);
//
//    // Verify that allocations succeeded
//    if (h_A == NULL || h_B == NULL || h_C == NULL)
//    {
//        fprintf(stderr, "Failed to allocate host vectors!\n");
//        exit(EXIT_FAILURE);
//    }
//
//    // Initialize the host input vectors
//    for (int i = 0; i < numElements; ++i)
//    {
//        h_A[i] = rand() / (float)RAND_MAX;
//        h_B[i] = rand() / (float)RAND_MAX;
//    }
//
//    // Allocate the device input vector A
//    float* d_A = NULL;
//    err = cudaMalloc((void**)&d_A, size);
//
//    if (err != cudaSuccess)
//    {
//        fprintf(stderr, "Failed to allocate device vector A (error code %s)!\n", cudaGetErrorString(err));
//        exit(EXIT_FAILURE);
//    }
//
//    // Allocate the device input vector B
//    float* d_B = NULL;
//    err = cudaMalloc((void**)&d_B, size);
//
//    if (err != cudaSuccess)
//    {
//        fprintf(stderr, "Failed to allocate device vector B (error code %s)!\n", cudaGetErrorString(err));
//        exit(EXIT_FAILURE);
//    }
//
//    // Allocate the device output vector C
//    float* d_C = NULL;
//    err = cudaMalloc((void**)&d_C, size);
//
//    if (err != cudaSuccess)
//    {
//        fprintf(stderr, "Failed to allocate device vector C (error code %s)!\n", cudaGetErrorString(err));
//        exit(EXIT_FAILURE);
//    }
//
//    // Copy the host input vectors A and B in host memory to the device input vectors in
//    // device memory
//    printf("Copy input data from the host memory to the CUDA device\n");
//    err = cudaMemcpy(d_A, h_A, size, cudaMemcpyHostToDevice);
//
//    if (err != cudaSuccess)
//    {
//        fprintf(stderr, "Failed to copy vector A from host to device (error code %s)!\n", cudaGetErrorString(err));
//        exit(EXIT_FAILURE);
//    }
//
//    err = cudaMemcpy(d_B, h_B, size, cudaMemcpyHostToDevice);
//
//    if (err != cudaSuccess)
//    {
//        fprintf(stderr, "Failed to copy vector B from host to device (error code %s)!\n", cudaGetErrorString(err));
//        exit(EXIT_FAILURE);
//    }
//
//    // Launch the Vector Add CUDA Kernel
//    int threadsPerBlock = 256;
//    int blocksPerGrid = (numElements + threadsPerBlock - 1) / threadsPerBlock;
//    printf("CUDA kernel launch with %d blocks of %d threads\n", blocksPerGrid, threadsPerBlock);
//    vectorAdd << <blocksPerGrid, threadsPerBlock >> > (d_A, d_B, d_C, numElements);
//    err = cudaGetLastError();
//
//    if (err != cudaSuccess)
//    {
//        fprintf(stderr, "Failed to launch vectorAdd kernel (error code %s)!\n", cudaGetErrorString(err));
//        exit(EXIT_FAILURE);
//    }
//
//    // Copy the device result vector in device memory to the host result vector
//    // in host memory.
//    printf("Copy output data from the CUDA device to the host memory\n");
//    err = cudaMemcpy(h_C, d_C, size, cudaMemcpyDeviceToHost);
//
//    if (err != cudaSuccess)
//    {
//        fprintf(stderr, "Failed to copy vector C from device to host (error code %s)!\n", cudaGetErrorString(err));
//        exit(EXIT_FAILURE);
//    }
//
//    // Verify that the result vector is correct
//    for (int i = 0; i < numElements; ++i)
//    {
//        if (fabs(h_A[i] + h_B[i] - h_C[i]) > 1e-5)
//        {
//            fprintf(stderr, "Result verification failed at element %d!\n", i);
//            exit(EXIT_FAILURE);
//        }
//    }
//
//    printf("Test PASSED\n");
//
//    // Free device global memory
//    err = cudaFree(d_A);
//
//    if (err != cudaSuccess)
//    {
//        fprintf(stderr, "Failed to free device vector A (error code %s)!\n", cudaGetErrorString(err));
//        exit(EXIT_FAILURE);
//    }
//
//    err = cudaFree(d_B);
//
//    if (err != cudaSuccess)
//    {
//        fprintf(stderr, "Failed to free device vector B (error code %s)!\n", cudaGetErrorString(err));
//        exit(EXIT_FAILURE);
//    }
//
//    err = cudaFree(d_C);
//
//    if (err != cudaSuccess)
//    {
//        fprintf(stderr, "Failed to free device vector C (error code %s)!\n", cudaGetErrorString(err));
//        exit(EXIT_FAILURE);
//    }
//
//    // Free host memory
//    free(h_A);
//    free(h_B);
//    free(h_C);
//
//    printf("Done\n");
//    return 0;
//}
