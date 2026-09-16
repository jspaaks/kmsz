#include "oclh.h"            // OCLH_*  opencl helpers
#include "opencl.h"
#include <stdio.h>           // fprintf, stdout,
#include <stdlib.h>          // EXIT_SUCCESS, free, calloc, srand
#include <string.h>          // strlen, strcat, strcpy


int main (int argc, char * argv[]) {

    // declare the error code variable
    cl_int err = EXIT_SUCCESS;

    // declare variables that are mentioned in cleanup label
    cl_context context = {0};
    cl_device_id * devices = nullptr;
    cl_kernel kernel = {0};
    cl_platform_id * platforms = nullptr;
    cl_program program = {0};
    cl_command_queue queue = {0};
    cl_mem value_meta = {0};


    // the value that is going to be incremented
    cl_int value = 100;


    // parse the command line arguments
    if (argc != 2) {
        err = __LINE__;
        fprintf(stderr,
                "Usage: %s KERNELDIR\n"
                "\n"
                "    Increment an integer on the device.\n"
                "\n"
                "    KERNELDIR  directory that holds the OpenCL kernel named\n"
                "               'increment.cl' (can be relative to working directory)\n"
                "\n",
                argv[0]);
        goto cleanup;
    }
    const char * kernel_dir_relpath = argv[1];


    // initialize the platforms
    int nplatforms = -1;
    {
        OCLH_platforms_count(&nplatforms, &err);
        OCLH_platforms_exist(nplatforms, &err);
        OCLH_platforms_create(nplatforms, &platforms, &err);
        OCLH_platforms_populate(nplatforms, &platforms[0], &err);
        if (err) goto cleanup;
        fprintf(stdout, "%d %s\n", nplatforms, nplatforms == 1 ? "platform" : "platforms");
    }


    // initialize the devices
    int ndevices = -1;
    {
        OCLH_devs_count(platforms[0], &ndevices, &err);
        OCLH_devs_exist(ndevices, &err);
        OCLH_devs_create(ndevices, &devices, &err);
        OCLH_devs_populate(platforms[0], ndevices, devices, &err);
        if (err) goto cleanup;
        fprintf(stdout, "%d %s\n", ndevices, ndevices == 1 ? "device" : "devices");
    }


    // initialize the context
    {
        int npicked = 1;
        OCLH_ctx_create(npicked, &devices[0], &context, &err);
        if (err) goto cleanup;
    }


    // initialize the device queue
    {
        OCLH_queue_create(context, devices[0], &queue, &err);
        if (err) goto cleanup;
    }


    // initialize the program
    {
        char * path = nullptr;
        {
            const char * filename = "increment.cl";
            int nchars = strlen(kernel_dir_relpath) + 1 + strlen(filename) + 1;
            path = calloc(nchars, sizeof(char));
            if (path == nullptr) {
                err = __LINE__;
                fprintf(stderr, "ERROR %d: problem allocating dynamic memory for kernel path, aborting\n", err);
                goto cleanup;
            }
            strcpy(path, kernel_dir_relpath);
            strcat(path, "/");
            strcat(path, filename);
            path[nchars - 1] = '\0';
        }
        OCLH_program_create(context, ndevices, devices, path, &program, &err);
        free(path);
        path = nullptr;
        if (err) goto cleanup;
    }


    // initialize the kernel arguments
    {
        OCLH_arg_create_buffer(context, CL_MEM_READ_WRITE, sizeof(cl_int), &value_meta, &err);
    }

    // enqueue writing of buffer
    {
        OCLH_arg_enqueue_writing_buffer(queue, value_meta, sizeof(cl_int), (const void *) &value, &err);
    }

    // print the before value
    {
        fprintf(stdout, "before: %d\n", value);
    }

    // initialize the kernel
    {
        OCLH_knl_create(program, "increment", &kernel, &err);
        OCLH_knl_set_arg(kernel, 0, sizeof(value_meta), &value_meta);

        cl_uint ndims = 1;
        const size_t global_work_size[3] = {1, 0, 0};
        const size_t local_work_size[3] = {1, 0, 0};
        OCLH_knl_enqueue_execution(queue, kernel, ndims, &global_work_size[0], &local_work_size[0], &err);
        if (err) goto cleanup;
    }


    // enqueue reading the output data
    {
        OCLH_arg_enqueue_reading_buffer(queue, value_meta, sizeof(cl_int), (void *) &value, &err);
        if (err) goto cleanup;
    }


    // wait for the queue to finish
    {
        OCLH_queue_finish(queue, &err);
        if (err) goto cleanup;
    }

    // verify
    {
        fprintf(stdout, "after : %d\n", value);
    }


cleanup:
    OCLH_knl_destroy(kernel);
    OCLH_arg_destroy(value_meta);
    OCLH_program_destroy(program);
    OCLH_queue_destroy(queue);
    OCLH_ctx_destroy(context);
    OCLH_devs_destroy(ndevices, &devices);
    OCLH_platforms_destroy(&platforms);

    return err;
}
