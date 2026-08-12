#ifndef ADD_ARRAY_INCLUDED
#define ADD_ARRAY_INCLUDED
#include "opencl.h"

void array_create (int nelems, int ** arr, int *err);
void array_destroy (int ** arr);
void array_init (int nelems, int ** arr, int * err);

#endif
