#include "program.h"
#include "opencl.h"
#include <errno.h>            // errno
#include <stdio.h>            // fprintf, stderr, FILE, fseek, ftell, SEEK_END, SEEK_SET, fopen, fclose, fread
#include <stdlib.h>           // calloc, free, exit
#include <string.h>           // strerror


void program_create (cl_context context, cl_device_id * device, const char * filename, cl_program * program, int * err) {
    if (err == nullptr) {
        *err = __LINE__;
        fprintf(stderr, "ERROR %d: argument `err` shouldn't be nullptr\n", *err);
        return;
    }
    if (*err) return;

    // read the source from `filename`
    char * source = nullptr;
    {
        FILE * file = fopen(filename, "r");
        if (file == nullptr) {
            *err = __LINE__;
            fprintf(stderr, "ERROR %d: problem opening file '%s'\n%s, aborting\n", *err, filename, strerror(errno));
            return;
        }
        fseek(file, 0, SEEK_END);
        long nbytes = ftell(file);
        fseek(file, 0, SEEK_SET);
        source = calloc(nbytes + 1, sizeof(char));
        if (source == nullptr) {
            *err = __LINE__;
            fprintf(stderr, "ERROR %d: problem allocating dynamic memory for storing source of file '%s', aborting\n", *err, filename);
            fclose(file);
            return;
        }
        fread(source, 1, nbytes, file);
        source[nbytes] = '\0';
        fclose(file);
    }

    // create the program from source
    {
        *program = clCreateProgramWithSource(context, 1, (const char **) &source, nullptr, err);
        if (*err != CL_SUCCESS) {
            fprintf(stderr, "ERROR %d: problem creating program from source\n", *err);
            return;
        }
        free(source);
        source = nullptr;
    }

    // compile and link the program
    {
        *err = clBuildProgram(*program, 1, device, nullptr, nullptr, nullptr);
        if (*err != CL_SUCCESS) {
            size_t nbytes;
            cl_program_build_info param = CL_PROGRAM_BUILD_LOG;
            clGetProgramBuildInfo(*program, *device, param, 0, nullptr, &nbytes);
            char * log = calloc(nbytes, sizeof(char));
            clGetProgramBuildInfo(*program, *device, param, nbytes, log, nullptr);
            fprintf(stderr, "ERROR %d CL_PROGRAM_BUILD_LOG:\n%s\naborting", *err, log);
            free(log);
            log = nullptr;
            return;
        }
    }
}


void program_destroy (cl_program program) {
    clReleaseProgram(program);
}
