#ifndef ADD_PLATFORMS_INCLUDED
#define ADD_PLATFORMS_INCLUDED
#include "opencl.h"

void OCLH_argument_create (cl_context context, size_t nbytes, cl_mem_flags flags, cl_mem * meta, int *err);
void OCLH_argument_destroy (cl_mem meta);
void OCLH_argument_enqueue_reading (cl_command_queue queue, cl_mem meta, size_t nbytes, void * arg, int * err);
void OCLH_argument_enqueue_writing (cl_command_queue queue, cl_mem meta, size_t nbytes, const void * arg, int * err);
void OCLH_context_create (int ndevices, const cl_device_id * devices, cl_context * context, int * err);
void OCLH_context_destroy (cl_context context);
void OCLH_device_get (cl_device_id device, cl_device_info param, void ** value, int * err);
void OCLH_devices_count (cl_platform_id platform, int * ndevices, int * err);
void OCLH_devices_create (int ndevices, cl_device_id ** devices, int * err);
void OCLH_devices_destroy (int ndevices, cl_device_id ** devices);
void OCLH_devices_exist(int ndevices, int * err);
void OCLH_devices_populate (cl_platform_id platform, int ndevices, cl_device_id * devices, int * err);
void OCLH_kernel_create (cl_program program, char * kernel_name, cl_kernel * kernel, int * err);
void OCLH_kernel_destroy (cl_kernel kernel);
void OCLH_kernel_enqueue_execution (cl_command_queue queue, cl_kernel kernel, const size_t * global_work_size, const size_t * local_work_size, int * err);
void OCLH_kernel_set_arg (cl_kernel kernel, cl_uint iarg, size_t nbytes, const void * arg);
void OCLH_platforms_count (int * nplatforms, int * err);
void OCLH_platforms_create (int nplatforms, cl_platform_id ** platforms, int * err);
void OCLH_platforms_destroy (cl_platform_id ** platforms);
void OCLH_platforms_exist (int nplatforms, int * err);
void OCLH_platforms_populate (int nplatforms, cl_platform_id * platforms, int * err);
void OCLH_program_create (cl_context context, int ndevices, const cl_device_id * devices, const char * filename, cl_program * program, int * err);
void OCLH_program_destroy (cl_program program);
void OCLH_queue_create (cl_context context, cl_device_id device, cl_command_queue * queue, int * err);
void OCLH_queue_destroy (cl_command_queue queue);
void OCLH_queue_finish (cl_command_queue queue, int * err);

#endif
