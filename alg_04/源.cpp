#include<iostream>
#include<algorithm>
#include<queue>
#include<iomanip>

using namespace std;

int main() {
	//初始化
	int n;
	priority_queue<int,vector<int>,greater<int>> weight;//优先队列
	cin >> n;
	int sumOfWeight = 0;//总的字符数量
	//输入n个权值
	for (int i = 0; i < n; i++) {
		int temp;
		cin >> temp;
		sumOfWeight += temp;
		weight.push(temp);
	}
	
	int sumOfPath = 0;//不需要构建树去求深度->把非叶节点加和，在第几层就被计算了几次
	while (n != 1) {
		//排序，把小的两个节点放到前面来
		//弹出两个节点
		int tempWeight1 = weight.top();
		weight.pop();
		int tempWeight2 = weight.top();
		weight.pop();
		//cout << tempWeight1 << " " << tempWeight2 << endl;
		//相加后放回
		int mergeWeight = tempWeight1 + tempWeight2;
		weight.push(mergeWeight);

		//总和加上该权，代表这一堆节点又多访问了一层，编码长度加1
		sumOfPath += mergeWeight;
		//总节点数减1
		n--;
	}
	cout << fixed << setprecision(2) << (float)sumOfPath / sumOfWeight;
}