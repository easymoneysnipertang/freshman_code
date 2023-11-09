#pragma once
#include<iostream>
#include<list>
using namespace std;

class SortingAlgorithm
{
private:
	template<class T>
	void mergePass(T x[], T y[], int n, int seg) {
		//ȷ���鲢�����������ұ߽�
		int i = 0;//��һ���ݶ����
		//��x��y�鲢���ڵ����ݶ�
		while (i <= n - 2 * seg) {
			merge(x, y, i, i + seg - 1, i + 2 * seg - 1);
			i = i + 2 * seg;
		}
		if (i + seg < n)
			//ʣ���������ݶΣ������һ���ղ���������
			merge(x, y, i, i + seg - 1, n - 1);
		else
			//ֻʣһ�����ݶ���,ֱ�Ӹ��ƹ�ȥ
			for (int j = i; j < n; j++)
				y[j] = x[j];
	}
	template<class T>
	void merge(T c[], T d[], int startOfFirst, int endOfFirst, int endOfSecond) {
		//�������������ݶδ�c�鲢��d
		int first = startOfFirst, second = endOfFirst + 1, result = startOfFirst;
		while (first <= endOfFirst && second <= endOfSecond) {
			//���Ŵ�c����d��ֱ����һ����ȫ����
			if (c[first] <= c[second])
				d[result++] = c[first++];
			else
				d[result++] = c[second++];
		}
		//�鲢ʣ���Ԫ��
		if (first > endOfFirst)
			for (int q = second; q <= endOfSecond; q++)
				d[result++] = c[q];
		else
			for (int q = first; q <= endOfFirst; q++)
				d[result++] = c[q];
	}
	template<class T>
	void max_heapify(T arr[], int start, int end) {
		//����Ϊ�����
		int root = start;
		int child = 2 * root + 1;
		while (child <= end) {//���µ���
			if (child + 1 <= end && arr[child] < arr[child + 1])//ѡ��child�д����һ��
				child++;
			if (arr[root] > arr[child])//ֱ�Ӳ���������
				return;
			else {
				swap(arr[root], arr[child]);//�������ӽڵ�������µ���
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
			int j = i - 1;//���i֮ǰ��Ԫ��
			while (j >= 0 && key < arr[j]) {
				arr[j + 1] = arr[j];
				j--;//������λ��
			}
			arr[j + 1] = key;
		}
	}

	template<class T>
	void shell_sort(T arr[], int len) {
		//int h = 1;//����
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
				//��ÿ�ν��в�������
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
		//���������ݶ�ֵ���ڵ�һ���鲢
		T* b = new T[len];
		int seg = 1;
		while (seg < len) {//����һ�������ι鲢����������arr��
			mergePass(arr, b, len, seg);
			seg += seg;
			mergePass(b, arr, len, seg);
			seg += seg;
		}
		delete[]b;
	}

	template<class T>
	void merge_sort_recursive(T arr[], T b[],int start, int end) {
		if (start < end) {//����һ��Ԫ�أ�����ϸ��
			int mid = (start + end) / 2;
			merge_sort_recursive(arr, b, start, mid);
			merge_sort_recursive(arr, b, mid + 1, end);
			merge(arr, b, start, mid, end);
			merge(b, arr, start, mid, end);//�ٷŻ�ȥ
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
	//		if (arr[left] >= pivot)//�Ż���ֵ
	//			swap(arr[left], arr[end]);
	//		else//���������end�Ǽ���ֵ
	//			left++;
	//		quick_sort(arr, start, left - 1);
	//		quick_sort(arr, left + 1, end);
	//	}
	//}

	template<class T>
	void quick_sort(T a[],int len) {//������
		if (len <= 1)return;
		//ѡ��һ��Ԫ�������࣬�����Ԫ���Ƶ��Ҷ�ȥ
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
		int leftcursor = left, rightcursor = right + 1;//��������
		T pivot = a[left];//��һ��Ԫ������ֵ
		while (true) {
			do {
				leftcursor++;
			} while (a[leftcursor] < pivot);
			do {
				rightcursor--;
			} while (a[rightcursor] > pivot);
			
			if (leftcursor >= rightcursor)break;//ָ�꽻��
			swap(a[leftcursor], a[rightcursor]);
		}
		//�Ż����࣡
		a[left] = a[rightcursor];
		a[rightcursor] = pivot;
		quickSort(a, left, rightcursor - 1);
		quickSort(a, rightcursor + 1, right);
	}
	
	template<class T>
	void heap_sort(T arr[], int len) {
		for (int i = len / 2 - 1; i >= 0; i--) {//�����һ�����ڵ㿪ʼ����
			max_heapify(arr, i, len - 1);
		}
		for (int i = len - 1; i > 0; i--) {
			swap(arr[0], arr[i]);//�����ķŵ��������������
			max_heapify(arr, 0, i - 1);
		}
	}

	void binSort(list<int>& l, int range, int times) {
		list<int>* bin;
		bin = new list<int>[range + 1];
		int numberOfElements = l.size();
		for (int i = 1; i <= numberOfElements; i++) {
			//������
			int x = l.front();
			l.pop_front();
			int y = (x % (int)pow(range, times)) / pow(range, times - 1);//���αȽϵ�λ��

			bin[y].insert(bin[y].begin(), x);//���벻ͬ����
		}
		for (int j = range; j >= 0; j--) {//���������ռ�Ԫ��
			while (!bin[j].empty()) {
				int x = bin[j].front();
				bin[j].pop_front();
				l.insert(l.begin(), x);
			}
		}
		delete[]bin;
	}
	void radix_sort(int arr[], int len) {
		//����λ��
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
		
		for (int i = 1; i <= length; i++) //һλһλ��
			binSort(l, 10, i);
		copy(l.begin(),l.end(),arr);
	}
};

