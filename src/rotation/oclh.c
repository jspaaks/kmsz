#include "oclh.h"             // header file corresponding to this implementation 
#include "opencl.h"           // opencl
#include <errno.h>            // errno
#include <stdio.h>            // fprintf, stderr, FILE, fseek, ftell, SEEK_END, SEEK_SET, fopen, fclose, fread
#include <stdlib.h>           // calloc, free
#include <string.h>           // strerror


static void assert_err_not_nullptr (cl_int * err, int line);
static void platforms_check_error (cl_int status, cl_int * err);


static void assert_err_not_nullptr (cl_int * err, int line) {
    if (err == nullptr) {
        *err = line;
        fprintf(stderr, "ERROR %d: argument `err` shouldn't be nullptr\n", *err);
        return;
    }
}


void OCLH_arg_create_buffer (cl_context context, cl_mem_flags flags, size_t nbytes, cl_mem * meta, cl_int * err) {
    assert_err_not_nullptr(err, __LINE__);
    if (*err) return;

    // provide a description for constructing the memory object `meta` corresponding to `arr`
    *meta = clCreateBuffer(context, flags, nbytes, nullptr, err);
}


void OCLH_arg_create_image (cl_context context, cl_mem_flags flags, const cl_image_format * image_format,
                                 const cl_image_desc * image_desc, cl_mem * meta, cl_int * err) {
    assert_err_not_nullptr(err, __LINE__);
    if (*err) return;

    // provide a description for constructing the memory object `meta` corresponding to `arr`
    *meta = clCreateImage(context, flags, image_format, image_desc, nullptr, err);
}


void OCLH_arg_destroy (cl_mem meta) {
    clReleaseMemObject(meta);
}


void OCLH_arg_enqueue_reading_buffer (cl_command_queue queue, cl_mem meta, size_t nbytes, void * arg, cl_int * err) {
    assert_err_not_nullptr(err, __LINE__);
    if (*err) return;

    cl_bool isblocking = CL_TRUE;
    size_t offset = 0;
    cl_uint num_events_in_waitlist = 0;
    const cl_event * event_wait_list = nullptr;
    cl_event * event = nullptr;

    *err = clEnqueueReadBuffer(queue, meta, isblocking, offset, nbytes, arg, num_events_in_waitlist, event_wait_list, event);

    if (*err) {
        fprintf(stderr, "ERROR %d: problem enqueueing reading a buffer\n", *err);
    }
}


void OCLH_arg_enqueue_reading_image (cl_command_queue queue, cl_mem meta, const cl_image_desc * desc, void * arg, cl_int * err) {
    assert_err_not_nullptr(err, __LINE__);
    if (*err) return;

    cl_bool isblocking = CL_TRUE;
    const size_t origin[3] = {0, 0, 0};
    const size_t region[3] = {desc->image_width, desc->image_height, 1};
    size_t input_row_pitch = 0;
    size_t input_slice_pitch = 0;
    cl_uint num_events_in_waitlist = 0;
    const cl_event * event_wait_list = nullptr;
    cl_event * event = nullptr;

    *err = clEnqueueReadImage(queue, meta, isblocking, &origin[0], &region[0], input_row_pitch, input_slice_pitch,
                              arg, num_events_in_waitlist, event_wait_list, event);

    if (*err) {
        fprintf(stderr, "ERROR %d: problem enqueueing reading an image\n", *err);
    }
}


void OCLH_arg_enqueue_writing_buffer (cl_command_queue queue, cl_mem meta, size_t nbytes, const void * arg, cl_int * err) {
    assert_err_not_nullptr(err, __LINE__);
    if (*err) return;

    cl_bool isblocking = CL_TRUE;
    size_t offset = 0;
    cl_uint num_events_in_waitlist = 0;
    const cl_event * event_wait_list = nullptr;
    cl_event * event = nullptr;

    *err = clEnqueueWriteBuffer(queue, meta, isblocking, offset, nbytes, arg, num_events_in_waitlist, event_wait_list, event);

    if (*err) {
        fprintf(stderr, "ERROR %d: problem enqueueing writing a buffer\n", *err);
    }
}


void OCLH_arg_enqueue_writing_image (cl_command_queue queue, cl_mem meta, const cl_image_desc * desc, const void * arg, cl_int * err) {
    assert_err_not_nullptr(err, __LINE__);
    if (*err) return;

    cl_bool isblocking = CL_TRUE;
    const size_t origin[3] = {0, 0, 0};
    const size_t region[3] = {desc->image_width, desc->image_height, 1};
    size_t input_row_pitch = 0;
    size_t input_slice_pitch = 0;
    cl_uint num_events_in_waitlist = 0;
    const cl_event * event_wait_list = nullptr;
    cl_event * event = nullptr;

    *err = clEnqueueWriteImage(queue, meta, isblocking, &origin[0], &region[0], input_row_pitch, input_slice_pitch,
                               arg, num_events_in_waitlist, event_wait_list, event);

    if (*err) {
        fprintf(stderr, "ERROR %d: problem enqueueing writing an image\n", *err);
    }
}


