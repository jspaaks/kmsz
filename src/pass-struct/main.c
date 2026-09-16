#include "oclh.h"            // OCLH_*  opencl helpers
#include "opencl.h"
#include <stdio.h>           // fprintf, stdout,
#include <stdlib.h>          // EXIT_SUCCESS, free, calloc, srand
#include <string.h>          // strlen, strcat, strcpy

struct dim {
    alignas(4) cl_int filter;
    cl_int image;
};

static void construct_kernel_relpath (const char * kernel_dir_relpath, const char * filename, char ** path, int * err);


static void construct_kernel_relpath (const char * kernel_dir_relpath, const char * filename, char ** path, int * err) {
    if (err == nullptr) return;
    if (*err) return;
    int nchars = strlen(kernel_dir_relpath) + 1 + strlen(filename) + 1;
    *path = calloc(nchars, sizeof(char));
    if (*path == nullptr) {
        *err = __LINE__;
        fprintf(stderr, "ERROR %d: problem allocating dynamic memory for kernel path, aborting\n", *err);
    }
    strcpy(*path, kernel_dir_relpath);
    strcat(*path, "/");
    strcat(*path, filename);
    (*path)[nchars - 1] = '\0';
}


int main (int argc, char * argv[]) {

    // declare the error code variable
    cl_int err = EXIT_SUCCESS;
    cl_int kernel_err = EXIT_SUCCESS;

    // declare variables that are mentioned in cleanup label
    cl_context context = {0};
    cl_kernel kernel = {0};
    cl_program program = {0};
    cl_command_queue queue = {0};

    cl_mem kernel_err_meta = {0};

    cl_platform_id * platforms = nullptr;
    cl_device_id * devices = nullptr;


    // parse the command line arguments
    if (argc != 2) {
        err = __LINE__;
        fprintf(stderr,
                "Usage: %s KERNELDIR\n"
                "\n"
                "    Pass a struct to the kernel to illustrate required alignment.\n"
                "\n"
                "    KERNELDIR  directory that holds the OpenCL kernel named\n"
                "               'pass-struct.cl' (can be relative to working directory)\n"
                "\n",
                argv[0]);
        goto cleanup;
    }
    const char * kernel_dir_relpath = argv[1];


#ifndef KMSZ_USE_KERNEL_ASSERTS
    err = __LINE__;
    fprintf(stderr, "ERROR %d: program is only useful when compilation variable KMSZ_USE_KERNEL_ASSERTS has been defined, aborting\n", err);
    goto cleanup;
#endif // KMSZ_USE_KERNEL_ASSERTS


    // define the number of rows in image and in filter
    struct dim nrows = (struct dim) {
        .filter = 3,
        .image = 800,
    };


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
        construct_kernel_relpath(kernel_dir_relpath, "pass-struct.cl", &path, &err);
        const char * options = nullptr;
#ifdef KMSZ_USE_KERNEL_ASSERTS
        options = "-DKMSZ_USE_KERNEL_ASSERTS";
#endif
        OCLH_program_create(context, ndevices, devices, path, &program, options, &err);
        free(path);
        path = nullptr;
        if (err) goto cleanup;
    }


    // initialize the kernel arguments
    {
        OCLH_arg_create_buffer(context, CL_MEM_READ_WRITE, sizeof(cl_int), &kernel_err_meta, &err);
    }

    // enqueue writing of buffer
    {
        OCLH_arg_enqueue_writing_buffer(queue, kernel_err_meta, sizeof(cl_int), (const void *) &kernel_err, &err);
    }

    // initialize the kernel
    {
        OCLH_knl_create(program, "pass_struct", &kernel, &err);
        OCLH_knl_set_arg(kernel, 0, sizeof(nrows), &nrows);
        OCLH_knl_set_arg(kernel, 1, sizeof(kernel_err_meta), &kernel_err_meta);

        cl_uint ndims = 1;
        const size_t global_work_size[3] = {1, 0, 0};
        const size_t local_work_size[3] = {1, 0, 0};
        OCLH_knl_enqueue_execution(queue, kernel, ndims, &global_work_size[0], &local_work_size[0], &err);
        if (err) goto cleanup;
    }


    // enqueue reading the output data
    {
        OCLH_arg_enqueue_reading_buffer(queue, kernel_err_meta, sizeof(cl_int), (void *) &kernel_err, &err);
        if (err) goto cleanup;
    }


    // wait for the queue to finish
    {
        OCLH_queue_finish(queue, &err);
        if (err) goto cleanup;
    }

    // report result
    {
        if (kernel_err) {
            fprintf(stderr, "ERROR %d: encountered problem inside kernel\n", kernel_err);
            goto cleanup;
        } else {
            fprintf(stdout, "kernel didn't report any problems\n");
        }
        if (err) goto cleanup;
    }


cleanup:
    OCLH_knl_destroy(kernel);
    OCLH_arg_destroy(kernel_err_meta);
    OCLH_program_destroy(program);
    OCLH_queue_destroy(queue);
    OCLH_ctx_destroy(context);
    OCLH_devs_destroy(ndevices, &devices);
    OCLH_platforms_destroy(&platforms);

    return err;
}
