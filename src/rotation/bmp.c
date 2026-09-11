#include "bmp.h"
#include <inttypes.h>  // PRIx32
#include <limits.h>    // INT_MAX
#include <errno.h>     // errno
#include <stdio.h>     // fopen, fclose
#include <stdint.h>    // uint8_t
#include <stdlib.h>    // calloc
#include <string.h>    // strerror


static void assert_err_not_nullptr (int * err, int line);
static void assert_magic_bytes_ok (FILE * fp, const char * relpath, int * err);
static void calloc_pixels (int nelems, uint8_t ** pixels, int * err);
static void calloc_write_buffer (int nbytes, uint8_t ** buffer, int * err);
static void free_write_buffer (uint8_t ** buffer);
static void read_pixels (FILE * fp, const char * relpath, int start, int nelems, uint8_t * pixels, int * err);
static void read_ncols (FILE * fp, const char * relpath, int * ncols, int * err);
static void read_nrows (FILE * fp, const char * relpath, int * nrows, int * err);
static void read_start (FILE * fp, const char * relpath, int * start, int * err);
static void write_bits_per_pixel (uint8_t * buffer);
static void write_color_bitmasks (uint8_t * buffer);
static void write_color_palette (uint8_t * buffer);
static void write_color_space_type (uint8_t * buffer);
static void write_compression (uint8_t * buffer);
static void write_filesize (uint8_t * buffer, int nrows, int ncols);
static void write_header_size (uint8_t * buffer);
static void write_image_height (uint8_t * buffer, uint32_t nrows);
static void write_image_data (const char * relpath, int nbytes, uint8_t * buffer, int * err);
static void write_image_size (uint8_t * buffer, int nrows, int ncols);
static void write_image_width (uint8_t * buffer, uint32_t ncols);
static void write_ncolors (uint8_t * buffer);
static void write_pixels (int nrows, int ncols, const uint8_t * pixels, uint8_t * buffer);
static void write_planes (uint8_t * buffer);
static void write_signature (uint8_t * buffer);
static void write_start (uint8_t * buffer);
static void write_xpixels_per_meter (uint8_t * buffer, uint32_t pixels_per_meter);
static void write_ypixels_per_meter (uint8_t * buffer, uint32_t pixels_per_meter);


static void assert_err_not_nullptr (int * err, int line) {
    if (err == nullptr) {
        *err = line;
        fprintf(stderr, "ERROR %d: argument `err` shouldn't be nullptr\n", *err);
        return;
    }
}


static void assert_magic_bytes_ok (FILE * fp, const char * relpath, int * err) {
    assert_err_not_nullptr(err, __LINE__);
    if (*err) return;
    int status = fseek(fp, 0, SEEK_SET);
    if (status != 0) {
        *err = __LINE__;
        fprintf(stderr, "ERROR %d: encountered problem setting the file position indicator in file '%s'\n%s\n", *err, relpath, strerror(errno));
        return;
    }
    uint16_t magic = -1;
    size_t nbytes = 2;
    size_t nelems = 1;
    size_t n = fread(&magic, nbytes, nelems, fp);
    if (n != nelems) {
        *err = __LINE__;
        fprintf(stderr, "ERROR %d: encountered problem reading the magic bytes from file '%s'\n%s\n", *err, relpath, strerror(errno));
        return;
    }
    uint16_t magic_expected = 'B' << 0 | 'M' << 8;
    if (magic != magic_expected) {
        *err = __LINE__;
        fprintf(stderr, "ERROR %d: magic bytes (0x%" PRIx32 ") do not match the expected value (0x%" PRIx32 ")\n", *err, magic, magic_expected);
        return;
    }
}


