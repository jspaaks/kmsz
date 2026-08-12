#ifndef ADD_QUEUE_INCLUDED
#define ADD_QUEUE_INCLUDED
#include "opencl.h"

void queue_create (cl_context context, cl_device_id device, cl_command_queue * queue, int * err);
void queue_destroy (cl_command_queue queue);

#endif
