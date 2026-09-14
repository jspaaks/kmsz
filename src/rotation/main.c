#include "oclh.h"            // OCLH_*  opencl helpers
#include "bmp.h"             // bmp_* 
#include "opencl.h"
#include "timer.h"           // TIMER_*, struct timer
#include <inttypes.h>        // PRIu8
#include <math.h>            // log10, ceil
#include <stdint.h>          // uint8_t
#include <stdio.h>           // fprintf, stdout,
#include <stdlib.h>          // EXIT_SUCCESS, free, calloc, srand
#include <string.h>          // strlen, strcat, strcpy
#include <time.h>            // timespec


static void print_corner_values (int nrows, int ncols, uint8_t * image);


int main (int argc, char * argv[]) {

    // declare the error code variable
    int err = EXIT_SUCCESS;
    struct timer * timer = nullptr;
    double duration = -1;

    // declare variables that are mentioned in cleanup label
    cl_context context = {0};
    cl_device_id * devices = nullptr;
    uint8_t * image = nullptr;
    cl_mem image_meta = {0};
    cl_kernel kernel = {0};
    cl_platform_id * platforms = nullptr;
    cl_program program = {0};
    cl_command_queue queue = {0};
    uint8_t * rotated = nullptr;
    cl_mem rotated_meta = {0};

    // parse the command line arguments
    if (argc != 3) {
        err = __LINE__;
        fprintf(stderr,
                "Usage: %s IMAGE KERNELDIR\n"
                "\n"
                "    Rotate an image 45 degrees clockwise using the first OpenCL capable device found.\n"
                "    Output file name is IMAGE minus .bmp plus .out.bmp (overwrites if file exists).\n"
                "\n"
                "    IMAGE      path to an 8-bit grayscale BMP image (can be\n"
                "               relative to working directory). Image dimensions\n"
                "               should be a multiple of 4.\n"
                "\n"
                "    KERNELDIR  directory that holds the OpenCL kernel named\n"
                "               'rotate.cl' (can be relative to working directory)\n"
                "\n",
                argv[0]);
        goto cleanup;
    }
    const char * input_relpath = argv[1];
    const char * kernel_dir_relpath = argv[2];


    // read the 8-bit grayscale bitmap from file
    int nrows = -1;
    int ncols = -1;
    {
        bmp_read(input_relpath, &nrows, &ncols, &image, &err);
        if (nrows % 4 != 0) {
            err = __LINE__;
            fprintf(stderr, "ERROR %d: number of rows in input image should be a multiple of 4.\n", err);
            goto cleanup;
        }
        if (ncols % 4 != 0) {
            err = __LINE__;
            fprintf(stderr, "ERROR %d: number of columns in input image should be a multiple of 4.\n", err);
            goto cleanup;
        }
    }

    // prepare time instrumentation
    {
        timer = TIMER_create();
    }


    // initialize the result array `rotated`
    {
        rotated = calloc(nrows * ncols, sizeof(uint8_t));
        if (rotated == nullptr) {
            err = __LINE__;
            fprintf(stderr, "ERROR %d: encountered problem allocating dynamic memory for `rotated` array", err);
            goto cleanup;
        }
    }

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
        const char * filename = "rotate.cl";
        int nchars = strlen(kernel_dir_relpath) + 1 + strlen(filename) + 1;
        char * path = calloc(nchars, sizeof(char));
        if (path == nullptr) {
            err = __LINE__;
            fprintf(stderr, "ERROR %d: problem allocating dynamic memory for kernel path, aborting\n", err);
            goto cleanup;
        }
        strcpy(path, kernel_dir_relpath);
        strcat(path, "/");
        strcat(path, filename);
        path[nchars - 1] = '\0';
        OCLH_program_create(context, ndevices, devices, path, &program, &err);
        free(path);
        path = nullptr;
        if (err) goto cleanup;
    }


    // initialize the kernel arguments and enqueue writing them on the device
    cl_image_format format = (cl_image_format) {
        .image_channel_order = CL_R,
        .image_channel_data_type = CL_UNSIGNED_INT8,
    };
    cl_image_desc desc = (cl_image_desc) {
        .image_type = CL_MEM_OBJECT_IMAGE2D,
        .image_width = ncols,
        .image_height = nrows,
    };
    {
        OCLH_arg_create_image(context, CL_MEM_READ_ONLY, &format, &desc, &image_meta, &err);
        OCLH_arg_enqueue_writing_image(queue, image_meta, &desc, (const void *) image, &err);
        OCLH_arg_create_image(context, CL_MEM_WRITE_ONLY, &format, &desc, &rotated_meta, &err);
        OCLH_arg_enqueue_writing_image(queue, rotated_meta, &desc, (const void *) rotated, &err);
        if (err) goto cleanup;
    }


    // initialize the kernel
    float angle = M_PI * 45 / 180;
    {
        OCLH_knl_create(program, "rotate", &kernel, &err);
        OCLH_knl_set_arg(kernel, 0, sizeof(cl_int), &nrows);
        OCLH_knl_set_arg(kernel, 1, sizeof(cl_int), &ncols);
        OCLH_knl_set_arg(kernel, 2, sizeof(image_meta), &image_meta);
        OCLH_knl_set_arg(kernel, 3, sizeof(rotated_meta), &rotated_meta);
        OCLH_knl_set_arg(kernel, 4, sizeof(cl_float), &angle);

        cl_uint ndims = 2;
        const size_t global_work_size[3] = {nrows, ncols, 0};
        const size_t local_work_size[3] = {16, 16, 0};
        OCLH_knl_enqueue_execution(queue, kernel, ndims, &global_work_size[0], &local_work_size[0], &err);
        if (err) goto cleanup;
    }


    // enqueue reading the output data
    {
        OCLH_arg_enqueue_reading_image(queue, rotated_meta, &desc, (void *) rotated, &err);
        if (err) goto cleanup;
    }


    // wait for the queue to finish
    {
        OCLH_queue_finish(queue, &err);
        if (err) goto cleanup;
    }

    // note the time elapsed
    {
        duration = TIMER_elapsed(timer);
    }


    // verify
    {
        fprintf(stdout, "\nimage:\n");
        print_corner_values(nrows, ncols, &image[0]);
        fprintf(stdout, "\nrotated:\n");
        print_corner_values(nrows, ncols, &rotated[0]);
        fprintf(stdout, "\n");
    }


    // allocate memory for the output name and set it to the right value; then write the
    // rotated pixels to a new bmp file
    {
        fprintf(stdout, "input file   %s\n", input_relpath);

        int nchars = strlen(input_relpath) + 1;
        char * output_relpath = calloc(nchars + 4, sizeof(char));
        if (output_relpath == nullptr) {
            err = __LINE__;
            fprintf(stderr, "ERROR %d: encountered problem allocating dynamic memory for output relpath\n", err);
            goto cleanup;
        }
        strcpy(output_relpath, input_relpath);
        strcpy(&output_relpath[nchars - 5], ".out.bmp");
        fprintf(stdout, "output file  %s\n", output_relpath);
        bmp_write(output_relpath, nrows, ncols, rotated, &err);
        fprintf(stdout, "rotating the image took %.3f s (walltime)\n", duration);
    }

 
