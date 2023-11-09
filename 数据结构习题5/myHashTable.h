#pragma once
#include<iostream>
#include<string>
using namespace std;

#define INTMAX 2147483646

class myHashTable {
public:
    myHashTable(int theDivisor);
    ~myHashTable() { 
        for (int i = 0; i < divisor; i++)
            if (table[i] != nullptr)delete table[i];
        delete[] table; 
    }

    bool empty() const { return dSize == 0; }
    int size() const { return dSize; }
    string* find(const string&);
    void insert(const string&);
    void output();

    int search(const string&);
    int squaSearch(const string&);
    unsigned int hash(const string&);

    string** table;          
    int dSize; 
    int divisor;

    int searchCount;
    int successCount;
    int failCount;
    long long sum1;
    long long sum2;
};
myHashTable::myHashTable(int theDivisor)
{
    dSize = 0;
    table = new string * [theDivisor];//todo:����Ч�����
    for (int i = 0; i < theDivisor; i++)
        table[i] = nullptr;
    divisor = theDivisor;
    searchCount = 0;
    successCount = failCount = sum1 = sum2 = 0;
}

unsigned int myHashTable::hash(const string& str)
{
    //���ַ���ת��Ϊһ������
    unsigned long long hashValue = 0;
    int length = str.length();
    for (int i = 0; i < length; i++)
    {
        hashValue = 31 * hashValue + str[i];
        hashValue %= INTMAX;
    }
    return hashValue;
}


string* myHashTable::find(const string& str)
{
    searchCount = 0;
    int b = search(str);
    //int b = squaSearch(str);
    //���Ƿ�ƥ��table[b]

    if (table[b] == nullptr || (*table[b]).compare(str) != 0)
    {
        failCount++;
        sum2 += searchCount;
        return nullptr;//��ƥ��
    }

    successCount++;
    sum1 += searchCount;
    return table[b];//ƥ��
}

void myHashTable::insert(const string& str)
{
    int b = search(str);
    //int b = squaSearch(str);
    //�鿴�Ƿ���ƥ���
    if (table[b] == nullptr)
    {
        //��δ���Ѵ��ڵ��ұ�δ��
        table[b] = new string(str);
        dSize++;
    }
    else
    {//����Ƿ�����
        if ((*table[b]).compare(str) == 0)
        {//�Ѵ���
            *table[b] = str;
            //cout << "duplicate"<<endl;
        }
        else //������
            cout << "hashTable is full!" << endl;
    }
}

void myHashTable::output()
{
    int count = 0;
    for (int i = 0; i < divisor; i++)
        if (table[i] != nullptr) {
            cout << *table[i] << endl;
            count++;
        }
       /* else
            cout << "NULL" << endl;*/
    cout << "number of str: " << count << endl;
}

int myHashTable::search(const string& str)
{

    int i = (int)hash(str) % divisor;//��ʼλ��
    int j = i;
    do
    {
        searchCount++;
        if (table[j] == nullptr || (*table[j]).compare(str)==0)
            return j;//�ҵ�����λ��
        j = (j + 1) % divisor;//��һ��λ��
    } while (j != i);//�ж��Ƿ�ص����

    return j;//������
}

inline int myHashTable::squaSearch(const string& str)
{
    int i = (int)hash(str) % divisor;
    int k = i;
    bool flag = 1;
    for (int j = 1; j <= sqrt(divisor); j++) {
        searchCount++;
        if (table[k] == nullptr || (*table[k]).compare(str) == 0)
            return k;
        if (flag)
        {
            k = (i + j * j) % divisor;
            j--;
            flag = 0;
        }
        else
        {
            k = (i - j * j) % divisor;
            flag = 1;
        }
    }
}