void bmp_read (const char * relpath, int * nrows, int * ncols, uint8_t ** image, int * err) {
    assert_err_not_nullptr(err, __LINE__);
    if (*err) return;

    FILE * fp = nullptr;

    // open file
    {
        fp = fopen(relpath, "rb");
        if (fp == nullptr) {
            *err = __LINE__;
            fprintf(stderr, "ERROR %d: encountered problem trying to open file '%s' for reading\n%s\n", *err, relpath, strerror(errno));
            return;
        }
    }

    assert_magic_bytes_ok(fp, relpath, err);
    int start = -1;
    read_start(fp, relpath, &start, err);
    read_nrows(fp, relpath, nrows, err);
    read_ncols(fp, relpath, ncols, err);
    int nelems = *nrows * *ncols;
    calloc_pixels(nelems, image, err);
    read_pixels(fp, relpath, start, nelems, *image, err);

    // close file
    {
        int status = fclose(fp);
        fp = nullptr;
        if (status == EOF) {
            fprintf(stderr, "ERROR %d: encountered problem trying to close file '%s'\n%s\n", *err, relpath, strerror(errno));
            return;
        }
    }
}


void bmp_write (const char * output_relpath, int nrows, int ncols, const uint8_t * pixels, int * err) {
    assert_err_not_nullptr(err, __LINE__);
    if (*err) return;

    // declare variables
    uint8_t * buffer = nullptr;
    int nbytes = -1;

    // calculate the size of the buffer that we're going to write to file and allocate
    // the corresponding memory
    {
        nbytes = 2 + 3 * 4 + 15 * 4 + 36 + 7 * 4 + 256 * 4 + nrows * ncols;
        calloc_write_buffer(nbytes, &buffer, err);
        if (*err) return;
    }

    write_signature(buffer);
    write_filesize(buffer, nrows, ncols);
    write_start(buffer);
    write_header_size(buffer);
    write_image_width(buffer, ncols);
    write_image_height(buffer, nrows);
    write_planes(buffer);
    write_bits_per_pixel(buffer);
    write_compression(buffer);
    write_image_size(buffer, nrows, ncols);
    write_xpixels_per_meter(buffer, 11811);      // ~300 dpi
    write_ypixels_per_meter(buffer, 11811);      // ~300 dpi
    write_ncolors(buffer);
    write_color_bitmasks(buffer);
    write_color_space_type(buffer);
    write_color_palette(buffer);
    write_pixels(nrows, ncols, pixels, buffer);
    write_image_data(output_relpath, nbytes, buffer, err);
    free_write_buffer(&buffer);
}


static void calloc_pixels (int nelems, uint8_t ** pixels, int * err) {
    assert_err_not_nullptr(err, __LINE__);
    if (*err) return;
    *pixels = calloc(nelems, sizeof(uint8_t));
    if (*pixels == nullptr) {
        *err = __LINE__;
        fprintf(stderr, "ERROR %d: encountered problem allocating dynamic memory for pixels\n", *err);
        return;
    }
}


static void calloc_write_buffer (int nbytes, uint8_t ** buffer, int * err) {
    assert_err_not_nullptr(err, __LINE__);
    if (*err) return;
    *buffer = calloc(nbytes, sizeof(uint8_t));
    if (*buffer == nullptr) {
        *err = __LINE__;
        fprintf(stderr, "ERROR %d: encountered problem allocating dynamic memory for write buffer\n", *err);
        return;
    }
}


static void free_write_buffer (uint8_t ** buffer) {
    free(*buffer);
    *buffer = nullptr;
}


static void read_ncols (FILE * fp, const char * relpath, int * ncols, int * err) {
    assert_err_not_nullptr(err, __LINE__);
    if (*err) return;
    int offset = 18;
    int status = fseek(fp, offset, SEEK_SET);
    if (status != 0) {
        *err = __LINE__;
        fprintf(stderr, "ERROR %d: encountered problem setting the file position indicator in file '%s'\n%s\n", *err, relpath, strerror(errno));
        return;
    }
    uint32_t val = -1;
    size_t nbytes = 4;
    size_t nelems = 1;
    size_t n = fread(&val, nbytes, nelems, fp);
    if (n != nelems) {
        *err = __LINE__;
        fprintf(stderr, "ERROR %d: encountered problem reading the number of columns from file '%s'\n%s\n", *err, relpath, strerror(errno));
        return;
    }
    if (val > INT_MAX) {
        *err = __LINE__;
        fprintf(stderr, "ERROR %d: number of columns from file '%s' too large to cast\n", *err, relpath);
        return;
    }
    *ncols = (int) val;
}


