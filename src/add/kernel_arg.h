#ifndef ADD_KERNEL_ARG_METADATA_INCLUDED
#define ADD_KERNEL_ARG_METADATA_INCLUDED
#include "opencl.h"

void kernel_arg_create_metadata (cl_context context, size_t nbytes, cl_mem_flags flags, cl_mem * meta, int *err);
void kernel_arg_destroy_metadata (cl_mem meta);
void kernel_arg_enqueue_reading (cl_command_queue queue, cl_mem meta, size_t nbytes, int * arg, int * err);
void kernel_arg_enqueue_writing (cl_command_queue queue, cl_mem meta, size_t nbytes, const int * arg, int * err);

#endif
