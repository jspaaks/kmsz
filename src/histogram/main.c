#include "oclh.h"            // OCLH_*  opencl helpers
#include "opencl.h"
#include <stdio.h>           // fprintf, stdout,
#include <stdlib.h>          // EXIT_SUCCESS, free, calloc, srand
#include <string.h>          // strlen, strcat, strcpy
#include <time.h>            // time


static void populate_image (cl_int nelems, cl_int * image);
static void set_sizes (cl_device_id device, int * gws, int * lws, int * ngroups, int nelems, int * err);

int main (int argc, char * argv[]) {

    // declare variables
    int err = EXIT_SUCCESS;

    int ndevices = -1;
    int nplatforms = -1;

    cl_context context = {0};
    cl_kernel kernel = {0};
    cl_program program = {0};
    cl_command_queue queue = {0};
    cl_device_id * devices = nullptr;
    cl_platform_id * platforms = nullptr;

    const cl_int nelems = 1920*1080+1;
    cl_int image[nelems] = {};
    populate_image(nelems, &image[0]);
    cl_mem image_meta = {0};
    size_t image_nbytes = nelems * sizeof(image[0]);

    const cl_int nbins = 256;
    cl_int histogram[nbins] = {};
    cl_mem histogram_meta = {0};
    size_t histogram_nbytes = nbins * sizeof(histogram[0]);

    // initialize the pseudorandom number generator
    srand(time(nullptr));

    // parse the command line arguments
    if (argc != 2) {
        err = __LINE__;
        fprintf(stderr,
                "Usage: %s KERNELDIR\n"
                "\n"
                "    Calculate a histogram using the first OpenCL capable device found\n"
                "\n"
                "    KERNELDIR  directory that holds the OpenCL kernel named\n"
                "               'histogram.cl' (can be a relative path)\n",
                argv[0]);
        goto cleanup;
    }
    const char * kernel_dir = argv[1];


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
        OCLH_context_create(npicked, &devices[0], &context, &err);
        if (err) goto cleanup;
    }


    // initialize the device queue
    {
        OCLH_queue_create(context, devices[0], &queue, &err);
        if (err) goto cleanup;
    }


    // initialize the program
    {
        const char * filename = "histogram.cl";
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
        OCLH_program_create(context, ndevices, devices, path, &program, &err);
        free(path);
        if (err) goto cleanup;
    }


    // initialize the kernel arguments and enqueue writing them on the device
    {
        OCLH_argument_create(context, image_nbytes, CL_MEM_READ_ONLY, &image_meta, &err);
        OCLH_argument_enqueue_writing(queue, image_meta, image_nbytes, image, &err);
        OCLH_argument_create(context, histogram_nbytes, CL_MEM_WRITE_ONLY, &histogram_meta, &err);
        OCLH_argument_enqueue_writing(queue, histogram_meta, histogram_nbytes, histogram, &err);
        if (err) goto cleanup;
    }


    // initialize the kernel
    {
        OCLH_kernel_create(program, "calc_histogram", &kernel, &err);

        OCLH_kernel_set_arg(kernel, 0, sizeof(cl_int), &nelems);
        OCLH_kernel_set_arg(kernel, 1, sizeof(image_meta), &image_meta);
        OCLH_kernel_set_arg(kernel, 2, sizeof(cl_int), &nbins);
        OCLH_kernel_set_arg(kernel, 3, sizeof(histogram_meta), &histogram_meta);

        int gws = -1;
        int lws = -1;
        int ngroups = -1;

        set_sizes(devices[0], &gws, &lws, &ngroups, nelems, &err);

        const size_t global_work_size[3] = {gws, 0, 0};
        const size_t local_work_size[3] = {lws, 0, 0};

        OCLH_kernel_enqueue_execution(queue, kernel, &global_work_size[0], &local_work_size[0], &err);

        if (err) goto cleanup;
    }


    // enqueue reading the output data
    {
        OCLH_argument_enqueue_reading(queue, histogram_meta, histogram_nbytes, &histogram[0], &err);
        if (err) goto cleanup;
    }


    // wait for the queue to finish
    {
        OCLH_queue_finish(queue, &err);
        if (err) goto cleanup;
    }

    // verify answer
    {
        int expected_histogram[nbins] = {};
        for (int i = 0; i < nelems; i++) {
            cl_int val = image[i];
            expected_histogram[val]++;
        }
        for (int i = 0; i < nbins; i++) {
            int actual = histogram[i];
            int expected = expected_histogram[i];
            if (actual != expected) {
                err = __LINE__;
                fprintf(stderr, "Mismatch at index %d\n", i);
            }
        }
        if (err) goto cleanup;
        fprintf(stdout, "Histogram calculated successfully\n");
    }


