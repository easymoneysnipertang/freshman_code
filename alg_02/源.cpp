#include<iostream>
#include<list>
using namespace std;

int main() {
	//-----��ʼ������-----
	bool isDAG = true;
	int numberOfVertices, numberOfEdges;
	cin >> numberOfVertices >> numberOfEdges;//������������

	int* entryTable = new int[numberOfVertices + 1];//��ȱ�
	for (int i = 0; i <= numberOfVertices; i++)entryTable[i] = 0;//��ʼ��
	
	list<int> freelist;//�������Ϊ0�Ľڵ�
	list<int> outlist;//�������
	list<int>* graph = new list<int>[numberOfVertices+1];//���ڽ������ʾͼ

	//-----����ߣ������ڽ��������ȱ���Դ����freelist-----
	for (int i = 0; i < numberOfEdges; i++) {//�����
		int vertice1, vertice2;
		cin >> vertice1 >> vertice2;
		if (vertice1 == vertice2) { //���Ի��Ҷ����������������
			isDAG = false;
			graph[vertice1].push_front(vertice2);//�Ի����ڵ�һ����һ�¾�������
		}
		else graph[vertice1].push_back(vertice2);//���ӱ�
		entryTable[vertice2]++;//��ȼ�1
	}
	for (int i = 1; i <= numberOfVertices; i++)//���Դ
		if (entryTable[i] == 0)freelist.push_back(i);

	//-----�����������㷨-----
	while (!freelist.empty()) {
		int currentVertice = freelist.front();//����һ��Դ�ڵ�
		freelist.pop_front();
		outlist.push_back(currentVertice);//�������
		//����Դ�ڵ���ڽӽڵ�
		for (list<int>::iterator it = graph[currentVertice].begin(); it != graph[currentVertice].end(); it++) {
			int tempvertice = *it;
			entryTable[tempvertice]--;//��ȼ�1
			if (entryTable[tempvertice] == 0)//�����ΪԴ�͹���freelist
				freelist.push_back(tempvertice);
		}
	}

	//------freelist���ˣ�û��Դ�ˣ����Ƿ���dag-----
	list<int> cycle;//����л���϶��Ǵ�ʣ�µĽڵ�������
	for (int i = 1; i <= numberOfVertices; i++)
		if (entryTable[i] != 0) {
			isDAG = false;
			cycle.push_back(i);
		}
	
	//-----���-----
	if (isDAG) {//��dag��������������
		cout << "YES" << endl;
		for (list<int>::iterator it = outlist.begin(); it != outlist.end(); ) {
			cout << *it;
			if (++it!= outlist.end())cout << ",";
		}
	}
	else {//����dag����һ�����������ǿ϶���ʣ�µĽڵ�������
		outlist.clear();//��������ǻ�������
		//dfs�һ�
		while (!cycle.empty()) {
			int currentVertice = cycle.front();
			cycle.pop_front();//����һ��������
			bool isFind = false;

			if (entryTable[currentVertice] >= 0) {//û�д������ڵ���ʵ����ýڵ�
				outlist.push_back(currentVertice);
				while (!outlist.empty()) {//�Ӹýڵ��������dfs
					int tempVertice = outlist.back();
					if (entryTable[tempVertice] == -1) {//�ҵ�һ����·
						isFind = true;
						break;
					}
					if (graph[tempVertice].begin() != graph[tempVertice].end())
					{//�ýڵ��ڽӱ��ж���
						outlist.push_back(graph[tempVertice].front());//�ѵ�ǰ�ڵ���һ�ڵ�ѹ��ջ��
						//�õ����ӵ�
						graph[tempVertice].pop_front();
						entryTable[tempVertice] = -1;//-1�����д��⾭����·
					}
					else {
						//û�������ʣ�����
						outlist.pop_back();
						entryTable[tempVertice] = -2;//�߹�����û�л�·
					}
				}
			}
			if (isFind) {//�ҵ���·��
				cout << "NO" << endl;
				//ȥ����·ǰ�����ò���
				while (!outlist.empty()) {
					if (outlist.front() != outlist.back())
						outlist.pop_front();
					else break;
				}
				//���
				for (list<int>::iterator it = outlist.begin(); it != outlist.end(); ) {
					cout << *it;
					if (++it != outlist.end())cout << ",";
				}
				break;
			}
		}
	}

}