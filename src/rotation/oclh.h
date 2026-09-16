#ifndef ADD_PLATFORMS_INCLUDED
#define ADD_PLATFORMS_INCLUDED
#include "opencl.h"

void OCLH_arg_create_buffer (cl_context context, size_t nbytes, cl_mem_flags flags, cl_mem * meta, cl_int * err);
void OCLH_arg_create_image (cl_context context, cl_mem_flags flags, const cl_image_format * image_format, const cl_image_desc * image_desc, cl_mem * meta, cl_int * err);
void OCLH_arg_destroy (cl_mem meta);
void OCLH_arg_enqueue_reading_buffer (cl_command_queue queue, cl_mem meta, size_t nbytes, void * arg, cl_int * err);
void OCLH_arg_enqueue_reading_image (cl_command_queue queue, cl_mem meta, const cl_image_desc * desc, void * arg, cl_int * err);
void OCLH_arg_enqueue_writing_buffer (cl_command_queue queue, cl_mem meta, size_t nbytes, const void * arg, cl_int * err);
void OCLH_arg_enqueue_writing_image (cl_command_queue queue, cl_mem meta, const cl_image_desc * desc, const void * arg, cl_int * err);
void OCLH_ctx_create (int ndevices, const cl_device_id * devices, cl_context * context, cl_int * err);
void OCLH_ctx_destroy (cl_context context);
void OCLH_dev_get (cl_device_id device, cl_device_info param, void ** value, cl_int * err);
void OCLH_devs_count (cl_platform_id platform, int * ndevices, cl_int * err);
void OCLH_devs_create (int ndevices, cl_device_id ** devices, cl_int * err);
void OCLH_devs_destroy (int ndevices, cl_device_id ** devices);
void OCLH_devs_exist(int ndevices, cl_int * err);
void OCLH_devs_populate (cl_platform_id platform, int ndevices, cl_device_id * devices, cl_int * err);
void OCLH_knl_create (cl_program program, char * kernel_name, cl_kernel * kernel, cl_int * err);
void OCLH_knl_destroy (cl_kernel kernel);
void OCLH_knl_enqueue_execution (cl_command_queue queue, cl_kernel kernel, cl_uint ndims, const size_t * global_work_size, const size_t * local_work_size, cl_int * err);
void OCLH_knl_set_arg (cl_kernel kernel, cl_uint iarg, size_t nbytes, const void * arg);
void OCLH_platforms_count (int * nplatforms, cl_int * err);
void OCLH_platforms_create (int nplatforms, cl_platform_id ** platforms, cl_int * err);
void OCLH_platforms_destroy (cl_platform_id ** platforms);
void OCLH_platforms_exist (int nplatforms, cl_int * err);
void OCLH_platforms_populate (int nplatforms, cl_platform_id * platforms, cl_int * err);
void OCLH_program_create (cl_context context, int ndevices, const cl_device_id * devices, const char * filename, cl_program * program, const char * options, cl_int * err);
void OCLH_program_destroy (cl_program program);
void OCLH_queue_create (cl_context context, cl_device_id device, cl_command_queue * queue, cl_int * err);
void OCLH_queue_destroy (cl_command_queue queue);
void OCLH_queue_finish (cl_command_queue queue, cl_int * err);

#endif
