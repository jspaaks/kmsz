#ifndef QUERYING_DEVICE_INCLUDED
#define QUERYING_DEVICE_INCLUDED
#define CL_TARGET_OPENCL_VERSION 300
#include "device.h"           // device_*, struct device
#ifdef __APPLE__
#include <OpenCL/opencl.h>    // cl*
#else
#include <CL/cl.h>            // cl*
#endif

struct device;

struct device * device_create (cl_device_id id);
void device_destroy (struct device ** self);
char * device_get_driver_version (struct device * self);
char * device_get_name (struct device * self);
char * device_get_opencl_c_version (struct device * self);
char * device_get_profile (struct device * self);
cl_device_type * device_get_type (struct device * self);
char * device_get_vendor (struct device * self);
cl_uint * device_get_vendor_id (struct device * self);
char * device_get_version (struct device * self);

#endif
