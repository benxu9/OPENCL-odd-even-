__kernel void odd_even(__global int *a, int array_size) {

    // Get the index of the current element to be processed
    int i = get_global_id(0);
    int j = (int)get_global_size(0);

    int pr = array_size / j;
    //Do the operation
    int temp;

    //even phase
    for (int k = 0; k < array_size; k++) {

        if ((i % 2 == 0) && (i + 1 <= j)) {
            for(int m = 0; m < pr; m++) {
                if (a[(i + (j*m))] > a[(i + (j*m)) + 1]) {
                    temp = a[(i + (j*m))];
                    a[(i + (j*m))] = a[(i + (j*m)) + 1];
                    a[(i + (j*m)) + 1] = temp;
                }
            }
            
        }
        //barrier to sychronise all other work items
        barrier(CLK_GLOBAL_MEM_FENCE);

        //odd phase
        if ((i % 2 == 1)) {
            for (int m = 0; m < pr; m++) {
                if (a[(i + (j*m))] > a[(i + (j*m)) + 1]) {
                    temp = a[(i + (j*m))];
                    a[(i + (j*m))] = a[(i + (j*m)) + 1];
                    a[(i + (j*m)) + 1] = temp;
                }
            }
            
        }
        barrier(CLK_GLOBAL_MEM_FENCE);
    }
}
