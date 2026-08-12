#include "context.h"
#include "opencl.h"
#include <stdio.h>


void context_create (int ndevices, const cl_device_id * devices, cl_context * context, int * err) {
    if (err == nullptr) {
        *err = __LINE__;
        fprintf(stderr, "ERROR %d: argument `err` shouldn't be nullptr\n", *err);
        return;
    }
    if (*err) return;
    cl_context_properties * properties = nullptr;
    void (*callback)(const char *, const void *, size_t,  void *) = nullptr;
    void * userdata = nullptr;
    *context = clCreateContext(properties, ndevices, devices, callback, userdata, (cl_int *) err);
}


void context_destroy (cl_context context) {
    clReleaseContext(context);
}
