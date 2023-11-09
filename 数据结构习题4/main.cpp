#include<iostream>
#include<fstream>
#include<list>;
using namespace std;

//�ؼ��ֲ���ıȽ��뽻������
int timesOfCompare;
int timesOfSwap;

void bubble_sort(int a[], int len) {
	for (int i = 0; i < len; i++) 
		for (int j = len - 1; j > i; j--) {
			timesOfCompare++;
			if (a[j] < a[j - 1]) {
				swap(a[j], a[j - 1]);
				timesOfSwap++;
			}
		}
}

void insertion_sort(int a[],int len) {
	for (int i = 1; i < len; i++) {
		int key = a[i];
		int j = i - 1;//���i֮ǰ��Ԫ��
		timesOfCompare++;
		while (j >= 0 && key < a[j]) {
			a[j + 1] = a[j];
			j--;//������λ��
			timesOfCompare++;
			timesOfSwap++;
		}
		a[j + 1] = key;
	}
}

void shell_sort(int a[], int len) {
	for (int inc = len / 2; inc >= 1; inc /= 2) {//���
		for (int i = inc; i < len; i++) {
			//��ÿ�ν��в�������
			int key = a[i];
			int j = i - inc;
			timesOfCompare++;
			while (j >= 0 && key < a[j]) {
				a[j + inc] = a[j];
				j -= inc;
				timesOfCompare++;
				timesOfSwap++;
			}
			a[j + inc] = key;
		}
	}
}

void quickSort(int a[], int left, int right) {
	//a[right+1]>=a[left:right]
	if (left >= right)return;
	int leftcursor = left, rightcursor = right + 1;//��������
	int pivot = a[left];//��һ��Ԫ������ֵ
	while (true) {
		do {
			leftcursor++;
			timesOfCompare++;
		} while (a[leftcursor] < pivot);
		do {
			rightcursor--;
			timesOfCompare++;
		} while (a[rightcursor] > pivot);

		if (leftcursor >= rightcursor)break;//ָ�꽻��
		timesOfSwap++;
		swap(a[leftcursor], a[rightcursor]);
	}
	//�Ż����࣡
	a[left] = a[rightcursor];
	a[rightcursor] = pivot;
	quickSort(a, left, rightcursor - 1);
	quickSort(a, rightcursor + 1, right);
}
void quick_sort(int a[], int len) {//������
	if (len <= 1)return;
	//ѡ��һ��Ԫ�������࣬�����Ԫ���Ƶ��Ҷ�ȥ
	int max = 0;
	for (int i = 1; i < len; i++) {
		timesOfCompare++;
		if (a[i] > a[max])max = i;
	}
	timesOfSwap++;
	swap(a[len - 1], a[max]);
	quickSort(a, 0, len - 2);
}