static void read_nrows (FILE * fp, const char * relpath, int * nrows, int * err) {
    assert_err_not_nullptr(err, __LINE__);
    if (*err) return;
    int offset = 22;
    int status = fseek(fp, offset, SEEK_SET);
    if (status != 0) {
        *err = __LINE__;
        fprintf(stderr, "ERROR %d: encountered problem setting the file position indicator in file '%s'\n%s\n", *err, relpath, strerror(errno));
        return;
    }
    uint32_t val = -1;
    size_t nbytes = 4;
    size_t nelems = 1;
    size_t n = fread(&val, nbytes, nelems, fp);
    if (n != nelems) {
        *err = __LINE__;
        fprintf(stderr, "ERROR %d: encountered problem reading the number of rows from file '%s'\n%s\n", *err, relpath, strerror(errno));
        return;
    }
    if (val > INT_MAX) {
        *err = __LINE__;
        fprintf(stderr, "ERROR %d: number of rows from file '%s' too large to cast\n", *err, relpath);
        return;
    }
    *nrows = (int) val;
}


static void read_pixels (FILE * fp, const char * relpath, int start, int nelems, uint8_t * pixels, int * err) {
    assert_err_not_nullptr(err, __LINE__);
    if (*err) return;
    int status = fseek(fp, start, SEEK_SET);
    if (status != 0) {
        *err = __LINE__;
        fprintf(stderr, "ERROR %d: encountered problem setting the file position indicator in file '%s'\n%s\n", *err, relpath, strerror(errno));
        return;
    }
    size_t nbytes = 1;
    size_t n = fread(pixels, nbytes, nelems, fp);
    if (n != (size_t) nelems) {
        *err = __LINE__;
        fprintf(stderr, "ERROR %d: encountered problem reading the pixels from file '%s'\n%s\n", *err, relpath, strerror(errno));
        return;
    }
}


static void read_start (FILE * fp, const char * relpath, int * start, int * err) {
    assert_err_not_nullptr(err, __LINE__);
    if (*err) return;
    int offset = 10;
    int status = fseek(fp, offset, SEEK_SET);
    if (status != 0) {
        *err = __LINE__;
        fprintf(stderr, "ERROR %d: encountered problem setting the file position indicator in file '%s'\n%s\n", *err, relpath, strerror(errno));
        return;
    }
    uint32_t val = -1;
    size_t nbytes = 4;
    size_t nelems = 1;
    size_t n = fread(&val, nbytes, nelems, fp);
    if (n != nelems) {
        *err = __LINE__;
        fprintf(stderr, "ERROR %d: encountered problem reading the data start position from file '%s'\n%s\n", *err, relpath, strerror(errno));
        return;
    }
    if (val > INT_MAX) {
        *err = __LINE__;
        fprintf(stderr, "ERROR %d: data start position from file '%s' too large to cast\n", *err, relpath);
        return;
    }
    *start = (int) val;
}


static void write_bits_per_pixel (uint8_t * buffer) {
    int offset = 28;
    int nbytes = 2;
    uint16_t val = 8;
    memcpy(&buffer[offset], &val, nbytes);
}


static void write_color_bitmasks (uint8_t * buffer) {
    int offset = 54;
    int nbytes = 16;
    uint32_t val[4] = {0x00ff0000, 0x0000ff00, 0x000000ff, 0x00000000};
    memcpy(&buffer[offset], &val[0], nbytes);
}


static void write_color_space_type (uint8_t * buffer) {
    int offset = 70;
    int nbytes = 4;
    uint32_t val = 'B' << 8*0 | 'G' << 8*1 | 'R' << 8*2 | 's' << 8*3;
    memcpy(&buffer[offset], &val, nbytes);
}


