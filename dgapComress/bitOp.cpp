#include <iostream>
#include<fstream>
#include <bitset>
#include <algorithm>
#include<sstream>
#include"bitOp.h"
using namespace std;

// ��nλ����д�뵽int��������
void writeBitData(vector<unsigned int>& array, unsigned int index, unsigned int data, int n)
{
    int shift = index % 32;
    int elementIndex = index / 32;
    int remainingBits = 32 - shift;
    unsigned mask = (long long)pow(2, n) - 1;
    if (remainingBits >= n) {
        // ��Խһ������Ԫ�ص����
        array[elementIndex] &= ~(mask << shift);  // �����Ӧλ������
        array[elementIndex] |= (data & mask) << shift;  // д��������
    }
    else {
        // ��Խ��������Ԫ�ص����
        int bitsInFirstElement = remainingBits;
        int bitsInSecondElement = n - remainingBits;

        array[elementIndex] &= ~(mask << shift);  // �����һ��Ԫ���е�����
        array[elementIndex] |= (data & ((1 << bitsInFirstElement) - 1)) << shift;  // д���һ��Ԫ�ص�����

        array[elementIndex + 1] &= ~((1 << bitsInSecondElement) - 1);  // ����ڶ���Ԫ���е�����
        array[elementIndex + 1] |= (data >> bitsInFirstElement);  // д��ڶ���Ԫ�ص�����
    }
}

// ��unsigned int�������ж�ȡnλ����
unsigned int readBitData(const vector<unsigned int>& array, unsigned int index, int n) {
    unsigned int element_index = index / 32;
    unsigned int bit_position = index % 32;

    unsigned int mask = (unsigned)((long long)pow(2, n) - 1);// ע�ⲻͬcpuȡ����һ��
    unsigned int data = (array[element_index] >> bit_position) & mask;

    // ���Ҫ��ȡ��nλ��Խ������������
    if (bit_position > 32 - n) {
        unsigned int next_element = array[element_index + 1];
        unsigned int remaining_bits = n - (32 - bit_position);
        data |= (next_element & ((1 << remaining_bits) - 1)) << (n - remaining_bits);
    }

    return data;
}

void getIndex(vector<vector<unsigned>>& invertedLists)
{
    //��ȡ�������ļ�
    fstream file;
    file.open("ExpIndex", ios::binary | ios::in);
    if (!file.is_open()) {
        cout << "Wrong in opening file!";
        return;
    }
    unsigned int maxdocId = 0;
    double avgLen = 0;
    for (int i = 0; i < 2000; i++)		//�ܹ���ȡ2000����������
    {
        vector<unsigned> t;				//һ����������
        int len;
        file.read((char*)&len, sizeof(len));
        avgLen += len;
        for (int j = 0; j < len; j++)
        {
            unsigned int docId;			//�ļ�id
            file.read((char*)&docId, sizeof(docId));
            t.push_back(docId);//���������ű�
            if (docId > maxdocId)
                maxdocId = docId;
        }
        sort(t.begin(), t.end());//���ĵ��������
        invertedLists.push_back(t);		//����һ�����ű�
    }
    cout << maxdocId << endl;
    cout << avgLen  << endl;
    file.close();

    cout << "here" << endl;
}

void getIndexOPT(vector<vector<unsigned>>& invertedLists) {
    vector<unsigned>data;
    //��ȡ�������ļ�
    fstream file;
    file.open("ExpIndex", ios::binary | ios::in);
    if (!file.is_open()) {
        cout << "Wrong in opening file!";
        return;
    }
    // ��ȡ�ļ���С
    file.seekg(0, ios::end);
    streampos fileSize = file.tellg();
    file.seekg(0, ios::beg);
    // ����������С
    size_t numElements = fileSize / sizeof(unsigned int);
    data.resize(numElements);
    // ���ļ���ȡ���ݵ�����
    file.read(reinterpret_cast<char*>(data.data()), fileSize);
    file.close();

    int index = 0;
    for (int i = 0; i < 2000; i++)		//�ܹ���ȡ2000����������
    {
        vector<unsigned> t;				//һ����������
        int len = data[index++]; //�Ȼ�ȡ����
        t.assign(data.begin() + index, data.begin() + index + len);
        sort(t.begin(), t.end());//���ĵ��������
        invertedLists.push_back(t);		//����һ�����ű�
        index += len;
    }
    cout << "here" << endl;
}

void vectorToBin(const vector<unsigned int>& data, const string& filename)
{
    // �򿪶������ļ�����д��
    ofstream file(filename, ios::binary);
    if (!file.is_open()) {
        cerr << "Failed to open the file: " << filename << endl;
        return;
    }

    // �������е�ÿ��Ԫ��д���ļ�
    for (const unsigned int& element : data) {
        file.write(reinterpret_cast<const char*>(&element), sizeof(unsigned int));
    }

    // �ر��ļ�
    file.close();
}

void binToVector(const string& filename, vector<unsigned int>& data)
{

    // �򿪶������ļ����ڶ�ȡ
    ifstream file(filename, ios::binary);
    if (!file.is_open()) {
        cerr << "Failed to open the file: " << filename << endl;
        return;
    }

    // ��ȡ�ļ���С
    file.seekg(0, ios::end);
    streampos fileSize = file.tellg();
    file.seekg(0, ios::beg);

    // ����������С
    size_t numElements = fileSize / sizeof(unsigned int);
    data.resize(numElements);

    // ���ļ���ȡ���ݵ�����
    file.read(reinterpret_cast<char*>(data.data()), fileSize);

    // �ر��ļ�
    file.close();

    return;
}