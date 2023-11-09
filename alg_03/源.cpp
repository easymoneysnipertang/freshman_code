#include<iostream>
#include<algorithm>
#include<queue>
using namespace std;

//------�����------
struct edge {
	int v1;
	int v2;
	int weight;
};

//------kruskalʵ��------
bool cmp(edge a, edge b) {
	return a.weight < b.weight;
}
int findSet(int* set, int vertice) {
	if (set[vertice] == vertice)//���ڵ�
		return vertice;
	else
		return findSet(set, set[vertice]);//�Ǹ��ڵ㣬�������²�
}
void unionSet(int * set,int vertice,int newRoot) {
	if (set[vertice] == vertice)//���ڵ�
		set[vertice] = newRoot;
	else
		unionSet(set, set[vertice], newRoot);//�Ǹ��ڵ㣬�������²�
	//��һ���Ż��������·���ϵĽڵ㶼�޸���->�������ߣ�
	set[vertice] = newRoot;
}
int kruskal(edge* edges,int m,int n) {
	//------�߰�Ȩ����------
	sort(edges, edges + m, cmp);

	//------��ʼ��ÿ���ڵ���������------
	int* set;
	set = new int[n + 1];
	for (int i = 0; i <= n; i++)set[i] = i;

	//------ѭ�����ÿһ���߼���MST------
	int count = 0, sum = 0;
	while (count != m) {
		int vertice1 = edges[count].v1, vertice2 = edges[count].v2;
		int root1 = findSet(set, vertice1), root2 = findSet(set, vertice2);
		if (root1 != root2) {
			//���ڵ㲻����һ�����ϣ�������
			unionSet(set, vertice2, root1);
			//������ϸñߵ�Ȩ
			sum += edges[count].weight;
		}
		count++;
	}
	
	return sum;
}

//-----primʵ��------
bool operator <(edge a, edge b) {
	return a.weight > b.weight;//С����
}
int prim(edge *edges,int m,int n) {
	//------��ʼ��------
	int** graph;//�ڽӾ���
	bool* flag;//�������
	graph = new int* [n + 1];
	flag = new bool[n + 1];
	for (int i = 1; i <= n; i++) {
		*(graph + i) = new int[n + 1]();
		flag[i] = false;
	}

	//------���ߵ���Ϣ�����ڽӾ���------
	for (int i = 0; i < m; i++) {
		graph[edges[i].v1][edges[i].v2] = edges[i].weight;
		graph[edges[i].v2][edges[i].v1] = edges[i].weight;
	}

	//------�������ȶ��У���ѡ��һ����ʼ��------
	int count = 1, s=1, sum = 0;
	priority_queue<edge> queue;

	//------ѭ����prim�ó��ڵ�------
	while (count != n) {
		//����������
		flag[s] = true;
		for (int i = 1; i <= n; i++)//����ڽӵ㣬���������
			if (graph[s][i] != 0 && !flag[i]) {//�ڽӵ�û�б����ʹ�
				edge newEdge;
				newEdge.v1 = s;
				newEdge.v2 = i;
				newEdge.weight = graph[s][i];
				queue.push(newEdge);
			}
		while (1) {//�Ӷ��������ҳ�һ�����ʵĽڵ���Ϊs
			edge temp;
			temp = queue.top();
			queue.pop();
			s = temp.v2;
			if (!flag[s])
			{//�ҵ��˼���MST
				sum += temp.weight;
				count++;
				break;
			}
		}
	}
	return sum;

}

int main() {
	//-------��ʼ����------
	int n, m;
	cin >> n >> m;
	edge* edges;
	edges = new edge[m];//�������б�
	for (int i = 0; i < m; i++)
		cin >> edges[i].v1 >> edges[i].v2 >> edges[i].weight;
	
	//------���ú���------
	//int result = kruskal(edges, m, n);
	int result = prim(edges, m, n);
	cout << result;
	return 0;
}