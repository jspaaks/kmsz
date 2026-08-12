#include "array.h"
#include <stdio.h>      // fprintf, stderr
#include <stdlib.h>     // calloc, free


void array_create (int nelems, int ** arr, int * err) {
    if (err == nullptr) {
        *err = __LINE__;
        fprintf(stderr, "ERROR %d: argument `err` shouldn't be nullptr\n", *err);
        return;
    }
    if (*err) return;
    *arr = calloc(nelems, sizeof(int));
    if (*arr == nullptr) {
        *err = __LINE__;
        fprintf(stderr, "ERROR %d: problem allocating dynamic memory for array\n", *err);
        return;
    }
}


void array_destroy (int ** arr) {
    free(*arr);
    *arr = nullptr;
}


void array_init (int nelems, int ** arr, int * err) {
    if (err == nullptr) {
        *err = __LINE__;
        fprintf(stderr, "ERROR %d: argument `err` shouldn't be nullptr\n", *err);
        return;
    }
    if (*err) return;
    for (int i = 0; i < nelems; i++) {
        (*arr)[i] = i;
    }
}
