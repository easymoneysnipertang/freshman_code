#include<iostream>
#include<fstream>
#include<list>;
using namespace std;

//关键字参与的比较与交换次数
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
		int j = i - 1;//检查i之前的元素
		timesOfCompare++;
		while (j >= 0 && key < a[j]) {
			a[j + 1] = a[j];
			j--;//往后腾位置
			timesOfCompare++;
			timesOfSwap++;
		}
		a[j + 1] = key;
	}
}

void shell_sort(int a[], int len) {
	for (int inc = len / 2; inc >= 1; inc /= 2) {//间隔
		for (int i = inc; i < len; i++) {
			//对每段进行插入排序
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
	int leftcursor = left, rightcursor = right + 1;//遍历索引
	int pivot = a[left];//第一个元素作枢值
	while (true) {
		do {
			leftcursor++;
			timesOfCompare++;
		} while (a[leftcursor] < pivot);
		do {
			rightcursor--;
			timesOfCompare++;
		} while (a[rightcursor] > pivot);

		if (leftcursor >= rightcursor)break;//指标交错
		timesOfSwap++;
		swap(a[leftcursor], a[rightcursor]);
	}
	//放回中枢！
	a[left] = a[rightcursor];
	a[rightcursor] = pivot;
	quickSort(a, left, rightcursor - 1);
	quickSort(a, rightcursor + 1, right);
}
void quick_sort(int a[], int len) {//启动器
	if (len <= 1)return;
	//选第一个元素作中枢，把最大元素移到右端去
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
	//调整为大根堆
	int root = start;
	int child = 2 * root + 1;
	while (child <= end) {//往下调整
		timesOfCompare++;
		if (child + 1 <= end && arr[child] < arr[child + 1])//旋转child中大的那一个
			child++;
		timesOfCompare++;
		if (arr[root] > arr[child])//直接并调整结束
			return;
		else {
			swap(arr[root], arr[child]);//交换父子节点继续向下调整
			timesOfSwap++;
			root = child;
			child = 2 * root + 1;
		}
	}
}
void heap_sort(int arr[], int len) {
	for (int i = len / 2 - 1; i >= 0; i--) {//从最后一个父节点开始调整
		max_heapify(arr, i, len - 1);
	}
	for (int i = len - 1; i > 0; i--) {
		timesOfSwap++;
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
		timesOfSwap++;
		bin[y].insert(bin[y].begin(), x);//插入不同箱子
	}
	for (int j = range; j >= 0; j--) {//从箱子中收集元素
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
	//求出最长位数
	list<int>l;
	int maxdata = arr[0];
	l.push_back(arr[0]);
	for (int i = 1; i < len; i++)
	{
		timesOfCompare++;//todo:不好说
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
int getindex(bool* a,int m) {//转换成10进制数
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

	//奇偶性必须与k一致且不能大于k才可能导通，只有两行完全一样才可能同时导通
	//找所有可同时导通里面行数最多的
	
	//第一种思路：模拟过程，可导通的行遍历自己下面所有能导通的行，自身为0的列下面行对应翻转，出现0即pass
	int max = 0;
	for (int i = 0; i < n; i++)//遍历行
	{
		bool parityOfK = (k % 2) == 0;
		if (col[i].numsOfZero > k|| col[i].parity != parityOfK)continue;
		int count = 1;
		for (int j = i + 1; j < n; j++) {//遍历行，只往下组合
			if (col[j].numsOfZero!=col[i].numsOfZero)//必须与i个数相同，条件很强
				continue;
			bool flag = 1;
			for (int c = 0; c < m; c++)
			{
				bool t = mat[j][c];
				if (mat[i][c] == 0)t = t == 0 ? 1 : 0;//翻转
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
	//第二种：为可导通的行设定数值，转成十进制数，排序得到出现次数最多的十进制数
	for (int i = 0; i < n; i++)valid[i].index = valid[i].num = 0;
	bool parityOfK = (k % 2) == 0;
	int countOfAvailable = 0;
	for (int i = 0; i < n; i++) {//遍历行
		if (col[i].parity != parityOfK || col[i].numsOfZero > k)continue;
		int index = getindex(mat[i], m);//这一行对应的十进制数
		bool flag = 1;
		for (int j = 0; j < countOfAvailable; j++)
			if (valid[j].index == index) {
				valid[j].num++;
				flag = 0;
				break;
			}
		if (flag) {//未出现过的导通行
			valid[countOfAvailable].num++;
			valid[countOfAvailable++].index = index;
		}
	}
	//冒泡排序
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