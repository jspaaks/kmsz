#include "platforms.h"
#include "opencl.h"
#include <stdio.h>       // fprintf, stderr


void platforms_create (int * nplatforms, cl_platform_id ** platforms, int * err) {
    if (err == nullptr) {
        *err = __LINE__;
        fprintf(stderr, "ERROR %d: argument `err` shouldn't be nullptr\n", *err);
        return;
    }
    if (*err) return;
    cl_int status = clGetPlatformIDs(0, *platforms, (cl_uint *) nplatforms);
    switch (status) {
        case CL_INVALID_VALUE: {
            *err = __LINE__;
            fprintf(stderr, "ERROR %d: Invalid value, aborting\n", *err);
            return;
        }
        case CL_OUT_OF_HOST_MEMORY: {
            *err = __LINE__;
            fprintf(stderr, "ERROR %d: out of host memory, aborting\n", *err);
            return;
        }
        case CL_SUCCESS: {
            break;
        }
        default: {
            *err = __LINE__;
            fprintf(stderr, "ERROR %d: unreachable, aborting\n", *err);
            return;
        }
    }

    *platforms = calloc(*nplatforms, sizeof(cl_platform_id));
    if (*platforms == nullptr) {
        *err = __LINE__;
        fprintf(stderr, "ERROR %d: problem allocating dynamic memory for platforms array, aborting\n", *err);
        return;
    }

    status = clGetPlatformIDs((cl_uint) *nplatforms, *platforms, nullptr);
    switch (status) {
        case CL_INVALID_VALUE: {
            *err = __LINE__;
            fprintf(stderr, "ERROR %d: Invalid value, aborting\n", *err);
            return;
        }
        case CL_OUT_OF_HOST_MEMORY: {
            *err = __LINE__;
            fprintf(stderr, "ERROR %d: out of host memory, aborting\n", *err);
            return;
        }
        case CL_SUCCESS: {
            break;
        }
        default: {
            *err = __LINE__;
            fprintf(stderr, "ERROR %d: unreachable, aborting\n", *err);
            return;
        }
    }
}


void platforms_destroy (cl_platform_id ** platforms) {
    if (*platforms == nullptr) return;
    free(*platforms);
    *platforms = nullptr;
}


void platforms_verify_exist (int nplatforms, int * err) {
    if (err == nullptr) {
        *err = __LINE__;
        fprintf(stderr, "ERROR %d: argument `err` shouldn't be nullptr\n", *err);
        return;
    }
    if (*err) return;
    if (nplatforms > 0) return;
    *err = __LINE__;
    fprintf(stderr, "ERROR %d: There are no OpenCL platforms, aborting\n", *err);
}
