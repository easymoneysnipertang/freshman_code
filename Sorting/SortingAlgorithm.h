#pragma once
#include<iostream>
#include<list>
using namespace std;

class SortingAlgorithm
{
private:
	template<class T>
	void mergePass(T x[], T y[], int n, int seg) {
		//确定归并的子序列左右边界
		int i = 0;//下一数据段起点
		//从x到y归并相邻的数据段
		while (i <= n - 2 * seg) {
			merge(x, y, i, i + seg - 1, i + 2 * seg - 1);
			i = i + 2 * seg;
		}
		if (i + seg < n)
			//剩有两个数据段，但最后一个凑不满完整的
			merge(x, y, i, i + seg - 1, n - 1);
		else
			//只剩一个数据段了,直接复制过去
			for (int j = i; j < n; j++)
				y[j] = x[j];
	}
	template<class T>
	void merge(T c[], T d[], int startOfFirst, int endOfFirst, int endOfSecond) {
		//把两个相邻数据段从c归并到d
		int first = startOfFirst, second = endOfFirst + 1, result = startOfFirst;
		while (first <= endOfFirst && second <= endOfSecond) {
			//挨着从c拿入d，直到有一段完全放完
			if (c[first] <= c[second])
				d[result++] = c[first++];
			else
				d[result++] = c[second++];
		}
		//归并剩余的元素
		if (first > endOfFirst)
			for (int q = second; q <= endOfSecond; q++)
				d[result++] = c[q];
		else
			for (int q = first; q <= endOfFirst; q++)
				d[result++] = c[q];
	}
	template<class T>
	void max_heapify(T arr[], int start, int end) {
		//调整为大根堆
		int root = start;
		int child = 2 * root + 1;
		while (child <= end) {//往下调整
			if (child + 1 <= end && arr[child] < arr[child + 1])//选择child中大的那一个
				child++;
			if (arr[root] > arr[child])//直接并调整结束
				return;
			else {
				swap(arr[root], arr[child]);//交换父子节点继续向下调整
				root = child;
				child = 2 * root + 1;
			}
		}
	}

public:
	template<class T>
	void bubble_sort(T arr[], int len) {
		for (int i = 0; i < len; i++) {
			for (int j = len - 1; j > i; j--)
				if (arr[j] < arr[j - 1])swap(arr[j], arr[j - 1]);
		}
	}

	template<class T>
	void selection_sort(T arr[], int len) {
		for (int i = 0; i < len-1; i++) {
			int min = i;
			for (int j = i + 1; j < len; j++)
				if (arr[j] < arr[min])
					min = j;
			swap(arr[i], arr[min]);
		}
	}

	template<class T>
	void insertion_sort(T arr[], int len) {
		for (int i = 1; i < len; i++) {
			T key = arr[i];
			int j = i - 1;//检查i之前的元素
			while (j >= 0 && key < arr[j]) {
				arr[j + 1] = arr[j];
				j--;//往后腾位置
			}
			arr[j + 1] = key;
		}
	}

	template<class T>
	void shell_sort(T arr[], int len) {
		//int h = 1;//增量
		//while (h < len / 3)
		//	h = 3 * h + 1;
		//while (h >= 1) {
		//	for (int i = h; i < len; i++)
		//		for (int j = i; j >= h && arr[j] < arr[j - h]; j -= h)
		//			swap(arr[j], arr[j - h]);
		//	h /= 3;
		//}
		for (int inc = len / 2; inc >= 1; inc /= 2) {
			for (int i = inc; i < len; i++) {
				//对每段进行插入排序
				T key = arr[i];
				int j = i - inc;
				while (j >= 0 && key < arr[j]) {
					arr[j + inc] = arr[j];
					j -= inc;
				}
				arr[j + inc] = key;
			}
		}
	}

	
	template<class T>
	void merge_sort(T arr[], int len) {
		//迭代，根据段值相邻的一点点归并
		T* b = new T[len];
		int seg = 1;
		while (seg < len) {//迭代一次做两次归并，数据仍在arr中
			mergePass(arr, b, len, seg);
			seg += seg;
			mergePass(b, arr, len, seg);
			seg += seg;
		}
		delete[]b;
	}

