#ifndef ADD_CONTEXT_INCLUDED
#define ADD_CONTEXT_INCLUDED
#include "opencl.h"

void context_create (int ndevices, const cl_device_id * devices, cl_context * context, int * err);
void context_destroy (cl_context context);

#endif