static void write_compression (uint8_t * buffer) {
    int offset = 30;
    int nbytes = 4;
    uint32_t val = 0;
    memcpy(&buffer[offset], &val, nbytes);
}


static void write_image_data (const char * output_relpath, int nbytes, uint8_t * buffer, int * err) {
    FILE * fp;

     // open file
    {
        fp = fopen(output_relpath, "wb");
        if (fp == nullptr) {
            *err = __LINE__;
            fprintf(stderr, "ERROR %d: encountered problem trying to open file '%s' for writing\n%s\n", *err, output_relpath, strerror(errno));
            return;
        }
    }

    // write the binary data to `output_relpath`
    {
        size_t nwritten = fwrite(buffer, sizeof(char), nbytes, fp);
        if (nwritten != (size_t) nbytes) {
            *err = __LINE__;
            fprintf(stderr, "ERROR %d: incorrect number of bytes written to file '%s'\n", *err, output_relpath);
            return;
        }
    }

    // close file
    {
        int status = fclose(fp);
        fp = nullptr;
        if (status == EOF) {
            fprintf(stderr, "ERROR %d: encountered problem trying to close file '%s'\n%s\n", *err, output_relpath, strerror(errno));
            return;
        }
    }
}


static void write_image_size (uint8_t * buffer, int nrows, int ncols) {
    int offset = 34;
    int nbytes = 4;
    uint32_t val = nrows * ncols;
    memcpy(&buffer[offset], &val, nbytes);
}


static void write_filesize (uint8_t * buffer, int nrows, int ncols) {
    int offset = 2;
    int nbytes = 4;
    uint32_t val = 1162 + nrows * ncols * sizeof(uint8_t);
    memcpy(&buffer[offset], &val, nbytes);
}


static void write_header_size (uint8_t * buffer) {
    int offset = 14;
    int nbytes = 4;
    uint32_t val = 124;
    memcpy(&buffer[offset], &val, nbytes);
}


static void write_image_height (uint8_t * buffer, uint32_t nrows) {
    int offset = 22;
    int nbytes = 4;
    memcpy(&buffer[offset], &nrows, nbytes);
}


static void write_image_width (uint8_t * buffer, uint32_t ncols) {
    int offset = 18;
    int nbytes = 4;
    memcpy(&buffer[offset], &ncols, nbytes);
}


static void write_ncolors (uint8_t * buffer) {
    int offset = 46;
    int nbytes = 4;
    uint32_t val = 256;
    memcpy(&buffer[offset], &val, nbytes);
}


static void write_color_palette (uint8_t * buffer) {
    int offset = 138;
    int nbytes = 1;
    for (int icolor = 0; icolor < 256; icolor++) {
        for (int ichannel = 0; ichannel < 3; ichannel++) {
            uint8_t val = (uint8_t) icolor;
            memcpy(&buffer[offset+icolor*4+ichannel], &val, nbytes);
        }
    }
}


static void write_pixels (int nrows, int ncols, const uint8_t * pixels, uint8_t * buffer) {
    int offset = 1162;
    memcpy(&buffer[offset], &pixels[0], nrows*ncols);
}


static void write_planes (uint8_t * buffer) {
    int offset = 26;
    int nbytes = 2;
    uint16_t val = 1;
    memcpy(&buffer[offset], &val, nbytes);
}


static void write_signature (uint8_t * buffer) {
    int offset = 0;
    buffer[offset] = 'B';
    buffer[offset + 1] = 'M';
}


static void write_start (uint8_t * buffer) {
    int offset = 10;
    int nbytes = 4;
    uint32_t start = 1162;
    memcpy(&buffer[offset], &start, nbytes);
}


static void write_xpixels_per_meter (uint8_t * buffer, uint32_t pixels_per_meter) {
    int offset = 38;
    int nbytes = 4;
    memcpy(&buffer[offset], &pixels_per_meter, nbytes);
}


static void write_ypixels_per_meter (uint8_t * buffer, uint32_t pixels_per_meter) {
    int offset = 42;
    int nbytes = 4;
    memcpy(&buffer[offset], &pixels_per_meter, nbytes);
}

