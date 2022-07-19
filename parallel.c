#include <stdio.h>
#include <stdlib.h>

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

void check_err(cl_int err, char* err_output) {
    if (err != CL_SUCCESS) {
        fprintf(stderr, "Error %i: %s\n", (int)err, err_output);
        exit(1);
    }
}
// used to read kernel file
long LoadOpenCLKernel(char const* path, char **buf)
{
    FILE  *fp;
    size_t fsz;
    long   off_end;
    int    rc;

    /* Open the file */
    fp = fopen(path, "r");
    if( NULL == fp ) {
        return -1L;
    }

    /* Seek to the end of the file */
    rc = fseek(fp, 0L, SEEK_END);
    if( 0 != rc ) {
        return -1L;
    }

    /* Byte offset to the end of the file (size) */
    if( 0 > (off_end = ftell(fp)) ) {
        return -1L;
    }
    fsz = (size_t)off_end;

    /* Allocate a buffer to hold the whole file */
    *buf = (char *) malloc( fsz+1);
    if( NULL == *buf ) {
        return -1L;
    }

    /* Rewind file pointer to start of file */
    rewind(fp);

    /* Slurp file into buffer */
    if( fsz != fread(*buf, 1, fsz, fp) ) {
        free(*buf);
        return -1L;
    }

    /* Close the file */
    if( EOF == fclose(fp) ) {
        free(*buf);
        return -1L;
    }


    /* Make sure the buffer is NUL-terminated, just in case */
    (*buf)[fsz] = '\0';

    /* Return the file size */
    return (long)fsz;
}

int main(void) {
    // Create the list
    
    const int LIST_SIZE = 65536;
    int n_workitems = 1024;
    printf("List size : %d\n", LIST_SIZE);
    int *A = (int*)malloc(sizeof(int)*LIST_SIZE);
    
    for(int i = 0; i < LIST_SIZE; i++) {
        A[i] = rand() % 5000;
        
    }

   //load the kernal
   char *KernelSource;
   long lFileSize;

   lFileSize = LoadOpenCLKernel("kernel.cl", &KernelSource);
   if( lFileSize < 0L ) {
       perror("File read failed - RAWR check kernel name");
       return 1;
   }


    // variable used for OpenCL error handling
    cl_int err;

    // Get the platform ID
    cl_platform_id platform_id;
    cl_uint ret_num_platforms;
    err = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
    check_err(err, "clGetPlatformIDs");
    //printf("%i Platforms found\n", ret_num_platforms);

    // Get the first GPU device associated with the platform
    cl_device_id device_id;
    cl_uint ret_num_devices;
    err = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, &device_id, &ret_num_devices);
    check_err(err, "clGetPlatformIDs");
    //printf("%i Devices found\n", ret_num_devices);

    // Get maximum work group size
    size_t max_work_group_size;
    err = clGetDeviceInfo(device_id, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(max_work_group_size), &max_work_group_size, NULL);
    check_err(err, "clGetDeviceInfo");
    //printf("Max workgroup size is %i\n", (int)max_work_group_size);

    // Create an OpenCL context
    cl_context context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &err);
    check_err(err, "clCreateContext");

    // Create Queue with Profiling enabled
    cl_command_queue command_queue = clCreateCommandQueue(context, device_id, CL_QUEUE_PROFILING_ENABLE, &err);
    check_err(err, "clCreateCommandQueue");

    // Create memory buffers on the device for each vector
    cl_mem a_mem_obj = clCreateBuffer(context, CL_MEM_READ_WRITE, LIST_SIZE * sizeof(int), NULL, &err);
    check_err(err, "clCreateBuffer a");
    // cl_mem c_mem_obj = clCreateBuffer(context, CL_MEM_WRITE_ONLY, LIST_SIZE * sizeof(int), NULL, &err);
    // check_err(err, "clCreateBuffer c");

    // Copy the lists A and B to their respective memory buffers
    err = clEnqueueWriteBuffer(command_queue, a_mem_obj, CL_TRUE, 0, LIST_SIZE * sizeof(int), A, 0, NULL, NULL);
    check_err(err, "clEnqueueWriteBuffer a");

    // Create a program from the kernel source
    cl_program program = clCreateProgramWithSource(context, 1, (const char **)&KernelSource, (const size_t *)&lFileSize, &err);
    check_err(err, "clCreateProgramWithSource");

    // Build the program
    err = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
    check_err(err, "clBuildProgram");

    // Create the OpenCL kernel
    cl_kernel kernel = clCreateKernel(program, "odd_even", &err);
    check_err(err, "clCreateKernel");

    // Set the arguments of the kernel
    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&a_mem_obj);
    check_err(err, "clSetKernelArg a");
    //err = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&b_mem_obj);
    //check_err(err, "clSetKernelArg b");
    // err = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&c_mem_obj);
    // check_err(err, "clSetKernelArg c");
    int lsize = LIST_SIZE;
    err = clSetKernelArg(kernel, 1, sizeof(int), (void *)&lsize);
    check_err(err, "clSetKernelArg listsize");

    // Ensure to have executed all enqueued tasks
    err = clFinish(command_queue);
    check_err(err, "clFinish");

    // Launch Kernel linked to an event
    cl_event event;
    size_t local_work_size = (size_t)n_workitems;
    size_t global_work_size = local_work_size;
    err = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_work_size, &local_work_size, 0, NULL, &event);
    check_err(err, "clEnqueueNDRangeKernel");
    printf("global work size ==== %d\n", (int)global_work_size);
    // Ensure kernel execution is finished
    err = clWaitForEvents(1, &event);
    check_err(err, "clWaitForEvents");

    // Get the Profiling data
    cl_ulong time_start, time_end;
    double total_time;
    err = clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(time_start), &time_start, NULL);
    check_err(err, "clGetEventProfilingInfo Start");
    err = clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(time_end), &time_end, NULL);
    check_err(err, "clGetEventProfilingInfo End");
    total_time = time_end - time_start;

    // Read the memory buffer C on the device to the local variable C
    int *C = (int*)malloc(sizeof(int)*LIST_SIZE);
    err = clEnqueueReadBuffer(command_queue, a_mem_obj, CL_TRUE, 0, LIST_SIZE * sizeof(int), C, 0, NULL, NULL);
    check_err(err, "clEnqueueReadBuffer c");

    // Display the result to the screen
    // for(int i = 0; i < LIST_SIZE; i++)
    //     printf("%d +  = %d\n", A[i], C[i]);

    // Display GPU execution time
    printf("Execution time in milliseconds = %0.3f ms\n\n", (total_time / 1000000.0));

    // Clean up
    err = clFlush(command_queue);
    err = clFinish(command_queue);
    err = clReleaseKernel(kernel);
    err = clReleaseProgram(program);
    err = clReleaseMemObject(a_mem_obj);
    //err = clReleaseMemObject(b_mem_obj);
    // err = clReleaseMemObject(c_mem_obj);
    err = clReleaseCommandQueue(command_queue);
    err = clReleaseContext(context);
    free(A);
    //free(B);
    // free(C);
    return 0;
}
