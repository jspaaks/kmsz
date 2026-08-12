#include "kernel_arg.h"
#include "opencl.h"
#include <stdio.h>


void kernel_arg_create_metadata (cl_context context, size_t nbytes, cl_mem_flags flags, cl_mem * meta, int * err) {
    if (err == nullptr) {
        *err = __LINE__;
        fprintf(stderr, "ERROR %d: argument `err` shouldn't be nullptr\n", *err);
        return;
    }
    if (*err) return;

    // provide a description for constructing the memory object `meta` corresponding to `arr`
    *meta = clCreateBuffer(context, flags, nbytes, nullptr, err);
}


void kernel_arg_destroy_metadata (cl_mem meta) {
    clReleaseMemObject(meta);
}


void kernel_arg_enqueue_reading (cl_command_queue queue, cl_mem meta, size_t nbytes, int * arg, int * err) {
    if (err == nullptr) {
        *err = __LINE__;
        fprintf(stderr, "ERROR %d: argument `err` shouldn't be nullptr\n", *err);
        return;
    }
    if (*err) return;

    cl_bool blocking_write = CL_TRUE;
    size_t offset = 0;
    cl_uint num_events_in_waitlist = 0;
    const cl_event * event_wait_list = nullptr;
    cl_event * event = nullptr;

    *err = clEnqueueReadBuffer(queue, meta, blocking_write, offset, nbytes, arg, num_events_in_waitlist, event_wait_list, event);

    if (*err) {
        fprintf(stderr, "ERROR %d: problem enqueueing reading a buffer\n", *err);
    }
}


void kernel_arg_enqueue_writing (cl_command_queue queue, cl_mem meta, size_t nbytes, const int * arg, int * err) {
    if (err == nullptr) {
        *err = __LINE__;
        fprintf(stderr, "ERROR %d: argument `err` shouldn't be nullptr\n", *err);
        return;
    }
    if (*err) return;

    cl_bool blocking_write = CL_FALSE;
    size_t offset = 0;
    cl_uint num_events_in_waitlist = 0;
    const cl_event * event_wait_list = nullptr;
    cl_event * event = nullptr;

    *err = clEnqueueWriteBuffer(queue, meta, blocking_write, offset, nbytes, arg, num_events_in_waitlist, event_wait_list, event);

    if (*err) {
        fprintf(stderr, "ERROR %d: problem enqueueing writing a buffer\n", *err);
    }
}

