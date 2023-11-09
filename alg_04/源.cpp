#include<iostream>
#include<algorithm>
#include<queue>
#include<iomanip>

using namespace std;

int main() {
	//��ʼ��
	int n;
	priority_queue<int,vector<int>,greater<int>> weight;//���ȶ���
	cin >> n;
	int sumOfWeight = 0;//�ܵ��ַ�����
	//����n��Ȩֵ
	for (int i = 0; i < n; i++) {
		int temp;
		cin >> temp;
		sumOfWeight += temp;
		weight.push(temp);
	}
	
	int sumOfPath = 0;//����Ҫ������ȥ�����->�ѷ�Ҷ�ڵ�Ӻͣ��ڵڼ���ͱ������˼���
	while (n != 1) {
		//���򣬰�С�������ڵ�ŵ�ǰ����
		//���������ڵ�
		int tempWeight1 = weight.top();
		weight.pop();
		int tempWeight2 = weight.top();
		weight.pop();
		//cout << tempWeight1 << " " << tempWeight2 << endl;
		//��Ӻ�Ż�
		int mergeWeight = tempWeight1 + tempWeight2;
		weight.push(mergeWeight);

		//�ܺͼ��ϸ�Ȩ��������һ�ѽڵ��ֶ������һ�㣬���볤�ȼ�1
		sumOfPath += mergeWeight;
		//�ܽڵ�����1
		n--;
	}
	cout << fixed << setprecision(2) << (float)sumOfPath / sumOfWeight;
}