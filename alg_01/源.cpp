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
		man.push_back(i);//将男人放入栈中
		for (int j = 1; j <= n; j++) {//将男人的偏好列表挂入队列
			int temp;
			cin >> temp;
			manPref[i].push_back(temp);
		}
	}
	for (int i = 1; i <= n; i++) {//接受女人的偏好列表
		womanPref[i] = new int[n+1];
		reverse[i] = new int[n+1];
		for (int j = 1; j <= n; j++)
		{
			cin >> womanPref[i][j];
			reverse[i][womanPref[i][j]] = j;//预处理
		}
		current[i] = 0;//女i当前匹配对象为0
		currentMan[i] = 0;
	}

	while (!man.empty()) {
		int manIndex = man.back();
		man.pop_back();//待查男士弹出一个
		int womanIndex = manPref[manIndex].front();//选一个未求过婚的排名最高的女人
		manPref[manIndex].pop_front();

		if (current[womanIndex] == 0) {//woman自由，订婚
			current[womanIndex] = manIndex;
			currentMan[manIndex] = womanIndex;
		}
		else {
			if (reverse[womanIndex][manIndex] > reverse[womanIndex][current[womanIndex]]) {
				//优先级在当前订婚男士之后
				man.push_back(manIndex);//放回栈中
			}
			else {
				man.push_back(current[womanIndex]);//已订婚男士恢复自由
				currentMan[current[womanIndex]] = 0;
				current[womanIndex] = manIndex;//两人订婚
				currentMan[manIndex] = womanIndex;
			}
		}
	}

	for (int i = 1; i <= n; i++) {
		cout << currentMan[i] << " ";
	}
	return 0;

}