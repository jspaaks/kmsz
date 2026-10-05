#include "bmp.h"             // bmp_read
#include "oclh.h"            // OCLH_*  opencl helpers
#include <stdio.h>           // fprintf, stderr
#include <stdlib.h>
#include <string.h>          // strlen, strcat, strcpy


int main (int argc, char * argv[]) {
    // declare the error code variable
    cl_int err = EXIT_SUCCESS;
    cl_int kernel_err = 0;

    // declare opencl overhead variables
    cl_context context = {0};
    cl_device_id * devices = nullptr;
    cl_mem input_meta = {0};
    cl_mem output_meta = {0};
    cl_mem filter_meta = {0};
    cl_kernel kernel = {0};
    cl_mem kernel_err_meta = {0};
    int ndevices = -1;
    int nplatforms = -1;
    cl_platform_id * platforms = nullptr;
    cl_program program = {0};
    cl_command_queue queue = {0};

    // declare pointers to the input image and the output image
    uint8_t * input = nullptr;
    uint8_t * output = nullptr;

    // declare the image properties variables
    cl_image_format format = {0};
    cl_image_desc desc = {0};

    // define the weights of the gaussian blur filter
    // note: filter weights introduce bias, sum is 281
    constexpr cl_int nrange = 2;
    constexpr cl_int nfilter = (nrange * 2 + 1) * (nrange * 2 + 1);
    cl_float filter[nfilter] = {
        1.0f / 273.0f,  4.0f / 273.0f,  7.0f / 273.0f,  4.0f / 273.0f, 1.0f / 273.0f, 
        4.0f / 273.0f, 16.0f / 273.0f, 26.0f / 273.0f, 16.0f / 273.0f, 4.0f / 273.0f, 
        7.0f / 273.0f, 26.0f / 273.0f, 41.0f / 273.0f, 26.0f / 273.0f, 7.0f / 273.0f, 
        4.0f / 273.0f, 16.0f / 273.0f, 26.0f / 273.0f, 16.0f / 273.0f, 4.0f / 273.0f, 
        1.0f / 273.0f,  4.0f / 273.0f,  7.0f / 273.0f,  4.0f / 273.0f, 1.0f / 273.0f, 
    };

    // parse the command line arguments
    if (argc != 3) {
        err = __LINE__;
        fprintf(stderr,
                "Usage: %s IMAGE KERNELDIR\n"
                "\n"
                "    Apply a filter to IMAGE using the kernel from KERNELDIR with the first OpenCL\n"
                "    capable device found.\n"
                "\n"
                "    Output file name is IMAGE minus .bmp plus .filtered.bmp (overwrites if file exists).\n"
                "\n"
                "    IMAGE      path to an 8-bit grayscale BMP image (can be\n"
                "               relative to working directory). Image dimensions\n"
                "               should be a multiple of 4.\n"
                "\n"
                "    KERNELDIR  directory that holds the OpenCL kernel named\n"
                "               'convolution.cl' (can be relative to working directory)\n"
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
        bmp_read(input_relpath, &nrows, &ncols, &input, &err);
        if (err) {
            fprintf(stderr, "ERROR %d: something went wrong reading from bmp, aborting.\n", err);
            goto cleanup;
        }
        if (nrows % 4 != 0) {
            err = __LINE__;
            fprintf(stderr, "ERROR %d: number of rows in input image (%d) should be a multiple of 4.\n", err, nrows);
            goto cleanup;
        }
        if (ncols % 4 != 0) {
            err = __LINE__;
            fprintf(stderr, "ERROR %d: number of columns in input image (%d) should be a multiple of 4.\n", err, ncols);
            goto cleanup;
        }
    }


    // initialize the result array `output`
    {
        output = calloc(nrows * ncols, sizeof(uint8_t));
        if (output == nullptr) {
            err = __LINE__;
            fprintf(stderr, "ERROR %d: encountered problem allocating dynamic memory for `output` array", err);
            goto cleanup;
        }
    }

    // initialize the platforms
    {
        OCLH_platforms_count(&nplatforms, &err);
        OCLH_platforms_exist(nplatforms, &err);
        OCLH_platforms_create(nplatforms, &platforms, &err);
        OCLH_platforms_populate(nplatforms, &platforms[0], &err);
        if (err) goto cleanup;
        fprintf(stdout, "%d %s\n", nplatforms, nplatforms == 1 ? "platform" : "platforms");
    }


    // initialize the devices
    {
        OCLH_devices_count(platforms[0], &ndevices, &err);
        OCLH_devices_exist(ndevices, &err);
        OCLH_devices_create(ndevices, &devices, &err);
        OCLH_devices_populate(platforms[0], ndevices, devices, &err);
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
            const char * filename = "convolution.cl";
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
        const char * options = nullptr;
        OCLH_program_create(context, ndevices, devices, path, &program, options, &err);
        free(path);
        path = nullptr;
        if (err) goto cleanup;
    }


    // initialize the kernel arguments
    {
        format = (cl_image_format) {
            .image_channel_order = CL_R,
            .image_channel_data_type = CL_UNSIGNED_INT8,
        };
        desc = (cl_image_desc) {
            .image_type = CL_MEM_OBJECT_IMAGE2D,
            .image_width = ncols,
            .image_height = nrows,
        };
        OCLH_arg_create_image(context, CL_MEM_READ_ONLY, &format, &desc, &input_meta, &err);
        OCLH_arg_create_image(context, CL_MEM_WRITE_ONLY, &format, &desc, &output_meta, &err);
        OCLH_arg_create_buffer(context, CL_MEM_READ_WRITE, sizeof(cl_int), &kernel_err_meta, &err);
        OCLH_arg_create_buffer(context, CL_MEM_READ_ONLY, sizeof(cl_float) * (nrange * 2 + 1) * (nrange * 2 + 1), &filter_meta, &err);
    }


    // enqueue writing of kernel arguments on the device
    {
        OCLH_arg_enqueue_writing_image(queue, input_meta, &desc, (const void *) input, &err);
        OCLH_arg_enqueue_writing_image(queue, output_meta, &desc, (const void *) output, &err);                     // not strictly needed but ok
        OCLH_arg_enqueue_writing_buffer(queue, kernel_err_meta, sizeof(cl_int), (const void *) &kernel_err, &err);  // not strictly needed but ok
        OCLH_arg_enqueue_writing_buffer(queue, filter_meta, sizeof(cl_float) * nfilter, (const void *) &filter, &err);
        if (err) goto cleanup;
    }

    
    // initialize the kernel
    {
        // define how pixels are sampled in the kernel
        cl_sampler_properties properties[] = {
            CL_SAMPLER_NORMALIZED_COORDS, CL_FALSE,
            CL_SAMPLER_ADDRESSING_MODE,   CL_ADDRESS_CLAMP_TO_EDGE,
            CL_SAMPLER_FILTER_MODE,       CL_FILTER_NEAREST,
            0                             // The array must be terminated with 0
        };
        cl_sampler sampler = clCreateSamplerWithProperties(context, properties, &err);

        OCLH_kernel_create(program, "convolution", &kernel, &err);
        OCLH_kernel_set_arg(kernel, 0, sizeof(cl_mem), &input_meta);
        OCLH_kernel_set_arg(kernel, 1, sizeof(cl_mem), &output_meta);
        OCLH_kernel_set_arg(kernel, 2, sizeof(cl_int), &nrange);
        OCLH_kernel_set_arg(kernel, 3, sizeof(cl_mem), &filter_meta);
        OCLH_kernel_set_arg(kernel, 4, sizeof(cl_sampler), &sampler);
        OCLH_kernel_set_arg(kernel, 5, sizeof(cl_mem), &kernel_err_meta);

        cl_uint ndims = 2;
        const size_t global_work_size[3] = {ncols, nrows, 0};
        const size_t local_work_size[3] = {16, 16, 0};
        OCLH_kernel_enqueue_execution(queue, kernel, ndims, &global_work_size[0], &local_work_size[0], &err);
        if (err) goto cleanup;
    }


    // enqueue reading the output data
    {
        OCLH_arg_enqueue_reading_image(queue, output_meta, &desc, (void *) output, &err);
        OCLH_arg_enqueue_reading_buffer(queue, kernel_err_meta, sizeof(cl_int), (void *) &kernel_err, &err);
        if (err) goto cleanup;
    }


    // wait for the queue to finish
    {
        OCLH_queue_finish(queue, &err);
        if (kernel_err) {
            fprintf(stderr, "ERROR %d: encountered problem inside kernel\n", kernel_err);
            goto cleanup;
        }
        if (err) goto cleanup;
    }


    // allocate memory for the output name and set it to the right value; then write the
    // output to a new bmp file
    {
        fprintf(stdout, "input file   %s\n", input_relpath);
        int nchars = strlen(input_relpath) + 1;
        char * output_relpath = calloc(nchars + 9, sizeof(char));
        if (output_relpath == nullptr) {
            err = __LINE__;
            fprintf(stderr, "ERROR %d: encountered problem allocating dynamic memory for output relpath\n", err);
            goto cleanup;
        }
        strcpy(output_relpath, input_relpath);
        strcpy(&output_relpath[nchars - 5], ".filtered.bmp");
        fprintf(stdout, "output file  %s\n", output_relpath);
        bmp_write(output_relpath, nrows, ncols, output, &err);
        free(output_relpath);
    }


cleanup:

    free(output);
    free(input);    
    OCLH_queue_destroy(queue);
    OCLH_program_destroy(program);
    OCLH_platforms_destroy(&platforms);
    OCLH_devices_destroy(&devices);
    OCLH_arg_destroy(kernel_err_meta);
    OCLH_arg_destroy(output_meta);
    OCLH_arg_destroy(input_meta);
    OCLH_kernel_destroy(kernel);
    OCLH_ctx_destroy(context);
    
    return EXIT_SUCCESS;
}
