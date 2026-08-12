#include "kernel.h"
#include "opencl.h"
#include <stdio.h>    // fprintf, stderr


void kernel_create (cl_program program, char * kernel_name, cl_kernel * kernel, int * err) {
    if (err == nullptr) {
        *err = __LINE__;
        fprintf(stderr, "ERROR %d: argument `err` shouldn't be nullptr\n", *err);
        return;
    }
    if (*err) return;
    *kernel = clCreateKernel(program, kernel_name, err);
    if (*err) {
        fprintf(stderr, "ERROR %d: enocountered problem in call to clCreateKernel\n", *err);
    }
}


void kernel_destroy (cl_kernel kernel) {
    clReleaseKernel(kernel);
}


void kernel_enqueue_execution (cl_command_queue queue, cl_kernel kernel, const size_t * global_work_size, const size_t * local_work_size, int * err) {
    if (err == nullptr) {
        *err = __LINE__;
        fprintf(stderr, "ERROR %d: argument `err` shouldn't be nullptr\n", *err);
        return;
    }
    if (*err) return;

    cl_uint work_dim = 1;
    const size_t * global_work_offset = nullptr;
    cl_uint num_events_in_waitlist = 0;
    const cl_event * event_wait_list = nullptr;
    cl_event * event = nullptr;

    *err = clEnqueueNDRangeKernel(queue, kernel, work_dim, global_work_offset, global_work_size, local_work_size, num_events_in_waitlist, event_wait_list, event);

    if (*err) {
        fprintf(stderr, "ERROR %d: problem enqueueing kernel execution\n", *err);
    }
}


void kernel_set_arg (cl_kernel kernel, cl_uint idim, size_t nbytes, const void * arg) {
    clSetKernelArg(kernel, idim, nbytes, arg);
}