void OCLH_ctx_create (int ndevices, const cl_device_id * devices, cl_context * context, cl_int * err) {
    assert_err_not_nullptr(err, __LINE__);
    if (*err) return;
    cl_context_properties * properties = nullptr;
    void (*callback)(const char *, const void *, size_t,  void *) = nullptr;
    void * userdata = nullptr;
    *context = clCreateContext(properties, ndevices, devices, callback, userdata, (cl_int *) err);
}


void OCLH_ctx_destroy (cl_context context) {
    clReleaseContext(context);
}


void OCLH_dev_get (cl_device_id device, cl_device_info param, void ** value, cl_int * err) {
    size_t nbytes = 0;
    *err = clGetDeviceInfo(device, param, 0, nullptr, &nbytes);
    if (*err) {
        fprintf(stderr, "ERROR in call to clGetDeviceInfo on line %d of %s\n", __LINE__, __FILE__);
        return;
    }
    *value = calloc(nbytes, sizeof(char));
    if (*value == nullptr) {
        *err = __LINE__;
        fprintf(stderr, "ERROR %d: encountered error allocating dynamic memory for cl_device_info parameter\n", *err);
        return;
    }
    *err = clGetDeviceInfo(device, param, nbytes, *value, nullptr);
}


void OCLH_devs_count (cl_platform_id platform, int * ndevices, cl_int * err) {
    assert_err_not_nullptr(err, __LINE__);
    if (*err) return;
    cl_device_type device_type = CL_DEVICE_TYPE_ALL;
    clGetDeviceIDs(platform, device_type, 0, nullptr, (cl_uint *) ndevices);
}


void OCLH_devs_create (int ndevices, cl_device_id ** devices, cl_int * err) {
    assert_err_not_nullptr(err, __LINE__);
    if (*err) return;
    *devices = calloc(ndevices, sizeof(cl_device_id));
    if (*devices == nullptr) {
        *err = __LINE__;
        fprintf(stderr, "ERROR %d: problem allocating dynamic memory for device type array, aborting\n", *err);
        return;
    }
}


void OCLH_devs_destroy (int ndevices, cl_device_id ** devices) {
    if (*devices == nullptr) return;

    for (int i = 0; i < ndevices; i++) {
        clReleaseDevice((*devices)[i]);
    }

    free(*devices);
    *devices = nullptr;
}


void OCLH_devs_exist (int ndevices, cl_int * err) {
    assert_err_not_nullptr(err, __LINE__);
    if (*err) return;
    if (ndevices > 0) return;
    *err = __LINE__;
    fprintf(stderr, "ERROR %d: There are no devices on the platform, aborting\n", *err);
}


void OCLH_devs_populate (cl_platform_id platform, int ndevices, cl_device_id * devices, cl_int * err) {
    assert_err_not_nullptr(err, __LINE__);
    if (*err) return;
    cl_device_type device_type = CL_DEVICE_TYPE_ALL;
    clGetDeviceIDs(platform, device_type, (cl_uint) ndevices, &devices[0], nullptr);
}


void OCLH_knl_create (cl_program program, char * kernel_name, cl_kernel * kernel, cl_int * err) {
    assert_err_not_nullptr(err, __LINE__);
    if (*err) return;
    *kernel = clCreateKernel(program, kernel_name, err);
    if (*err) {
        fprintf(stderr, "ERROR %d: encountered problem in call to clCreateKernel\n", *err);
    }
}


void OCLH_knl_destroy (cl_kernel kernel) {
    clReleaseKernel(kernel);
}


void OCLH_knl_enqueue_execution (cl_command_queue queue, cl_kernel kernel, cl_uint ndims, const size_t * global_work_size,
                                const size_t * local_work_size, cl_int * err) {
    assert_err_not_nullptr(err, __LINE__);
    if (*err) return;

    const size_t * global_work_offset = nullptr;
    cl_uint num_events_in_waitlist = 0;
    const cl_event * event_wait_list = nullptr;
    cl_event * event = nullptr;

    *err = clEnqueueNDRangeKernel(queue, kernel, ndims, global_work_offset, global_work_size, local_work_size,
                                  num_events_in_waitlist, event_wait_list, event);

    if (*err) {
        fprintf(stderr, "ERROR %d: problem enqueueing kernel execution\n", *err);
    }
}


void OCLH_knl_set_arg (cl_kernel kernel, cl_uint iarg, size_t nbytes, const void * arg) {
    clSetKernelArg(kernel, iarg, nbytes, arg);
}


void OCLH_platforms_count (int * nplatforms, cl_int * err) {
    assert_err_not_nullptr(err, __LINE__);
    if (*err) return;
    cl_int status = clGetPlatformIDs(0, nullptr, (cl_uint *) nplatforms);
    platforms_check_error(status, err);
}


