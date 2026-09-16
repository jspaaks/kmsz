#ifndef ROTATION_OPENCL_INCLUDED
#define ROTATION_OPENCL_INCLUDED

#ifndef CL_TARGET_OPENCL_VERSION
#define CL_TARGET_OPENCL_VERSION 300

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif                        // __APPLE__

#endif                        // CL_TARGET_OPENCL_VERSION

#endif                        // ROTATION_OPENCL_INCLUDED
