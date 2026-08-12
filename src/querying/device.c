#define CL_TARGET_OPENCL_VERSION 300
#include "device.h"           // device_*, struct device
#ifdef __APPLE__
#include <OpenCL/opencl.h>    // cl*
#else
#include <CL/cl.h>            // cl*
#endif
#include <stdio.h>            // fprintf, stderr
#include <stdlib.h>           // calloc, free, exit


struct device {
    char * driver_version;
    cl_device_id id;
    char * name;
    char * opencl_c_version;
    char * profile;
    cl_device_type * type;
    char * vendor;
    cl_uint * vendor_id;
    char * version;
};


static void check_status (cl_int status);


static void check_status (cl_int status) {
    switch (status) {
        case CL_INVALID_DEVICE: {
            const int code = __LINE__;
            fprintf(stderr, "ERROR %d: invalid device, aborting\n", code);
            exit(code);
            break;
        }
        case CL_INVALID_VALUE: {
            const int code = __LINE__;
            fprintf(stderr, "ERROR %d: invalid value, aborting\n", code);
            exit(code);
            break;
        }
        case CL_OUT_OF_RESOURCES: {
            const int code = __LINE__;
            fprintf(stderr, "ERROR %d: out of resources, aborting\n", code);
            exit(code);
            break;
        }
        case CL_OUT_OF_HOST_MEMORY: {
            const int code = __LINE__;
            fprintf(stderr, "ERROR %d: out of host memory, aborting\n", code);
            exit(code);
            break;
        }
        case CL_SUCCESS: {
            break;
        }
        default: {
            const int code = __LINE__;
            fprintf(stderr, "ERROR %d: unreachable, aborting\n", code);
            exit(code);
            break;
        }
    }
}


struct device * device_create (cl_device_id id) {
    struct device * self = calloc(1, sizeof(struct device));
    if (self == nullptr) {
        const int code = __LINE__;
        fprintf(stderr, "ERROR %d: problem allocating dynamic memory for struct device, aborting\n", code);
        exit(code);
    }
    *self = (struct device) {
        .id = id,
    };
    return self;
}


void device_destroy (struct device ** self) {
    if (*self == nullptr) return;

    free((*self)->driver_version);
    (*self)->driver_version = nullptr;

    free((*self)->name);
    (*self)->name = nullptr;

    free((*self)->vendor);
    (*self)->vendor = nullptr;

    free(*self);
    *self = nullptr;
}


char * device_get_driver_version (struct device * self) {
    size_t nbytes = 0;
    {
        cl_int status = clGetDeviceInfo(self->id, CL_DRIVER_VERSION, 0, self->driver_version, &nbytes);
        check_status(status);
    }
    self->driver_version = calloc(nbytes, sizeof(char));
    if (self->driver_version == nullptr) {
        const int code = __LINE__;
        fprintf(stderr, "ERROR %d: problem allocating dynamic memory for driver version, aborting\n", code);
        exit(code);
    }
    {
        cl_int status = clGetDeviceInfo(self->id, CL_DRIVER_VERSION, nbytes, self->driver_version, nullptr);
        check_status(status);
    }
    return self->driver_version;
}


char * device_get_name (struct device * self) {
    size_t nbytes = 0;
    {
        cl_int status = clGetDeviceInfo(self->id, CL_DEVICE_NAME, 0, self->name, &nbytes);
        check_status(status);
    }
    self->name = calloc(nbytes, sizeof(char));
    if (self->name == nullptr) {
        const int code = __LINE__;
        fprintf(stderr, "ERROR %d: problem allocating dynamic memory for device name, aborting\n", code);
        exit(code);
    }
    {
        cl_int status = clGetDeviceInfo(self->id, CL_DEVICE_NAME, nbytes, self->name, nullptr);
        check_status(status);
    }
    return self->name;
}


char * device_get_opencl_c_version (struct device * self) {
    size_t nbytes = 0;
    {
        cl_int status = clGetDeviceInfo(self->id, CL_DEVICE_OPENCL_C_VERSION, 0, self->opencl_c_version, &nbytes);
        check_status(status);
    }
    self->opencl_c_version = calloc(nbytes, sizeof(char));
    if (self->opencl_c_version == nullptr) {
        const int code = __LINE__;
        fprintf(stderr, "ERROR %d: problem allocating dynamic memory for device OpenCL C version, aborting\n", code);
        exit(code);
    }
    {
        cl_int status = clGetDeviceInfo(self->id, CL_DEVICE_OPENCL_C_VERSION, nbytes, self->opencl_c_version, nullptr);
        check_status(status);
    }
    return self->opencl_c_version;
}


