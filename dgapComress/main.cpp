#include"bitOp.h"
#include"dgap.h"
#include<windows.h>
#include<iostream>
#include<bitset>
using namespace std;

int main()
{
    //---------------------------------------��ȡ---------------------------------------
    vector<vector<unsigned>> invertedLists;//��ȡ������
    vector<vector<unsigned>> decompressed;//��ѹ�������Ӧ�õ���invertedLists

    //��ʱ
    long long head, tail, freq;
    QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
    QueryPerformanceCounter((LARGE_INTEGER*)&head);// Start Time

    getIndex(invertedLists);//��ȡ��������

    //��ʱ��ֹ
    QueryPerformanceCounter((LARGE_INTEGER*)&tail);// End Time
    cout << "Read(orignal) Time:" << (tail - head) * 1000.0 / freq << "ms" << endl;

    vector<unsigned> compressedRes;//ѹ�����
    vector<unsigned> compressed; //��������ѹ������
    vector<unsigned> curList;//��ǰ��ѹ������

    //---------------------------------------ѹ��---------------------------------------------
    //int idx = 0;
    //for (int i = 0; i < invertedLists.size(); i++) // ��ÿ���������ѹ�����浽һ��vector<unsigned> compressedRes��
    //    dgapCompress(invertedLists[i], compressedRes, idx, idx);

    //vectorToBin(compressedRes, "compress.bin");

    //---------------------------------------��ѹ---------------------------------------------

    //��ʱ
    //QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
    //QueryPerformanceCounter((LARGE_INTEGER*)&head);// Start Time
    ////�ӵ�0��bit��ʼ�����ݣ���ѹ
    //int curIdx = 0;
    //binToVector("compress.bin", compressed);//��ȡ��compressed�У�Ӧ��compressed����compressedRes

    //QueryPerformanceCounter((LARGE_INTEGER*)&tail);// End Time
    //cout << "Read Compressed File Time:" << (tail - head) * 1000.0 / freq << "ms" << endl;

    //dgapDecompressAll(compressed, decompressed);

    ////��ʱ��ֹ
    //QueryPerformanceCounter((LARGE_INTEGER*)&tail);// End Time
    //cout << "Decompresss Time:" << (tail - head) * 1000.0 / freq << "ms" << endl;
    return 0;
}