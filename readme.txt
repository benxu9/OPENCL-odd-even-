Instructions to run the code:

ssh into the aws ronin server.

The folder contains parallel.c and kernel.c for the parallel implementation.

The serial implementation can be found in serial.c

To edit the array size and number of working items,
go to line 74 and 75 of parallel.c
Change the array size to a power of 2. Use array sizes greater than 1000
worker-item numbers from 1 to 1024. powers of 2 also.
Parallel:
1. make
2. sbatch jobscript.sh


Serial:
1. make serial
2. sbatch serial.sh


All output will be in a slurm file.


Parallel code explanation kernel explanation:


The kernel takes an array an array A as input.
If the array size is greater than the number of work items,
For each work item, we give it multiple array values.
The number of array values that a work item must processed is determined by 
Arraysize / number of work items. For example. Array size 2048 and work items 1024.
2048/1024 = 2.
This means each work item must process 2 index swaps.
For example index 0 will compare 0 and 1 as well as 1025 and 1026.

We can see that if we reduce the work items all the way down to 1, the process actually becomes serial again. Because there are two for loops running N times each. Whereas when we increase the size of the work items, the second for loop that must process multiple index values is reduced to a fraction of the array size compared to the work item numbers.
In an ideal scenario, we would want 1 work item per 1 index computation. This would mean that each work item only needs to do one swap comparison (odd or even).