cleanup:
    TIMER_destroy(&timer);
    free(rotated);
    free(image);
    OCLH_knl_destroy(kernel);
    OCLH_arg_destroy(rotated_meta);
    OCLH_arg_destroy(image_meta);
    OCLH_program_destroy(program);
    OCLH_queue_destroy(queue);
    OCLH_ctx_destroy(context);
    OCLH_devs_destroy(ndevices, &devices);
    OCLH_platforms_destroy(&platforms);
    return err;
}


static void print_corner_values (int nrows, int ncols, uint8_t * image) {
    int ul = 0;
    int ur = ncols - 1;
    int ll = (nrows - 1) * ncols;
    int lr = nrows * ncols - 1;
    int ndigits = (int) (ceilf(log10f((float) (nrows * ncols))));
    fprintf(stdout, "nrows %*s  = %d\n", ndigits, "", nrows);
    fprintf(stdout, "ncols %*s  = %d\n", ndigits, "", ncols);
    fprintf(stdout, "image[%*d] = %" PRIu8 "\n", ndigits, ul, image[ul]);
    fprintf(stdout, "image[%*d] = %" PRIu8 "\n", ndigits, ur, image[ur]);
    fprintf(stdout, "image[%*d] = %" PRIu8 "\n", ndigits, ll, image[ll]);
    fprintf(stdout, "image[%*d] = %" PRIu8 "\n", ndigits, lr, image[lr]);
}
