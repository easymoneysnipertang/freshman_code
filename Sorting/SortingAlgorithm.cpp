#include "SortingAlgorithm.h"
#include<iostream>
using namespace std;


int main() {
    SortingAlgorithm s;
    int arr[] = { 61, 17, 29, 22, 34, 60, 72, 21, 50, 1, 62 };
    int len = (int)sizeof(arr) / sizeof(*arr);
    /*int* b;
    b = new int[len];*/
    s.quick_sort(arr,len);
    for (int i = 0; i < len; i++)
        cout << arr[i] << ' ';
    cout << endl;
    float arrf[] = { 17.5, 19.1, 0.6, 1.9, 10.5, 12.4, 3.8, 19.7, 1.5, 25.4, 28.6, 4.4, 23.8, 5.4 };
    len = (float)sizeof(arrf) / sizeof(*arrf);
   /* float* bf;
    bf = new float[len];*/
    s.quick_sort(arrf,len);
    for (int i = 0; i < len; i++)
        cout << arrf[i] << ' ' << endl;
    return 0;
}