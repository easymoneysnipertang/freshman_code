#include<iostream>
#include<vector>
#include<algorithm>
#include<iomanip>
#include<math.h>

using namespace std;

struct Point {
	float x;
	float y;
};
// ������
bool cmpX(Point a, Point b) {
	return a.x < b.x;
}
bool cmpY(Point a, Point b) {
	return a.y < b.y;
}


float closestPairRec(vector<Point> Px) {
	if (Px.size() == 2) {
		// ���������㣬ֱ�ӷ���
		return pow((Px[0].x - Px[1].x), 2) + pow((Px[0].y - Px[1].y), 2);
	}
	else if (Px.size() == 3) {
		// �������㣬��Ҫ�ж�
		float dis1 = pow((Px[0].x - Px[1].x), 2) + pow((Px[0].y - Px[1].y), 2);
		float dis2 = pow((Px[0].x - Px[2].x), 2) + pow((Px[0].y - Px[2].y), 2);
		float dis3 = pow((Px[2].x - Px[1].x), 2) + pow((Px[2].y - Px[1].y), 2);
		if (dis1 <= dis2 && dis1 <= dis3)
			return dis1;
		if (dis2 <= dis1 && dis2 <= dis3)
			return dis2;
		if (dis3 <= dis1 && dis3 <= dis2)
			return dis3;
	}
	// ���������㣬����
	vector<Point>Qx, Rx;
	for (int i = 0; i < Px.size(); i++) {
		if (i < ceil(Px.size() / 2)) {
			// ��벿��
			Qx.push_back(Px[i]);
		}
		else {
			// �Ұ벿��
			Rx.push_back(Px[i]);
		}
	}

	// ���
	float ret1 = closestPairRec(Qx);
	float ret2 = closestPairRec(Rx);
	float minDis = min(ret1, ret2);
	float L = Qx[Qx.size() - 1].x;

	// ����խ��
	vector<Point> Sy;
	for (int i = Qx.size() - 1; i >= 0; i--) {
		if (L - Qx[i].x > minDis)break;
		Sy.push_back(Qx[i]);
	}
	for (int i = 0; i < Rx.size(); i++) {
		if (Rx[i].x - L > minDis)break;
		Sy.push_back(Rx[i]);
	}
	sort(Sy.begin(), Sy.end(), cmpY);// �����y��һ�������
	for (int i = 0; i < Sy.size(); i++) {
		for (int j = 1; j <= 12; j++) {
			if (i + j >= Sy.size())break;
			float dis = pow((Sy[i].x - Sy[i + j].x), 2) + pow((Sy[i].y-Sy[i + j].y), 2);
			if (dis < minDis)minDis = dis;
		}
	}
	return minDis;


}

void closestPair() {
	// ��ʼ������
	int n;
	cin >> n;
	Point* points;
	points = new Point[n];
	vector<Point> Px; 
	for (int i = 0; i < n; i++) {
		cin >> points[i].x >> points[i].y;
		// ����Px��Py
		Px.push_back(points[i]);
	}
	sort(Px.begin(), Px.end(), cmpX);
	// ��ȡ���ٽ����
	//pair<Point,Point> ret= closetPairRec(Px, Py);
	float ret = closestPairRec(Px);
	cout << fixed << setprecision(2) << ret;
}

int main() {
	closestPair();
	return 0;
}