cleanup:
    OCLH_kernel_destroy(kernel);
    OCLH_argument_destroy(histogram_meta);
    OCLH_argument_destroy(image_meta);
    OCLH_program_destroy(program);
    OCLH_queue_destroy(queue);
    OCLH_context_destroy(context);
    OCLH_devices_destroy(ndevices, &devices);
    OCLH_platforms_destroy(&platforms);
    return err;
}


static void populate_image (cl_int nelems, cl_int * image) {
    for (int i = 0; i < nelems; i++) {
        image[i] = rand() % 100;
    }
}


static void set_sizes (cl_device_id device, int * gws, int * lws, int * ngroups, int nelems, int * err) {
    if (err == nullptr) {
        *err = __LINE__;
        fprintf(stderr, "ERROR %d: argument `err` shouldn't be nullptr\n", *err);
        return;
    }
    if (*err) return;

    char * cl_device_name = nullptr;
    OCLH_device_get(device, CL_DEVICE_NAME, (void **) &cl_device_name, err);
    fprintf(stdout, "%s\n", cl_device_name);

    int * cl_device_max_compute_units = nullptr;
    OCLH_device_get(device, CL_DEVICE_MAX_COMPUTE_UNITS, (void **) &cl_device_max_compute_units, err);
    fprintf(stdout, "%10d CL_DEVICE_MAX_COMPUTE_UNITS\n", *cl_device_max_compute_units);

    int * cl_device_preferred_work_group_size_multiple = nullptr;
    OCLH_device_get(device, CL_DEVICE_PREFERRED_WORK_GROUP_SIZE_MULTIPLE, (void **) &cl_device_preferred_work_group_size_multiple, err);
    fprintf(stdout, "%10d CL_DEVICE_PREFERRED_WORK_GROUP_SIZE_MULTIPLE\n", *cl_device_preferred_work_group_size_multiple);

    int * cl_device_max_work_group_size = nullptr;
    OCLH_device_get(device, CL_DEVICE_MAX_WORK_GROUP_SIZE, (void **) &cl_device_max_work_group_size, err);
    fprintf(stdout, "%10d CL_DEVICE_MAX_WORK_GROUP_SIZE\n", *cl_device_max_work_group_size);

    *lws = *cl_device_max_work_group_size;
    *ngroups = (nelems + *lws - 1) / *lws;
    *gws = *ngroups * *lws;

    fprintf(stdout, "%10d image size\n", nelems);
    fprintf(stdout, "%10d global work size (padded image size)\n", *gws);
    fprintf(stdout, "%10d local work size (group size)\n", *lws);
    fprintf(stdout, "%10d ngroups\n", *ngroups);

    free(cl_device_name);
    cl_device_name = nullptr;

    free(cl_device_max_compute_units);
    cl_device_max_compute_units = nullptr;

    free(cl_device_preferred_work_group_size_multiple);
    cl_device_preferred_work_group_size_multiple = nullptr;

    free(cl_device_max_work_group_size);
    cl_device_max_work_group_size = nullptr;
}
