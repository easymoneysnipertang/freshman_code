#include<iostream>
#include<string>
#include<math.h>
using namespace std;

int Alignment(string strX, string strY, int penalty) {
	// ��ʼ��
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
			// i��j���뷣��
			int penaltyIJ = abs(strX[i - 1] - strY[j - 1]) + OPT[i - 1][j-1];
			// j���϶����
			int penaltyI = penalty + OPT[i - 1][j];
			// i���϶����
			int penaltyJ = penalty + OPT[i][j-1];
			OPT[i][j] = min(penaltyIJ, min(penaltyI, penaltyJ));
		}
	}

	return OPT[m][n];
}

int spaceEfficientAlignment(string strX, string strY, int penalty) {
	// ��ʼ��
	int m = strX.length(), n = strY.length();
	int** OPT;
	OPT = new int* [m + 1];
	for (int i = 0; i < m + 1; i++) {
		*(OPT + i) = new int[2];
		OPT[i][0] = i * penalty;// i��0�����϶����
	}

	for (int j = 1; j <= n; j++) {
		OPT[0][1] = j * penalty;// j��0�����϶����
		// ����
		for (int i = 1; i <= m; i++){
			// i��j���뷣��
			int penaltyIJ = abs(strX[i - 1] - strY[j - 1]) + OPT[i - 1][0];
			// j���϶����
			int penaltyI = penalty + OPT[i - 1][1];
			// i���϶����
			int penaltyJ = penalty + OPT[i][0];
			OPT[i][1] = min(penaltyIJ, min(penaltyI, penaltyJ));
		}
		// Ϊ��һ�ε����ó��ռ�
		for (int i = 0; i < m + 1; i++)OPT[i][0] = OPT[i][1];
	}
	// OPT[i][1]������OPT(i,n)��ֵ
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