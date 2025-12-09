

#ifndef TIMSORT_TIMSORT_H
#define TIMSORT_TIMSORT_H

#include <iostream>

using namespace std;

void timSort(int arr[], int n);
void printArray(int arr[], int n);
void mergeGallop(int arr[], int start, int mid, int end);
void mergeSort(int arr[], int start, int end);

#endif //TIMSORT_TIMSORT_H
