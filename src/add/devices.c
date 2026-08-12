#include "devices.h"
#include <stdio.h>       // fprintf, stderr
#include <stdlib.h>      // calloc, free


void devices_create (cl_platform_id platform, int * ndevices, cl_device_id ** devices, int * err) {
    if (err == nullptr) {
        *err = __LINE__;
        fprintf(stderr, "ERROR %d: argument `err` shouldn't be nullptr\n", *err);
        return;
    }
    if (*err) return;

    cl_device_type device_type = CL_DEVICE_TYPE_ALL;
    clGetDeviceIDs(platform, device_type, 0, nullptr, (cl_uint *) ndevices);
    *devices = calloc(*ndevices, sizeof(cl_device_id));
    if (*devices == nullptr) {
        *err = __LINE__;
        fprintf(stderr, "ERROR %d: problem allocating dynamic memory for device type array, aborting\n", *err);
        return;
    }

    clGetDeviceIDs(platform, device_type, (cl_uint) *ndevices, *devices, nullptr);
}


void devices_destroy (int ndevices, cl_device_id ** devices) {
    if (*devices == nullptr) return;

    for (int i = 0; i < ndevices; i++) {
        clReleaseDevice((*devices)[i]);
    }

    free(*devices);
    *devices = nullptr;
}


void devices_verify_exist (int ndevices, int * err) {
    if (err == nullptr) {
        *err = __LINE__;
        fprintf(stderr, "ERROR %d: argument `err` shouldn't be nullptr\n", *err);
        return;
    }
    if (*err) return;
    if (ndevices > 0) return;
    *err = __LINE__;
    fprintf(stderr, "ERROR %d: There are no devices on the platform, aborting\n", *err);
}
