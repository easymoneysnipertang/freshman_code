#include<iostream>
#include<string>
#include<set>
#include<list>
#include <map>
#include<fstream>
#include<Windows.h>
using namespace std;

class Tokenization
{
public:
	set<string> dictionary;// �ʵ�

	// ͨ�����빹��ʵ�
	void constructDicByCin() {
		cout << "-----enter the total number of words in dictionary-----" << endl;
		int numOfTerms;
		cin >> numOfTerms;
		cout << "-----enter the words-----" << endl;
		for (int i = 0; i < numOfTerms; i++) {
			string temp;
			cin >> temp;
			dictionary.insert(temp);
		}
	}

	// ���ļ��ж�ȡ�ʵ�
	void constructDicByFile() {
		string str;
		ifstream infile("dic.txt");
		// ��ȡ�ļ�
		while (!infile.eof())
		{
			getline(infile, str);
			dictionary.insert(str);
		}
		infile.close();
	}
	
	// ��̬�滮�ִ�
	list<string> solve(string str) {
		// ���ȶ�̬�滮�ж��ܲ��������
		//auto OPT = vector <bool>(s.length() + 1);
		bool* OPT = new bool[str.length() + 1];// ��ʼ��
		for (int i = 1; i < str.length() + 1; i++)OPT[i] = false;
		OPT[0] = true;// �ַ�������Ϊ0�϶�û����

		for (int i = 1; i <= str.length(); i++) {
			// ǰi���ַ��ܷ�ִ�
			// ö�ٷָ��
			for (int j = 0; j < i; j++) {
				// ǰj���ַ����Էָ��j�������i�ֿ������һ������
				if (OPT[j] && dictionary.find(str.substr(j, i - j)) != dictionary.end()) {
					// ����¼OPT[j]��ʾ0��j-1���Էִʣ���s��ȡj��i���������ֵ�
					OPT[i] = true;// ���Էִ�
					break;
				}
			}
		}
		
		// �����������������Ľ�
		list<string> ans;
		if (OPT[str.length()]) {// �ַ����ܹ��ִ�
			list<string>stack;
			dfs(str, str.length(),OPT,stack, ans);
		}

		return ans;

	}
	// �������
	void dfs(string s, int length, bool* OPT,list<string>&stack,list<string>&ans) {
		if (length == 0) {
			// �����������ַ�����ȫ���ִʴ洢��stack��
			string partOfAns = "";
			string endStack = stack.back();// ��ǰ�ó����һ���������ӿո�
			stack.pop_back();
			// ƴ���ַ���
			for (string str : stack)
				partOfAns += str + " ";
			partOfAns += endStack;
			// �����һ���Ż�ջ��
			stack.push_back(endStack);
			// �õ�һ����
			ans.push_back(partOfAns);
			return;
		}
		// �Ӻ���ǰ���������ҵ�һ����׺����
		for (int i = length - 1; i >= 0; i--) {
			string suffix = s.substr(i, length - i);
			// i��ĩβ����һ�����ʣ���ǰi���ַ����Էִ�
			if (dictionary.find(suffix)!=dictionary.end() && OPT[i]) {
				stack.push_front(suffix);
				dfs(s, i, OPT, stack, ans);
				stack.pop_front();
			}
		}
	}

	// �Ż�����map�洢��strÿ���±꿪ʼ������ɵľ����б�
	// �ڻ���ʱ�����Ѿ����ʹ����±����ֱ�ӵý���������ظ�����
	list<string> solveOptimized(string str) {
		map<int, list<string>> ans;
		dfsWithOPT(str, 0, ans);
		return ans[0];
	}
	void dfsWithOPT(string str, int index, map<int, list <string >> &ans) {
		if (!ans.count(index)) {// ��ǰ�±�δ������
			if (index == str.length()) {// �Ѿ�����ĩβ
				//ans.insert(pair<int, list<string>>(index, {""}));
				ans[index] = { "" };
				return;
			}
			ans[index] = {};
			// �鿴��index��ʼ������������Щ����
			for (int i = index + 1; i <= str.length(); i++) {
				string temp = str.substr(index, i - index);
				// index��i���Թ���һ������
				if (dictionary.find(temp) != dictionary.end()) {
					dfsWithOPT(str, i,ans);// �鿴���滹�ܷ񹹳ɵ���
					for (string suffix : ans[i])
						// ����ǰ�������׺��ϣ����ɵ�ǰ�±����������ɵľ���
						ans[index].push_back(temp + " " + suffix);
				}
			}
		}
	}


};


void testCin() {
	Tokenization t;
	t.constructDicByCin();
	cout << "-----enter the str-----" << endl;
	string str;
	cin >> str;
	list<string> s = t.solveOptimized(str);
	cout << "-----the results as follows----" << endl;
	for (string ts : s)
		cout << ts << endl;
}


int main() {
	while (true) {
		// ����ѡ��
		cout << "-----enter your choice:-----" << endl;
		cout << "1.test by cin" << endl << "2.test by file"<<endl << "3.exit" << endl;
		int choice;
		cin >> choice;
		bool flag = false;
		// �����ֵ�
		Tokenization t;
		switch (choice)
		{
		case 1:
			t.constructDicByCin();
			break;
		case 2:
			t.constructDicByFile();
			break;
		case 3:
			flag = true;
			break;
		default:
			break;
		}
		// ѡ���˳�
		if (flag) {
			cout << "-----exit...-----" << endl;
			break;
		}
		// ������ִ��ַ���
		cout << "-----enter the str-----" << endl;
		string str;
		cin >> str;

		long long head, tail, freq;// ����ʱ��
		QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
		QueryPerformanceCounter((LARGE_INTEGER*)&head);// Start Time

		// ���ú����ִ�
		list<string> s = t.solve(str);
		cout << "-----the results as follows----" << endl;
		/*for (string ts : s)
			cout << ts << endl;*/

		QueryPerformanceCounter((LARGE_INTEGER*)&tail);// End Time
		printf("\ntime use: %f ms\n", (tail - head) * 1000.0 / freq);

		// ��һ�׶�ѭ��
		cout <<endl << "Go on? y/n" << endl;
		char ch;
		cin >> ch;
		if (ch == 'n') {
			cout << "-----exit...-----" << endl;
			break;
		}
		system("cls");
	}

	return 0;
}
