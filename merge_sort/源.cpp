#include<iostream>
#include<fstream>
#include<string>
#include<algorithm>
#include<iomanip>
using namespace std;

void test_txt() {
	string filename = "sort";
	srand((unsigned)time(NULL));
	for (int i = 0; i < 5; i++) {//д��ԭʼ����
		ofstream outfile;
		outfile.open(filename + to_string(i + 1) + ".txt");
		for (int j = 0; j < 100; j++) {
			int num = rand() % 1000;
			outfile << num << endl;
		}
		outfile.close();
	}

	for (int i = 0; i < 5; i++) {
		fstream file;
		file.open(filename + to_string(i + 1) + ".txt");//���ļ�
		int num[100];
		for (int j = 0; j < 100; j++) {
			file >> num[j];
		}//��������
		sort(num, num + 100);

		file.seekp(0);//����ָ��
		for (int j = 0; j < 100; j++) {//����д��
			file << num[j] << endl;
		}
		file.close();
	}

	ifstream infile[5];
	ofstream sortedout("sortedfile.txt");
	int num[5], numcount[5];
	for (int i = 0; i < 5; i++)//�ٴ�����ļ�
	{
		infile[i].open("sort" + to_string(i + 1) + ".txt");

		infile[i] >> num[i];//������һ��
		numcount[i] = 99;
	}

	int filecount = 5, min_index, min;
	while (filecount > 0) {//ѭ������
		min = 1000;
		for (int i = 0; i < 5; i++) {
			if (num[i] < min) {
				min = num[i];
				min_index = i;//�ҳ���С��λ��
			}
		}
		if (numcount[min_index] > 0) {
			infile[min_index] >> num[min_index];
			numcount[min_index]--;
		}
		else {//��ֹ����Ϲ��
			num[min_index] = 1001;
			filecount--;//�Ѿ�����һ���ļ�
			infile[min_index].close();
		}

		sortedout << min << endl;//д��
	}
	sortedout.close();

	ifstream sortedin("sortedfile.txt");//���������ļ�
	int tempt, count = 0;
	while (count++ < 500) {
		sortedin >> tempt;
		cout << setw(6) << tempt;
		if (count % 5 == 0 && count != 500) {
			cout << endl;
		}
	}
	sortedin.close();
}

void test_bin() {

	srand((unsigned)time(NULL));
	for (int i = 0; i < 5; i++) {//����ԭʼ����
		ofstream outfile("sort" + to_string(i + 1) + ".bin", ios::binary);
		for (int j = 0; j < 100; j++) {
			int num = rand() % 1000;
			outfile.write((char*)(&num), sizeof(int));
			
		}
		outfile.close();
	}

	//for (int i = 0; i < 5; i++) {//why?
	//	fstream file("sort" + to_string(i + 1) + ".bin", ios::binary);
	//	int num[100];
	//	for (int j = 0; j < 100; j++) {
	//		file.read((char*)(&num[j]), sizeof(int));

	//		cout << num[j] << endl;
	//	}//��������
	//	sort(num, num + 100);
	//	file.seekp(0);

	//	for (int j = 0; j < 100; j++) {//����д��
	//		file.write((char*)(&num[j]), sizeof(int));		
	//	}
	//	file.close();
	//}


	for (int i = 0; i < 5; i++) {
		ifstream ifile("sort" + to_string(i + 1) + ".bin", ios::binary);
		int num[100];
		for (int j = 0; j < 100; j++) {
			ifile.read((char*)(&num[j]), sizeof(int));			
		}//��������
		sort(num, num + 100);
		ifile.close();

		ofstream ofile("sort" + to_string(i + 1) + ".bin", ios::binary);

		for (int j = 0; j < 100; j++) {//����д��
			ofile.write((char*)(&num[j]), sizeof(int));		
		}
		ofile.close();
	}

	ifstream infile[5];
	ofstream sortedout("sortedfile.bin",ios::binary);
	int num[5], numcount[5];
	for (int i = 0; i < 5; i++)//�ٴ�����ļ�
	{
		infile[i].open("sort" + to_string(i + 1) + ".bin",ios::binary);

		infile[i].read((char*)(&num[i]), sizeof(int));
		numcount[i] = 99;
	}

	int filecount = 5, min_index, min;
	while (filecount > 0) {//ѭ������
		min = 1000;
		for (int i = 0; i < 5; i++) {
			if (num[i] < min) {
				min = num[i];
				min_index = i;//�ҳ���С��λ��
			}
		}
		if (numcount[min_index] > 0) {
			infile[min_index].read((char*)(&num[min_index]),sizeof(int));
			numcount[min_index]--;
		}
		else {//��ֹ����Ϲ��
			num[min_index] = 1001;
			filecount--;//�Ѿ�����һ���ļ�
			infile[min_index].close();
		}

		sortedout.write((char*)(&min), sizeof(int));//д��
	}
	sortedout.close();

	ifstream sortedin("sortedfile.bin",ios::binary);//���������ļ�
	int tempt, count = 0;
	while (count++ < 500) {
		sortedin.read((char*)(&tempt),sizeof(int));
		cout << setw(6) << tempt;
		if (count % 5 == 0 && count != 500) {
			cout << endl;
		}
	}
	sortedin.close();
}

int main() {

	//test_txt();
	test_bin();

	return 0;
}