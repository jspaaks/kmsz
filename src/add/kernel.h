#ifndef ADD_KERNEL_INCLUDED
#define ADD_KERNEL_INCLUDED
#include "opencl.h"

void kernel_create (cl_program program, char * kernel_name, cl_kernel * kernel, int * err);
void kernel_destroy (cl_kernel kernel);
void kernel_enqueue_execution (cl_command_queue queue, cl_kernel kernel, const size_t * global_work_size, const size_t * local_work_size, int * err);
void kernel_set_arg (cl_kernel kernel, cl_uint idim, size_t nbytes, const void * arg);

#endif
