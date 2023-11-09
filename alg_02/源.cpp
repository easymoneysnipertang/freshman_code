#include<iostream>
#include<list>
using namespace std;

int main() {
	//-----初始化变量-----
	bool isDAG = true;
	int numberOfVertices, numberOfEdges;
	cin >> numberOfVertices >> numberOfEdges;//顶点数，边数

	int* entryTable = new int[numberOfVertices + 1];//入度表
	for (int i = 0; i <= numberOfVertices; i++)entryTable[i] = 0;//初始化
	
	list<int> freelist;//保存入度为0的节点
	list<int> outlist;//输出序列
	list<int>* graph = new list<int>[numberOfVertices+1];//用邻接链表表示图

	//-----输入边，构造邻接链表和入度表，将源挂入freelist-----
	for (int i = 0; i < numberOfEdges; i++) {//输入边
		int vertice1, vertice2;
		cin >> vertice1 >> vertice2;
		if (vertice1 == vertice2) { //有自环我都不用做后面的事了
			isDAG = false;
			graph[vertice1].push_front(vertice2);//自环放在第一个，一下就能找着
		}
		else graph[vertice1].push_back(vertice2);//连接边
		entryTable[vertice2]++;//入度加1
	}
	for (int i = 1; i <= numberOfVertices; i++)//添加源
		if (entryTable[i] == 0)freelist.push_back(i);

	//-----跑拓扑排序算法-----
	while (!freelist.empty()) {
		int currentVertice = freelist.front();//弹出一个源节点
		freelist.pop_front();
		outlist.push_back(currentVertice);//输出序列
		//遍历源节点的邻接节点
		for (list<int>::iterator it = graph[currentVertice].begin(); it != graph[currentVertice].end(); it++) {
			int tempvertice = *it;
			entryTable[tempvertice]--;//入度减1
			if (entryTable[tempvertice] == 0)//如果成为源就挂入freelist
				freelist.push_back(tempvertice);
		}
	}

	//------freelist空了，没有源了，看是否是dag-----
	list<int> cycle;//如果有环则肯定是从剩下的节点里面找
	for (int i = 1; i <= numberOfVertices; i++)
		if (entryTable[i] != 0) {
			isDAG = false;
			cycle.push_back(i);
		}
	
	//-----输出-----
	if (isDAG) {//是dag，则有拓扑排序
		cout << "YES" << endl;
		for (list<int>::iterator it = outlist.begin(); it != outlist.end(); ) {
			cout << *it;
			if (++it!= outlist.end())cout << ",";
		}
	}
	else {//不是dag，找一个环出来，那肯定在剩下的节点里面找
		outlist.clear();//输出序列是环的序列
		//dfs找环
		while (!cycle.empty()) {
			int currentVertice = cycle.front();
			cycle.pop_front();//弹出一个来尝试
			bool isFind = false;

			if (entryTable[currentVertice] >= 0) {//没有从其他节点访问到过该节点
				outlist.push_back(currentVertice);
				while (!outlist.empty()) {//从该节点出发，做dfs
					int tempVertice = outlist.back();
					if (entryTable[tempVertice] == -1) {//找到一条回路
						isFind = true;
						break;
					}
					if (graph[tempVertice].begin() != graph[tempVertice].end())
					{//该节点邻接表有东西
						outlist.push_back(graph[tempVertice].front());//把当前节点下一节点压入栈中
						//用掉即扔掉
						graph[tempVertice].pop_front();
						entryTable[tempVertice] = -1;//-1代表有从这经过的路
					}
					else {
						//没东西访问，回溯
						outlist.pop_back();
						entryTable[tempVertice] = -2;//走过，且没有回路
					}
				}
			}
			if (isFind) {//找到环路了
				cout << "NO" << endl;
				//去掉回路前面无用部分
				while (!outlist.empty()) {
					if (outlist.front() != outlist.back())
						outlist.pop_front();
					else break;
				}
				//输出
				for (list<int>::iterator it = outlist.begin(); it != outlist.end(); ) {
					cout << *it;
					if (++it != outlist.end())cout << ",";
				}
				break;
			}
		}
	}

}