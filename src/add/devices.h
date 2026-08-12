#ifndef ADD_DEVICES_INCLUDED
#define ADD_DEVICES_INCLUDED
#include "opencl.h"

void devices_create (cl_platform_id platform, int * ndevices, cl_device_id ** devices, int * err);
void devices_destroy (int ndevices, cl_device_id ** devices);
void devices_verify_exist(int ndevices, int * err);

#endif
