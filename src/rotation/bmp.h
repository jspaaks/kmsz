#ifndef ROTATION_BMP_H_INCLUDED
#define ROTATION_BMP_H_INCLUDED
#include <stdint.h>    // uint8_t

void bmp_read (const char * relpath, int * nrows, int * ncols, uint8_t ** image, int * err);
void bmp_write (const char * relpath, int nrows, int ncols, const uint8_t * image, int * err);

#endif