void max_heapify(int arr[], int start, int end) {
	//����Ϊ�����
	int root = start;
	int child = 2 * root + 1;
	while (child <= end) {//���µ���
		timesOfCompare++;
		if (child + 1 <= end && arr[child] < arr[child + 1])//��תchild�д����һ��
			child++;
		timesOfCompare++;
		if (arr[root] > arr[child])//ֱ�Ӳ���������
			return;
		else {
			swap(arr[root], arr[child]);//�������ӽڵ�������µ���
			timesOfSwap++;
			root = child;
			child = 2 * root + 1;
		}
	}
}
void heap_sort(int arr[], int len) {
	for (int i = len / 2 - 1; i >= 0; i--) {//�����һ�����ڵ㿪ʼ����
		max_heapify(arr, i, len - 1);
	}
	for (int i = len - 1; i > 0; i--) {
		timesOfSwap++;
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
		timesOfSwap++;
		bin[y].insert(bin[y].begin(), x);//���벻ͬ����
	}
	for (int j = range; j >= 0; j--) {//���������ռ�Ԫ��
		while (!bin[j].empty()) {
			int x = bin[j].front();
			bin[j].pop_front();
			l.insert(l.begin(), x);
			timesOfSwap++;
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
		timesOfCompare++;//todo:����˵
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
	copy(l.begin(), l.end(), arr);
}

void question1() {
	ofstream output("data.txt");
	for (int i = 0; i < 10000; i++)
		output << rand() % 10000 << " ";
	output.close();
	int* data;
	data = new int[10000];
	ifstream input("data.txt");
	for (int i = 0; i < 10000; i++) 
		input >> data[i];
	input.close();
	ofstream out("sortedData.txt");

	/*bubble_sort(data, 10000);
	out << "bubble_sort: " << timesOfCompare << " " << timesOfSwap << endl;
	for (int i = 0; i < 10000; i++)
		out << data[i] << " ";
	timesOfCompare = timesOfSwap = 0;
	out.close();*/

	/*insertion_sort(data, 10000);
	out << "insertion_sort: " << timesOfCompare << " " << timesOfSwap << endl;
	for (int i = 0; i < 10000; i++)
		out << data[i] << " ";
	timesOfCompare = timesOfSwap = 0;
	out.close();*/

	/*shell_sort(data, 10000);
	out << "shell_sort: " << timesOfCompare << " " << timesOfSwap << endl;
	for (int i = 0; i < 10000; i++)
		out << data[i] << " ";
	timesOfCompare = timesOfSwap = 0;
	out.close();*/

	/*quick_sort(data, 10000);
	out << "quick_sort: " << timesOfCompare << " " << timesOfSwap << endl;
	for (int i = 0; i < 10000; i++)
		out << data[i] << " ";
	timesOfCompare = timesOfSwap = 0;
	out.close();*/

	/*heap_sort(data, 10000);
	out << "heap_sort: " << timesOfCompare << " " << timesOfSwap << endl;
	for (int i = 0; i < 10000; i++)
		out << data[i] << " ";
	timesOfCompare = timesOfSwap = 0;
	out.close();*/

	radix_sort(data, 10000);
	out << "radix_sort: " << timesOfCompare << " " << timesOfSwap << endl;
	for (int i = 0; i < 10000; i++)
		out << data[i] << " ";
	timesOfCompare = timesOfSwap = 0;
	out.close();
}



int numsOfZero(bool* a,int m) {
	int count = 0;
	for (int i = 0; i < m; i++)
		if (a[i] == 0)count++;
	return count;
}
struct info {
	int numsOfZero;
	bool parity;
}col[100];
struct dec{
	int index;
	int num;
}valid[100];
int getindex(bool* a,int m) {//ת����10������
	int num = 0;
	for (int i = 0; i < m; i++) {
		num *= 2;
		num += (int)a[i];
	}
	return num;
}
void question2() {
	int n, m, k;
	cin >> n >> m;
	bool** mat;
	mat = new bool* [n];
	for (int i = 0; i < n; i++)*(mat + i) = new bool[m];
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++)
			cin >> mat[i][j];
		col[i].numsOfZero = numsOfZero(mat[i], m);
		col[i].parity = (col[i].numsOfZero % 2) == 0;
	}
	cin >> k;

	//��ż�Ա�����kһ���Ҳ��ܴ���k�ſ��ܵ�ͨ��ֻ��������ȫһ���ſ���ͬʱ��ͨ
	//�����п�ͬʱ��ͨ������������
	
	//��һ��˼·��ģ����̣��ɵ�ͨ���б����Լ����������ܵ�ͨ���У�����Ϊ0���������ж�Ӧ��ת������0��pass
	int max = 0;
	for (int i = 0; i < n; i++)//������
	{
		bool parityOfK = (k % 2) == 0;
		if (col[i].numsOfZero > k|| col[i].parity != parityOfK)continue;
		int count = 1;
		for (int j = i + 1; j < n; j++) {//�����У�ֻ�������
			if (col[j].numsOfZero!=col[i].numsOfZero)//������i������ͬ��������ǿ
				continue;
			bool flag = 1;
			for (int c = 0; c < m; c++)
			{
				bool t = mat[j][c];
				if (mat[i][c] == 0)t = t == 0 ? 1 : 0;//��ת
				if (!t) {
					flag = 0;
					break;
				}
			}
			if (flag)count++;
		}
		if (count > max)max = count;
	}
	cout << max;

	cout << endl;
	//�ڶ��֣�Ϊ�ɵ�ͨ�����趨��ֵ��ת��ʮ������������õ����ִ�������ʮ������
	for (int i = 0; i < n; i++)valid[i].index = valid[i].num = 0;
	bool parityOfK = (k % 2) == 0;
	int countOfAvailable = 0;
	for (int i = 0; i < n; i++) {//������
		if (col[i].parity != parityOfK || col[i].numsOfZero > k)continue;
		int index = getindex(mat[i], m);//��һ�ж�Ӧ��ʮ������
		bool flag = 1;
		for (int j = 0; j < countOfAvailable; j++)
			if (valid[j].index == index) {
				valid[j].num++;
				flag = 0;
				break;
			}
		if (flag) {//δ���ֹ��ĵ�ͨ��
			valid[countOfAvailable].num++;
			valid[countOfAvailable++].index = index;
		}
	}
	//ð������
	for (int i = 0; i < countOfAvailable; i++) 
		for (int j = countOfAvailable - 1; j > i; j--)
			if (valid[j].num > valid[i].num)
				swap(valid[j], valid[i]);
	cout << valid[0].num;
}

int main() {
	/*srand((int)time(NULL));
	question1();*/
	question2();
	return 0;
}