#include "timsort.h"


// считает minRun для чисел больших 64
int calcMinRun(int n) {
    int r = 0;
    while (n >= 64) {
        // учитываем остаток если n не было степенью двойки
        r |= (n & 1);
        // побитовый свиг n так чтобы оно стало степенью двойки но было меньше 64
        n >>= 1;
    }
    return n + r;
}

// вспомогающая функция чтобы разворачивать уже отсортированный по убыванию массив
void reverse(int arr[], int start, int end) {
    // идем до середины
    for (int i = 0; i < (end - start) / 2; i++) {
        int t = arr[start + i];
        arr[start + i] = arr[end-i-1];
        arr[end-i-1] = t;
    }
}

// считаем длинну run и если он убывает то разворачиваем
int runLenAndMakeAsc(int arr[], int n, int start) {
    if (start == n-1) return 1;
    int runEnd = start + 1;
    if (arr[runEnd] < arr[start]) {
        while (runEnd < n && arr[runEnd] < arr[runEnd-1]) runEnd++;
        reverse(arr, start, runEnd);
    } else {
        while (runEnd < n && arr[runEnd] >= arr[runEnd-1]) runEnd++;
    }
    return runEnd - start;
}

// сортировка вставками
void insertionSort(int arr[], int left, int right) {
    int v, i, j;
    for (i = left + 1; i < right; i++) {
        v = arr[i];
        j = i - 1;

        while (j >= left && arr[j] > v) {
            arr[j+1] = arr[j];
            j = j - 1;
        }
        arr[j+1] = v;
    }
}



struct Run {
    int start;
    int len;
};

Run runStack[128];
int sizeStack = 0;

void pushRun(int start, int len) {
    runStack[sizeStack].start = start;
    runStack[sizeStack].len = len;
    sizeStack++;
}



int gallopSearch(int arr[], int start, int end, int key) {
    int jump = 1;

    while (start + jump < end && arr[start + jump] < key) jump *= 2;

    int left = start + jump / 2;
    int right = min(end, start + jump);

    while (left < right) {
        int mid = left + (right - left) / 2;
        if (arr[mid] < key) left = mid + 1;
        else right = mid;
    }

    return left;
}

void mergeGallop(int arr[], int start, int mid, int end) {
    int n1 = mid - start;
    int n2 = end - mid;

    int* left = new int[n1];
    int* right = new int[n2];

    for (int i = 0; i < n1; i++) left[i] = arr[start + i];
    for (int i = 0; i < n2; i++) right[i] = arr[mid + i];

    int l = 0, r = 0, a = start;
    static int minGallop = 7;

    while (l < n1 && r < n2) {
        int leftWins = 0, rightWins = 0;

        while (l < n1 && r < n2) {
            if (left[l] <= right[r]) {
                arr[a++] = left[l++];
                leftWins++;
                rightWins = 0;
            }
            else {
                arr[a++] = right[r++];
                rightWins++;
                leftWins = 0;
            }

            if ((leftWins >= minGallop) || (rightWins >= minGallop)) break;
        }

        if (leftWins >= minGallop && r < n2) {
            // найдем в left первую позицию > right[r]
            int pos = gallopSearch(left, l, n1, right[r]);
            int cnt = pos - l;

            if (cnt > 0)
                for (int i = 0; i < cnt; ++i)
                    arr[a++] = left[l++];

            // адаптация порога: успешный gallop -> уменьшаем minGallop
            if (cnt >= minGallop) minGallop = max(1, minGallop - 1);
            else minGallop++;
        } else if (rightWins >= minGallop && l < n1) {
            int pos = gallopSearch(right, r, n2, left[l]);
            int cnt = pos - r;

            if (cnt > 0)
                for (int i = 0; i < cnt; ++i)
                    arr[a++] = right[r++];

            if (cnt >= minGallop) minGallop = max(1, minGallop - 1);
            else minGallop++;
        }
    }


    while (l < n1) arr[a++] = left[l++];
    while (r < n2) arr[a++] = right[r++];

    delete[] left;
    delete[] right;

}

void mergeSort(int arr[], int start, int end) {
    if (end - start <= 1) return;
    int mid = start + (end - start) / 2;

    mergeSort(arr, start, mid);
    mergeSort(arr, mid, end);

    mergeGallop(arr, start, mid, end);
}

void mergeAt(int arr[], int index) {
    Run left = runStack[index];
    Run right = runStack[index+1];

    int start = left.start;
    int mid = right.start;
    int end = right.start + right.len;

    mergeGallop(arr, start, mid, end);

    runStack[index].start = start;
    runStack[index].len = left.len + right.len;

    for (int i = index + 1; i + 1 < sizeStack; ++i) runStack[i] = runStack[i+1];
    sizeStack--;
}

void mergeTop(int arr[]) {
    if (sizeStack < 2) return;
    mergeAt(arr, sizeStack-2);
}

void mergeCollapse(int arr[]) {
    while (sizeStack > 1) {
        int n = sizeStack - 2;

        if (n > 0 && runStack[n-1].len <= runStack[n].len + runStack[n+1].len) mergeAt(arr, n-1);
        else if (runStack[n].len <= runStack[n+1].len) mergeAt(arr, n);
        else break;
    }
}

void timSort(int arr[], int n) {
    int minRun = calcMinRun(n);
    int start = 0;
    while (start < n) {
        int runLen = runLenAndMakeAsc(arr, n, start);
        if (runLen < minRun) {
            int force = min(minRun, n - start);
            insertionSort(arr, start, start+force);
            runLen = force;
        }
        pushRun(start, runLen);
        mergeCollapse(arr);
//        mergeTop(arr);
        // merge
        start += runLen;
    }

    while (sizeStack > 1) mergeTop(arr);
}

void printArray(int arr[], int n)
{
    for (int i = 0; i < n; i++)
        printf("%d  ", arr[i]);
    printf("\n");
}