char * device_get_profile (struct device * self) {
    size_t nbytes = 0;
    {
        cl_int status = clGetDeviceInfo(self->id, CL_DEVICE_PROFILE, 0, self->profile, &nbytes);
        check_status(status);
    }
    self->profile = calloc(nbytes, sizeof(char));
    if (self->profile == nullptr) {
        const int code = __LINE__;
        fprintf(stderr, "ERROR %d: problem allocating dynamic memory for device profile, aborting\n", code);
        exit(code);
    }
    {
        cl_int status = clGetDeviceInfo(self->id, CL_DEVICE_PROFILE, nbytes, self->profile, nullptr);
        check_status(status);
    }
    return self->profile;
}


char * device_get_vendor (struct device * self) {
    size_t nbytes = 0;
    {
        cl_int status = clGetDeviceInfo(self->id, CL_DEVICE_VENDOR, 0, self->vendor, &nbytes);
        check_status(status);
    }
    self->vendor = calloc(nbytes, sizeof(char));
    if (self->vendor == nullptr) {
        const int code = __LINE__;
        fprintf(stderr, "ERROR %d: problem allocating dynamic memory for device vendor, aborting\n", code);
        exit(code);
    }
    {
        cl_int status = clGetDeviceInfo(self->id, CL_DEVICE_VENDOR, nbytes, self->vendor, nullptr);
        check_status(status);
    }
    return self->vendor;
}


cl_uint * device_get_vendor_id (struct device * self) {
    size_t nbytes = 0;
    {
        cl_int status = clGetDeviceInfo(self->id, CL_DEVICE_VENDOR_ID, 0, self->vendor_id, &nbytes);
        check_status(status);
    }
    self->vendor_id = calloc(nbytes, sizeof(cl_uint));
    if (self->vendor_id == nullptr) {
        const int code = __LINE__;
        fprintf(stderr, "ERROR %d: problem allocating dynamic memory for device vendor id, aborting\n", code);
        exit(code);
    }
    {
        cl_int status = clGetDeviceInfo(self->id, CL_DEVICE_VENDOR_ID, nbytes, self->vendor_id, nullptr);
        check_status(status);
    }
    return self->vendor_id;
}


char * device_get_version (struct device * self) {
    size_t nbytes = 0;
    {
        cl_int status = clGetDeviceInfo(self->id, CL_DEVICE_VERSION, 0, self->version, &nbytes);
        check_status(status);
    }
    self->version = calloc(nbytes, sizeof(char));
    if (self->version == nullptr) {
        const int code = __LINE__;
        fprintf(stderr, "ERROR %d: problem allocating dynamic memory for device version, aborting\n", code);
        exit(code);
    }
    {
        cl_int status = clGetDeviceInfo(self->id, CL_DEVICE_VERSION, nbytes, self->version, nullptr);
        check_status(status);
    }
    return self->version;
}


cl_device_type * device_get_type (struct device * self) {
    size_t nbytes = 0;
    {
        cl_int status = clGetDeviceInfo(self->id, CL_DEVICE_TYPE, 0, self->type, &nbytes);
        check_status(status);
    }
    self->type = calloc(nbytes, sizeof(char));
    if (self->type == nullptr) {
        const int code = __LINE__;
        fprintf(stderr, "ERROR %d: problem allocating dynamic memory for device type, aborting\n", code);
        exit(code);
    }
    {
        cl_int status = clGetDeviceInfo(self->id, CL_DEVICE_TYPE, nbytes, self->type, nullptr);
        check_status(status);
    }
    return self->type;
}


    // cl_device_info props[33] = {
    //     CL_DEVICE_TYPE_CPU,
    //     CL_DEVICE_MAX_COMPUTE_UNITS,
    //     CL_DEVICE_MAX_CLOCK_FREQUENCY,
    //     CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS,
    //     CL_DEVICE_MAX_WORK_ITEM_SIZES,
    //     CL_DEVICE_MAX_WORK_GROUP_SIZE,
    //     CL_DEVICE_ADDRESS_BITS,
    //     CL_DEVICE_MAX_READ_IMAGE_ARGS,
    //     CL_DEVICE_MAX_WRITE_IMAGE_ARGS,
    //     CL_DEVICE_GLOBAL_MEM_SIZE,
    //     CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE,
    //     CL_DEVICE_GLOBAL_MEM_CACHE_SIZE,
    //     CL_DEVICE_MAX_MEM_ALLOC_SIZE,
    //     CL_DEVICE_LOCAL_MEM_TYPE,
    //     CL_DEVICE_LOCAL_MEM_SIZE,
    //     CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE,
    //     CL_DEVICE_MAX_CONSTANT_ARGS,
    //     CL_DEVICE_AVAILABLE,
    //     CL_DEVICE_COMPILER_AVAILABLE,
    //     CL_DEVICE_LINKER_AVAILABLE,
    //     CL_DEVICE_ENDIAN_LITTLE,
    //     CL_DEVICE_ERROR_CORRECTION_SUPPORT,
    //     CL_DEVICE_HOST_UNIFIED_MEMORY,
    //     CL_DEVICE_EXTENSIONS,
    //     CL_DEVICE_PLATFORM,
    // };
