#include<iostream>
#include<list>

using namespace std;

int main() {
	int n;
	cin >> n;

	list<int> man, *manPref;
	manPref = new list<int>[n+1];
	int** womanPref, ** reverse, * current, * currentMan;
	womanPref = new int* [n+1];
	reverse = new int* [n+1];
	current = new int[n+1];
	currentMan = new int[n + 1];

	for (int i = 1; i <= n; i++) {
		man.push_back(i);//�����˷���ջ��
		for (int j = 1; j <= n; j++) {//�����˵�ƫ���б�������
			int temp;
			cin >> temp;
			manPref[i].push_back(temp);
		}
	}
	for (int i = 1; i <= n; i++) {//����Ů�˵�ƫ���б�
		womanPref[i] = new int[n+1];
		reverse[i] = new int[n+1];
		for (int j = 1; j <= n; j++)
		{
			cin >> womanPref[i][j];
			reverse[i][womanPref[i][j]] = j;//Ԥ����
		}
		current[i] = 0;//Ůi��ǰƥ�����Ϊ0
		currentMan[i] = 0;
	}

	while (!man.empty()) {
		int manIndex = man.back();
		man.pop_back();//������ʿ����һ��
		int womanIndex = manPref[manIndex].front();//ѡһ��δ������������ߵ�Ů��
		manPref[manIndex].pop_front();

		if (current[womanIndex] == 0) {//woman���ɣ�����
			current[womanIndex] = manIndex;
			currentMan[manIndex] = womanIndex;
		}
		else {
			if (reverse[womanIndex][manIndex] > reverse[womanIndex][current[womanIndex]]) {
				//���ȼ��ڵ�ǰ������ʿ֮��
				man.push_back(manIndex);//�Ż�ջ��
			}
			else {
				man.push_back(current[womanIndex]);//�Ѷ�����ʿ�ָ�����
				currentMan[current[womanIndex]] = 0;
				current[womanIndex] = manIndex;//���˶���
				currentMan[manIndex] = womanIndex;
			}
		}
	}

	for (int i = 1; i <= n; i++) {
		cout << currentMan[i] << " ";
	}
	return 0;

}