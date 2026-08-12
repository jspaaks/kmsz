#ifndef ADD_PLATFORMS_INCLUDED
#define ADD_PLATFORMS_INCLUDED
#include "opencl.h"

void platforms_create (int * nplatforms, cl_platform_id ** platforms, int * err);
void platforms_destroy (cl_platform_id ** platforms);
void platforms_verify_exist (int nplatforms, int * err);

#endif
