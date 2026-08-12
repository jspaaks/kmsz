#ifndef ADD_PROGRAM_INCLUDED
#define ADD_PROGRAM_INCLUDED
#include "opencl.h"

void program_create (cl_context context, cl_device_id * device, const char * filename, cl_program * program, int * err);
void program_destroy (cl_program program);

#endif
