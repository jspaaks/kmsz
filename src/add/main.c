#include "array.h"
#include "context.h"
#include "devices.h"
#include "opencl.h"
#include "kernel.h"
#include "kernel_arg.h"
#include "platforms.h"
#include "program.h"
#include "queue.h"
#include <stdlib.h>          // exit, EXIT_SUCCESS, calloc, free
#include <stdio.h>           // fprintf, stderr
#include <string.h>          // strcat, strcpy, strlen


int main (int argc, char * argv[]) {

    // declare variables
    int err = EXIT_SUCCESS;
    int ndevices = -1;
    int nelems = 2048;
    int nplatforms = -1;
    size_t nbytes = nelems * sizeof(int);

    int * a = nullptr;
    cl_mem a_meta = {0};

    int * b = nullptr;
    cl_mem b_meta = {0};

    int * c = nullptr;
    cl_mem c_meta = {0};

    cl_context context = {0};
    cl_device_id device = {0};
    cl_kernel kernel = {0};
    cl_platform_id platform = {0};
    cl_program program = {0};
    cl_command_queue queue = {0};

    cl_device_id * devices = nullptr;
    cl_platform_id * platforms = nullptr;


    // parse the command line arguments
    if (argc != 2) {
        err = __LINE__;
        fprintf(stderr,
                "Usage: %s KERNELDIR\n"
                "\n"
                "    Vector addition using the first OpenCL capable device found\n"
                "\n"
                "    KERNELDIR  directory that holds the OpenCL kernel\n"
                "               files (can be a relative path)\n",
                argv[0]);
        goto cleanup;
    }
    const char * kernel_dir = argv[1];


    // identify the list of platforms and arbitrarily pick the first platform
    {
        platforms_create(&nplatforms, &platforms, &err);
        platforms_verify_exist(nplatforms, &err);
        if (err) goto cleanup;
        platform = platforms[0];
    }


    // identify the list of devices and arbitrarily pick the first device
    {
        devices_create(platform, &ndevices, &devices, &err);
        devices_verify_exist(ndevices, &err);
        if (err) goto cleanup;
        device = devices[0];
    }


    // create a context that has only the device we just picked
    {
        int npicked = 1;
        context_create(npicked, (const cl_device_id *) &device, &context, &err);
        if (err) goto cleanup;
    }


    // create a command queue for the chosen device
    {
        queue_create(context, device, &queue, &err);
        if (err) goto cleanup;
    }


    // create the program and the kernel
    {
        const char * filename = "add.cl";
        int nchars = strlen(kernel_dir) + 1 + strlen(filename) + 1;
        char * path = calloc(nchars, sizeof(char));
        if (path == nullptr) {
            err = __LINE__;
            fprintf(stderr, "ERROR %d: problem allocating dynamic memory for kernel path, aborting\n", err);
            goto cleanup;
        }
        strcpy(path, kernel_dir);
        strcat(path, "/");
        strcat(path, filename);
        path[nchars - 1] = '\0';
        program_create(context, &device, path, &program, &err);
        free(path);
        if (err) goto cleanup;
    }


    // create arrays on the host
    {
        array_create(nelems, &a, &err);
        array_create(nelems, &b, &err);
        array_create(nelems, &c, &err);
        array_init(nelems, &a, &err);
        array_init(nelems, &b, &err);
        if (err) goto cleanup;
    }


    // create kernel argument metadata corresponding to host arrays
    {
        kernel_arg_create_metadata(context, nbytes, CL_MEM_READ_ONLY, &a_meta, &err);
        kernel_arg_create_metadata(context, nbytes, CL_MEM_READ_ONLY, &b_meta, &err);
        kernel_arg_create_metadata(context, nbytes, CL_MEM_WRITE_ONLY, &c_meta, &err);
        if (err) goto cleanup;
    }


    // enqueue writing the input data to the device
    {
        kernel_arg_enqueue_writing (queue, a_meta, nbytes, (const int *) a, &err);
        kernel_arg_enqueue_writing (queue, b_meta, nbytes, (const int *) b, &err);
    }


    // create the kernel, specify how it must be parameterized, and enqueue it for execution
    {
        kernel_create(program, "add", &kernel, &err);
        kernel_set_arg(kernel, 0, sizeof(int), &nelems);
        kernel_set_arg(kernel, 1, sizeof(a_meta), &a_meta);
        kernel_set_arg(kernel, 2, sizeof(b_meta), &b_meta);
        kernel_set_arg(kernel, 3, sizeof(c_meta), &c_meta);

        const size_t global_work_size[3] = {nelems, 0, 0};
        const size_t local_work_size[3] = {128, 0, 0};

        kernel_enqueue_execution(queue, kernel, &global_work_size[0], &local_work_size[0], &err);

        if (err) goto cleanup;
    }


    // enqueue reading the output data
    {
        kernel_arg_enqueue_reading(queue, c_meta, nbytes, c, &err);
    }

    // verify the result
    {
        int n = 3;
        fprintf(stdout, "first %d elements:\n", n);
        for (int i = 0; i < n; i++) {
            fprintf(stdout, "%d%c", c[i], i == n - 1 ? '\n' : ' ');
        }
        fprintf(stdout, "last %d elements:\n", n);
        for (int i = nelems - n; i < nelems; i++) {
            fprintf(stdout, "%d%c", c[i], i == nelems - 1 ? '\n' : ' ');
        }
    }


cleanup:
    kernel_destroy(kernel);
    kernel_arg_destroy_metadata(c_meta);
    kernel_arg_destroy_metadata(b_meta);
    kernel_arg_destroy_metadata(a_meta);
    array_destroy(&c);
    array_destroy(&b);
    array_destroy(&a);
    program_destroy(program);
    queue_destroy(queue);
    context_destroy(context);
    devices_destroy(ndevices, &devices);
    platforms_destroy(&platforms);
    return err;
}
