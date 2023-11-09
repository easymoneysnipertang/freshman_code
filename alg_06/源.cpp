#include<iostream>
#include<iomanip>
using namespace std;

struct point {
	int x;
	int y;
};
double segmentedLeastSquares(int n,int C, point* points) {
	double* M = new double[n+1];
	M[0] = 0;
	double** e = new double* [n+1];
	for (int i = 0; i <= n; i++) 
		*(e + i) = new double[n+1];
	
	// 为每个分段计算eij
	for (int i = 0; i < n; i++) {
		double sumY = 0, sumX = 0, sumX2 = 0, sumXY = 0;
		for (int j = i; j < n; j++) {
			int length = j - i + 1;
			sumY += points[j].y; sumX += points[j].x;
			sumXY += points[j].x * points[j].y; sumX2 += points[j].x * points[j].x;
			double a = (length * sumXY - sumX * sumY) / (length * sumX2 - sumX * sumX);
			double b = (sumY - a * sumX) / length;
			e[i + 1][j + 1] = 0;
			for (int k = i; k <= j; k++) {
				double temp = points[k].y - points[k].x * a - b;
				e[i + 1][j + 1] += temp * temp;
			}
		}
	}
	// 递归计算M[j]
	for (int j = 1; j <= n; j++) {
		M[j] = e[1][j] + C + M[0];
		for (int i = 2; i <= j; i++) {
			double temp = e[i][j] + C + M[i - 1];
			if (temp < M[j])M[j] = temp;
		}
	}
	return M[n];
}
int main() {
	int n, C;
	cin >> n >> C;
	point* points = new point[n];
	for (int i = 0; i < n; i++)
		cin >> points[i].x >> points[i].y;
	cout << fixed << setprecision(2) << segmentedLeastSquares(n, C, points);
	return 0;
}