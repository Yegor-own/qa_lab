#include "timsort.h"
#include "test.h"




int main() {
    int arr[500];
    int n = 500;
    gen_rand_arr(arr);
//    gen_many_same_arr(arr);

    printf("Given Array is\n");
    printArray(arr, n);

    // Function Call
    timSort(arr, n);

    printf("After Sorting Array is\n");
    printArray(arr, n);
    is_ok(arr, n);

//    test_gallop();
    return 0;
}
