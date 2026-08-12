#include "device.h"
#define CL_TARGET_OPENCL_VERSION 300
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif
#include <stdlib.h>
#include <stdio.h>


static void create_devices (cl_platform_id platform, int * ndevices, cl_device_id ** devices);
static void create_platform_name (cl_platform_id platform, char ** name);
static void create_platforms (int * nplatforms, cl_platform_id ** platforms);
static void destroy_devices (cl_device_id ** devices);
static void destroy_platform_name (char ** name);
static void destroy_platforms (cl_platform_id ** platforms);
static void print_device_header (int idev, int ndevices);
static void print_device_info (cl_device_id device_id);
static void print_devices_header (int ndevices);
static void print_platform_header (int iplat, int nplatforms, const char * name);
static void print_platforms_header (int nplatforms);


static void create_devices (cl_platform_id platform, int * ndevices, cl_device_id ** devices) {
    clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 0, *devices, (cl_uint *) ndevices);
    *devices = calloc(*ndevices, sizeof(cl_device_id));
    if (*devices == nullptr) {
        const int code = __LINE__;
        fprintf(stderr, "ERROR %d: problem allocating dynamic memory for device type array, aborting\n", code);
        exit(code);
    }

    clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, (cl_uint) *ndevices, *devices, nullptr);
}


static void create_platform_name (cl_platform_id platform, char ** name) {
    size_t nchars = 0;
    clGetPlatformInfo(platform, CL_PLATFORM_NAME, 0, nullptr, &nchars);

    *name = calloc(nchars, sizeof(char));
    if (*name == nullptr) {
        const int code = __LINE__;
        fprintf(stderr, "ERROR %d: problem allocating dynamic memory for platform name, aborting\n", code);
        exit(code);
    }

    clGetPlatformInfo(platform, CL_PLATFORM_NAME, nchars, *name, nullptr);
}


static void create_platforms (int * nplatforms, cl_platform_id ** platforms) {
    cl_int status = clGetPlatformIDs(0, *platforms, (cl_uint *) nplatforms);
    switch (status) {
        case CL_INVALID_VALUE: {
            const int code = __LINE__;
            fprintf(stdout, "ERROR %d: Invalid value, aborting\n", code);
            exit(code);
            break;
        }
        case CL_OUT_OF_HOST_MEMORY: {
            const int code = __LINE__;
            fprintf(stdout, "ERROR %d: out of host memory, aborting\n", code);
            exit(code);
            break;
        }
        case CL_SUCCESS: {
            break;
        }
        default: {
            const int code = __LINE__;
            fprintf(stdout, "ERROR %d: unreachable, aborting\n", code);
            exit(code);
            break;
        }
    }

    *platforms = calloc(*nplatforms, sizeof(cl_platform_id));
    if (*platforms == nullptr) {
        const int code = __LINE__;
        fprintf(stderr, "ERROR %d: problem allocating dynamic memory for platforms array, aborting\n", code);
        exit(code);
    }

    status = clGetPlatformIDs((cl_uint) *nplatforms, *platforms, nullptr);
    switch (status) {
        case CL_INVALID_VALUE: {
            const int code = __LINE__;
            fprintf(stdout, "ERROR %d: Invalid value, aborting\n", code);
            exit(code);
            break;
        }
        case CL_OUT_OF_HOST_MEMORY: {
            const int code = __LINE__;
            fprintf(stdout, "ERROR %d: out of host memory, aborting\n", code);
            exit(code);
            break;
        }
        case CL_SUCCESS: {
            break;
        }
        default: {
            const int code = __LINE__;
            fprintf(stdout, "ERROR %d: unreachable, aborting\n", code);
            exit(code);
            break;
        }
    }
}


static void destroy_devices (cl_device_id ** devices) {
    if (*devices == nullptr) return;
    free(*devices);
    *devices = nullptr;
}


static void destroy_platform_name (char ** name) {
    if (*name == nullptr) return;
    free(*name);
    *name = nullptr;
}


