#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

//odd even algorithm
void odd_even (int *a, int array_size) {

    int temp;
    for (int j = 0; j < array_size; j++) {
        for (int i = 0; i < array_size; i++) {
            
            //even step
            if ((i % 2 == 0) && (i + 1 < array_size)) {
                if (a[i] > a[i + 1]) {
                    temp = a[i];
                    a[i] = a[i + 1];
                    a[i + 1] = temp;
                }
            }
            //odd step
            if((i % 2 == 1)) {
                if (a[i] > a[i + 1]) {
                    temp = a[i];
                    a[i] = a[i + 1];
                    a[i + 1] = temp;
                }
            }
        }
    }
}


int main(void) {
    //list size
    //change integer List_size. must be a power of 2
    const int LIST_SIZE = 65536;
    printf("List size : %d\n", LIST_SIZE);
    //allocate memory for an array
    int *A = (int*)malloc(sizeof(int)*LIST_SIZE);
    //give a random int to the array 
    for(int i = 0; i < LIST_SIZE; i++) {
        A[i] = rand() % 5000;
    }
    //setup timer
    struct timeval start, end;

    gettimeofday(&start, NULL);
    //call function for odd-even sorting
    odd_even(A, LIST_SIZE);

    gettimeofday(&end, NULL);

    double time_taken = end.tv_sec + end.tv_usec / 1e6 -
                        start.tv_sec - start.tv_usec / 1e6; // in seconds

    time_taken = time_taken * 1000;
    printf("time program took %.03f milliseconds to execute\n\n", time_taken);

}