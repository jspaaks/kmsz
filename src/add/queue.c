#include "queue.h"
#include "opencl.h"
#include <stdio.h>


void queue_create (cl_context context, cl_device_id device, cl_command_queue * queue, int * err) {
    if (err == nullptr) {
        *err = __LINE__;
        fprintf(stderr, "ERROR %d: argument `err` shouldn't be nullptr\n", *err);
        return;
    }
    if (*err) return;
    cl_command_queue_properties * properties = nullptr;
    *queue = clCreateCommandQueueWithProperties(context, device, properties, err);
}


void queue_destroy (cl_command_queue queue) {
    clReleaseCommandQueue(queue);
}
