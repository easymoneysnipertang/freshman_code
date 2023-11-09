#include<iostream>
#include<algorithm>
#include<queue>
using namespace std;

//------定义边------
struct edge {
	int v1;
	int v2;
	int weight;
};

//------kruskal实现------
bool cmp(edge a, edge b) {
	return a.weight < b.weight;
}
int findSet(int* set, int vertice) {
	if (set[vertice] == vertice)//根节点
		return vertice;
	else
		return findSet(set, set[vertice]);//非根节点，继续向下查
}
void unionSet(int * set,int vertice,int newRoot) {
	if (set[vertice] == vertice)//根节点
		set[vertice] = newRoot;
	else
		unionSet(set, set[vertice], newRoot);//非根节点，继续向下查
	//进一步优化，并完后将路径上的节点都修改了->降低树高！
	set[vertice] = newRoot;
}
int kruskal(edge* edges,int m,int n) {
	//------边按权排序------
	sort(edges, edges + m, cmp);

	//------初始化每个节点所属集合------
	int* set;
	set = new int[n + 1];
	for (int i = 0; i <= n; i++)set[i] = i;

	//------循环检查每一个边加入MST------
	int count = 0, sum = 0;
	while (count != m) {
		int vertice1 = edges[count].v1, vertice2 = edges[count].v2;
		int root1 = findSet(set, vertice1), root2 = findSet(set, vertice2);
		if (root1 != root2) {
			//两节点不属于一个集合，并起来
			unionSet(set, vertice2, root1);
			//结果加上该边的权
			sum += edges[count].weight;
		}
		count++;
	}
	
	return sum;
}

//-----prim实现------
bool operator <(edge a, edge b) {
	return a.weight > b.weight;//小根堆
}
int prim(edge *edges,int m,int n) {
	//------初始化------
	int** graph;//邻接矩阵
	bool* flag;//标记数组
	graph = new int* [n + 1];
	flag = new bool[n + 1];
	for (int i = 1; i <= n; i++) {
		*(graph + i) = new int[n + 1]();
		flag[i] = false;
	}

	//------将边的信息存入邻接矩阵------
	for (int i = 0; i < m; i++) {
		graph[edges[i].v1][edges[i].v2] = edges[i].weight;
		graph[edges[i].v2][edges[i].v1] = edges[i].weight;
	}

	//------构造优先队列，并选择一个起始点------
	int count = 1, s=1, sum = 0;
	priority_queue<edge> queue;

	//------循环按prim拿出节点------
	while (count != n) {
		//这样更优美
		flag[s] = true;
		for (int i = 1; i <= n; i++)//检查邻接点，将边入队列
			if (graph[s][i] != 0 && !flag[i]) {//邻接点没有被访问过
				edge newEdge;
				newEdge.v1 = s;
				newEdge.v2 = i;
				newEdge.weight = graph[s][i];
				queue.push(newEdge);
			}
		while (1) {//从队列里面找出一个合适的节点作为s
			edge temp;
			temp = queue.top();
			queue.pop();
			s = temp.v2;
			if (!flag[s])
			{//找到了加入MST
				sum += temp.weight;
				count++;
				break;
			}
		}
	}
	return sum;

}

int main() {
	//-------初始输入------
	int n, m;
	cin >> n >> m;
	edge* edges;
	edges = new edge[m];//保存所有边
	for (int i = 0; i < m; i++)
		cin >> edges[i].v1 >> edges[i].v2 >> edges[i].weight;
	
	//------调用函数------
	//int result = kruskal(edges, m, n);
	int result = prim(edges, m, n);
	cout << result;
	return 0;
}