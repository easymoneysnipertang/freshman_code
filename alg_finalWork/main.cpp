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
	set<string> dictionary;// 词典

	// 通过输入构造词典
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

	// 从文件中读取词典
	void constructDicByFile() {
		string str;
		ifstream infile("dic.txt");
		// 读取文件
		while (!infile.eof())
		{
			getline(infile, str);
			dictionary.insert(str);
		}
		infile.close();
	}
	
	// 动态规划分词
	list<string> solve(string str) {
		// 首先动态规划判断能不能做拆分
		//auto OPT = vector <bool>(s.length() + 1);
		bool* OPT = new bool[str.length() + 1];// 初始化
		for (int i = 1; i < str.length() + 1; i++)OPT[i] = false;
		OPT[0] = true;// 字符串长度为0肯定没问题

		for (int i = 1; i <= str.length(); i++) {
			// 前i个字符能否分词
			// 枚举分割点
			for (int j = 0; j < i; j++) {
				// 前j个字符可以分割，从j往后加上i又可以组成一个单词
				if (OPT[j] && dictionary.find(str.substr(j, i - j)) != dictionary.end()) {
					// 备忘录OPT[j]表示0到j-1可以分词，而s截取j到i出现在了字典
					OPT[i] = true;// 可以分词
					break;
				}
			}
		}
		
		// 回溯搜索符合条件的解
		list<string> ans;
		if (OPT[str.length()]) {// 字符串能够分词
			list<string>stack;
			dfs(str, str.length(),OPT,stack, ans);
		}

		return ans;

	}
	// 深搜求解
	void dfs(string s, int length, bool* OPT,list<string>&stack,list<string>&ans) {
		if (length == 0) {
			// 找完了整个字符串，全部分词存储在stack里
			string partOfAns = "";
			string endStack = stack.back();// 提前拿出最后一个，避免多加空格
			stack.pop_back();
			// 拼接字符串
			for (string str : stack)
				partOfAns += str + " ";
			partOfAns += endStack;
			// 把最后一个放回栈中
			stack.push_back(endStack);
			// 得到一个解
			ans.push_back(partOfAns);
			return;
		}
		// 从后往前搜索，先找到一个后缀单词
		for (int i = length - 1; i >= 0; i--) {
			string suffix = s.substr(i, length - i);
			// i到末尾构成一个单词，且前i个字符可以分词
			if (dictionary.find(suffix)!=dictionary.end() && OPT[i]) {
				stack.push_front(suffix);
				dfs(s, i, OPT, stack, ans);
				stack.pop_front();
			}
		}
	}

	// 优化，用map存储从str每个下标开始可以组成的句子列表
	// 在回溯时遇到已经访问过的下标则可直接得结果，无需重复计算
	list<string> solveOptimized(string str) {
		map<int, list<string>> ans;
		dfsWithOPT(str, 0, ans);
		return ans[0];
	}
	void dfsWithOPT(string str, int index, map<int, list <string >> &ans) {
		if (!ans.count(index)) {// 当前下标未搜索过
			if (index == str.length()) {// 已经到了末尾
				//ans.insert(pair<int, list<string>>(index, {""}));
				ans[index] = { "" };
				return;
			}
			ans[index] = {};
			// 查看从index开始往后可以组成哪些句子
			for (int i = index + 1; i <= str.length(); i++) {
				string temp = str.substr(index, i - index);
				// index至i可以构成一个单词
				if (dictionary.find(temp) != dictionary.end()) {
					dfsWithOPT(str, i,ans);// 查看后面还能否构成单词
					for (string suffix : ans[i])
						// 将当前单词与后缀组合，构成当前下标往后可以组成的句子
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
		// 接受选择
		cout << "-----enter your choice:-----" << endl;
		cout << "1.test by cin" << endl << "2.test by file"<<endl << "3.exit" << endl;
		int choice;
		cin >> choice;
		bool flag = false;
		// 构造字典
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
		// 选择退出
		if (flag) {
			cout << "-----exit...-----" << endl;
			break;
		}
		// 输入待分词字符串
		cout << "-----enter the str-----" << endl;
		string str;
		cin >> str;

		long long head, tail, freq;// 测试时间
		QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
		QueryPerformanceCounter((LARGE_INTEGER*)&head);// Start Time

		// 调用函数分词
		list<string> s = t.solve(str);
		cout << "-----the results as follows----" << endl;
		/*for (string ts : s)
			cout << ts << endl;*/

		QueryPerformanceCounter((LARGE_INTEGER*)&tail);// End Time
		printf("\ntime use: %f ms\n", (tail - head) * 1000.0 / freq);

		// 下一阶段循环
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