	template<class T>
	void merge_sort_recursive(T arr[], T b[],int start, int end) {
		if (start < end) {//大于一个元素，继续细分
			int mid = (start + end) / 2;
			merge_sort_recursive(arr, b, start, mid);
			merge_sort_recursive(arr, b, mid + 1, end);
			merge(arr, b, start, mid, end);
			merge(b, arr, start, mid, end);//再放回去
		}
	}

	//template<class T>
	//void quick_sort(T arr[], int start,int end) {
	//	if (start < end) {
	//		T pivot = arr[end];
	//		int left = start, right = end - 1;
	//		while (left < right) {
	//			while (left < right && arr[left] < pivot)
	//				left++;
	//			while (right > left && arr[right] > pivot)
	//				right--;
	//			swap(arr[left], arr[right]);
	//		}
	//		if (arr[left] >= pivot)//放回枢值
	//			swap(arr[left], arr[end]);
	//		else//特殊情况，end是极大值
	//			left++;
	//		quick_sort(arr, start, left - 1);
	//		quick_sort(arr, left + 1, end);
	//	}
	//}

	template<class T>
	void quick_sort(T a[],int len) {//启动器
		if (len <= 1)return;
		//选第一个元素作中枢，把最大元素移到右端去
		int max = 0;
		for (int i = 1; i < len; i++) 
			if (a[i] > a[max])max = i;
		swap(a[len - 1], a[max]);
		quickSort(a, 0, len - 2);
	}
	template<class T>
	void quickSort(T a[], int left, int right) {
		//a[right+1]>=a[left:right]
		if (left >= right)return;
		int leftcursor = left, rightcursor = right + 1;//遍历索引
		T pivot = a[left];//第一个元素作枢值
		while (true) {
			do {
				leftcursor++;
			} while (a[leftcursor] < pivot);
			do {
				rightcursor--;
			} while (a[rightcursor] > pivot);
			
			if (leftcursor >= rightcursor)break;//指标交错
			swap(a[leftcursor], a[rightcursor]);
		}
		//放回中枢！
		a[left] = a[rightcursor];
		a[rightcursor] = pivot;
		quickSort(a, left, rightcursor - 1);
		quickSort(a, rightcursor + 1, right);
	}
	
	template<class T>
	void heap_sort(T arr[], int len) {
		for (int i = len / 2 - 1; i >= 0; i--) {//从最后一个父节点开始调整
			max_heapify(arr, i, len - 1);
		}
		for (int i = len - 1; i > 0; i--) {
			swap(arr[0], arr[i]);//将最大的放到最后，再重新整堆
			max_heapify(arr, 0, i - 1);
		}
	}

	void binSort(list<int>& l, int range, int times) {
		list<int>* bin;
		bin = new list<int>[range + 1];
		int numberOfElements = l.size();
		for (int i = 1; i <= numberOfElements; i++) {
			//结点分配
			int x = l.front();
			l.pop_front();
			int y = (x % (int)pow(range, times)) / pow(range, times - 1);//本次比较的位数

			bin[y].insert(bin[y].begin(), x);//插入不同箱子
		}
		for (int j = range; j >= 0; j--) {//从箱子中收集元素
			while (!bin[j].empty()) {
				int x = bin[j].front();
				bin[j].pop_front();
				l.insert(l.begin(), x);
			}
		}
		delete[]bin;
	}
	void radix_sort(int arr[], int len) {
		//求出最长位数
		list<int>l;
		int maxdata = arr[0];
		l.push_back(arr[0]);
		for (int i = 1; i < len; i++)
		{
			if (maxdata < arr[i])maxdata = arr[i];
			l.push_back(arr[i]);
		}
		int length = 1, p = 10;
		while (maxdata >= p) {
			maxdata /= 10;
			length++;
		}
		
		for (int i = 1; i <= length; i++) //一位一位排
			binSort(l, 10, i);
		copy(l.begin(),l.end(),arr);
	}
};