void OCLH_platforms_create (int nplatforms, cl_platform_id ** platforms, cl_int * err) {
    assert_err_not_nullptr(err, __LINE__);
    if (*err) return;
    *platforms = calloc(nplatforms, sizeof(cl_platform_id));
    if (*platforms == nullptr) {
        *err = __LINE__;
        fprintf(stderr, "ERROR %d: problem allocating dynamic memory for platforms array, aborting\n", *err);
        return;
    }
}


void OCLH_platforms_destroy (cl_platform_id ** platforms) {
    if (*platforms == nullptr) return;
    free(*platforms);
    *platforms = nullptr;
}


void OCLH_platforms_exist (int nplatforms, cl_int * err) {
    assert_err_not_nullptr(err, __LINE__);
    if (*err) return;
    if (nplatforms > 0) return;
    *err = __LINE__;
    fprintf(stderr, "ERROR %d: There are no OpenCL platforms, aborting\n", *err);
}


void OCLH_platforms_populate (int nplatforms, cl_platform_id * platforms, cl_int * err) {
    assert_err_not_nullptr(err, __LINE__);
    if (*err) return;
    cl_int status = clGetPlatformIDs((cl_uint) nplatforms, platforms, nullptr);
    platforms_check_error(status, err);
}

void OCLH_program_create (cl_context context, int ndevices, const cl_device_id * devices, const char * filename,
                          cl_program * program, const char * options, cl_int * err) {
    assert_err_not_nullptr(err, __LINE__);
    if (*err) return;

    if (ndevices != 1) {
        *err = __LINE__;
        fprintf(stderr, "ERROR %d: function assumes number of devices is 1\n", *err);
        return;
    }

    // read the source from `filename`
    char * sources[1] = {};
    {
        FILE * file = fopen(filename, "r");
        if (file == nullptr) {
            *err = __LINE__;
            fprintf(stderr, "ERROR %d: problem opening file '%s'\n%s, aborting\n", *err, filename, strerror(errno));
            return;
        }
        fseek(file, 0, SEEK_END);
        long nbytes = ftell(file);
        fseek(file, 0, SEEK_SET);

        sources[0] = calloc(nbytes + 1, sizeof(char));
        if (sources[0] == nullptr) {
            *err = __LINE__;
            fprintf(stderr, "ERROR %d: problem allocating dynamic memory for storing source of file '%s', aborting\n", *err, filename);
            fclose(file);
            return;
        }
        long nread = fread(sources[0], 1, nbytes, file);
        if (nread != nbytes) {
            *err = __LINE__;
            fprintf(stderr, "ERROR %d: encountered problem reading from '%s'\n", *err, filename);
            return;
        }
        sources[0][nbytes] = '\0';
        fclose(file);
    }

    // create the program from source
    {
        *program = clCreateProgramWithSource(context, 1, (const char **) sources, nullptr, err);
        if (*err != CL_SUCCESS) {
            fprintf(stderr, "ERROR %d: problem creating program from source\n", *err);
            return;
        }
        free(sources[0]);
        sources[0] = nullptr;
    }

    // compile and link the program
    {
        *err = clBuildProgram(*program, ndevices, devices, options, nullptr, nullptr);
        if (*err != CL_SUCCESS) {
            size_t nbytes;
            cl_program_build_info param = CL_PROGRAM_BUILD_LOG;
            clGetProgramBuildInfo(*program, devices[0], param, 0, nullptr, &nbytes);
            char * log = calloc(nbytes, sizeof(char));
            clGetProgramBuildInfo(*program, devices[0], param, nbytes, log, nullptr);
            fprintf(stderr, "ERROR %d CL_PROGRAM_BUILD_LOG:\n%s\naborting", *err, log);
            free(log);
            log = nullptr;
            return;
        }
    }
}


void OCLH_program_destroy (cl_program program) {
    clReleaseProgram(program);
}


void OCLH_queue_create (cl_context context, cl_device_id device, cl_command_queue * queue, cl_int * err) {
    assert_err_not_nullptr(err, __LINE__);
    if (*err) return;
    cl_command_queue_properties * properties = nullptr;
    *queue = clCreateCommandQueueWithProperties(context, device, properties, err);
}


void OCLH_queue_destroy (cl_command_queue queue) {
    clReleaseCommandQueue(queue);
}


void OCLH_queue_finish (cl_command_queue queue, cl_int * err) {
    assert_err_not_nullptr(err, __LINE__);
    if (*err) return;
    *err = clFinish(queue);
}


static void platforms_check_error (cl_int status, cl_int * err) {
    switch (status) {
        case CL_INVALID_VALUE: {
            *err = __LINE__;
            fprintf(stderr, "ERROR %d: Invalid value, aborting\n", *err);
            break;
        }
        case CL_OUT_OF_HOST_MEMORY: {
            *err = __LINE__;
            fprintf(stderr, "ERROR %d: out of host memory, aborting\n", *err);
            break;
        }
        case CL_SUCCESS: {
            break;
        }
        default: {
            *err = __LINE__;
            fprintf(stderr, "ERROR %d: unreachable, aborting\n", *err);
            break;
        }
    }
}
