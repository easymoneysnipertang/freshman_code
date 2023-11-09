#include<iostream>
#include<string>
#include<math.h>
using namespace std;

int Alignment(string strX, string strY, int penalty) {
	// 初始化
	int m = strX.length(), n = strY.length();
	int** OPT;
	OPT = new int* [m + 1];
	for (int i = 0; i < m + 1; i++) {
		*(OPT + i) = new int[n + 1];
		OPT[i][0] = i * penalty;
	}
	for (int j = 0; j < n + 1; j++)OPT[0][j] = j * penalty;

	for (int j = 1; j <= n; j++) {
		for (int i = 1; i <= m; i++) {
			// i与j对齐罚分
			int penaltyIJ = abs(strX[i - 1] - strY[j - 1]) + OPT[i - 1][j-1];
			// j与空隙对齐
			int penaltyI = penalty + OPT[i - 1][j];
			// i与空隙对齐
			int penaltyJ = penalty + OPT[i][j-1];
			OPT[i][j] = min(penaltyIJ, min(penaltyI, penaltyJ));
		}
	}

	return OPT[m][n];
}

int spaceEfficientAlignment(string strX, string strY, int penalty) {
	// 初始化
	int m = strX.length(), n = strY.length();
	int** OPT;
	OPT = new int* [m + 1];
	for (int i = 0; i < m + 1; i++) {
		*(OPT + i) = new int[2];
		OPT[i][0] = i * penalty;// i与0对齐空隙罚分
	}

	for (int j = 1; j <= n; j++) {
		OPT[0][1] = j * penalty;// j与0对齐空隙罚分
		// 迭代
		for (int i = 1; i <= m; i++){
			// i与j对齐罚分
			int penaltyIJ = abs(strX[i - 1] - strY[j - 1]) + OPT[i - 1][0];
			// j与空隙对齐
			int penaltyI = penalty + OPT[i - 1][1];
			// i与空隙对齐
			int penaltyJ = penalty + OPT[i][0];
			OPT[i][1] = min(penaltyIJ, min(penaltyI, penaltyJ));
		}
		// 为下一次迭代让出空间
		for (int i = 0; i < m + 1; i++)OPT[i][0] = OPT[i][1];
	}
	// OPT[i][1]保存了OPT(i,n)的值
	return OPT[m][1];
}

int main() {
	string strA, strB;
	cin >> strA >> strB;
	int penaltyK;
	cin >> penaltyK;
	cout << Alignment(strA, strB, penaltyK) << endl;
	return 0;
}