static void destroy_platforms (cl_platform_id ** platforms) {
    if (*platforms == nullptr) return;
    free(*platforms);
    *platforms = nullptr;
}


int main (void) {
    int nplatforms = -1;
    cl_platform_id * platforms = nullptr;

    create_platforms(&nplatforms, &platforms);
    print_platforms_header(nplatforms);

    for (int iplat = 0; iplat < nplatforms; iplat++) {
        char * name = nullptr;
        create_platform_name(platforms[iplat], &name);
        print_platform_header(iplat, nplatforms, name);

        int ndevices = -1;
        cl_device_id * devices = nullptr;
        create_devices(platforms[iplat], &ndevices, &devices);
        print_devices_header(ndevices);

        for (int idev = 0; idev < ndevices; idev++) {
            print_device_header(idev, ndevices);
            print_device_info(devices[idev]);
        }

        destroy_devices(&devices);
        destroy_platform_name(&name);
    }

    destroy_platforms(&platforms);

    return EXIT_SUCCESS;
}


static void print_device_info (cl_device_id device_id) {
    struct device * device = device_create(device_id);
    fprintf(stdout, "         CL_DEVICE_NAME               %s\n", device_get_name(device));
    fprintf(stdout, "         CL_DEVICE_VENDOR             %s\n", device_get_vendor(device));
    fprintf(stdout, "         CL_DEVICE_VENDOR_ID          0x%04x\n", (uint32_t) *device_get_vendor_id(device));
    fprintf(stdout, "         CL_DEVICE_VERSION            %s\n", device_get_version(device));
    fprintf(stdout, "         CL_DRIVER_VERSION            %s\n", device_get_driver_version(device));
    fprintf(stdout, "         CL_DEVICE_OPENCL_C_VERSION   %s\n", device_get_opencl_c_version(device));
    fprintf(stdout, "         CL_DEVICE_PROFILE            %s\n", device_get_profile(device));
    {
        cl_device_type * t = device_get_type(device);
        switch (*t) {
        case CL_DEVICE_TYPE_ACCELERATOR:
            fprintf(stdout, "         CL_DEVICE_TYPE               CL_DEVICE_TYPE_ACCELERATOR\n");
            break;
        case CL_DEVICE_TYPE_CPU:
            fprintf(stdout, "         CL_DEVICE_TYPE               CL_DEVICE_TYPE_CPU\n");
            break;
        case CL_DEVICE_TYPE_DEFAULT:
            fprintf(stdout, "         CL_DEVICE_TYPE               CL_DEVICE_TYPE_DEFAULT\n");
            break;
        case CL_DEVICE_TYPE_GPU:
            fprintf(stdout, "         CL_DEVICE_TYPE               CL_DEVICE_TYPE_GPU\n");
            break;
        default:
            const int code = __LINE__;
            fprintf(stderr, "ERROR %d: unreachable code, aborting\n", code);
            exit(code);
            break;
        }
    }
    device_destroy(&device);
}


static void print_device_header (int idev, int ndevices) {
    fprintf(stdout, "      device %d of %d\n", idev + 1, ndevices);
}


static void print_devices_header (int ndevices) {
    fprintf(stdout,
            "   There %s %d %s on the platform%c\n",
            ndevices == 1 ? "is" : "are",
            ndevices,
            ndevices == 1 ? "device" : "devices",
            ndevices == 0 ? '.' : ':');
}


static void print_platform_header (int iplat, int nplatforms, const char * name) {
    fprintf(stdout, "   Platform %d of %d: %s\n", iplat + 1, nplatforms, name);
}


static void print_platforms_header (int nplatforms) {
    fprintf(stdout,
            "There %s %d OpenCL %s on the host%s\n",
            nplatforms == 1 ? "is" : "are",
            nplatforms,
            nplatforms == 1 ? "platform" : "platforms",
            nplatforms == 0 ? "." : ":");
}
