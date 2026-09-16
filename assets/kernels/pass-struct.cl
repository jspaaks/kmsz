// this struct definition needs to be exactly the same as how it is defined
// on the host, including order of members
struct __attribute__((aligned(4))) dims {
    int filter;
    int image;
};


__kernel void pass_struct (struct dims nrows, __global int * err) {
#ifdef KMSZ_USE_KERNEL_ASSERTS
    if (nrows.image != 800) {
        atomic_cmpxchg(err, 0, __LINE__);
        return;
    }
    if (nrows.filter != 3) {
        atomic_cmpxchg(err, 0, __LINE__);
        return;
    }
#endif
    /* body of the kernel would go here */
}
