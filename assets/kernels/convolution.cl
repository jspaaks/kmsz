__kernel void convolution (__read_only image2d_t input, __write_only image2d_t output,
                           int nrange, __constant float * filter, sampler_t sampler, global int * err) {
    #ifdef KMSZ_USE_KERNEL_ASSERTS
    if (*err != 0) return;
    if (nrange % 2 != 0) {
        // conditionally report the first error
        atomic_cmpxchg(err, 0, __LINE__);
        return;
    }
    #endif

    const int2 pos = {
        get_global_id(0),
        get_global_id(1)
    };
    int2 offset = {};
    float4 weighted = {};
    int i = 0;
    for (offset.y = -nrange; offset.y <= nrange; offset.y++) {
        for (offset.x = -nrange; offset.x <= nrange; offset.x++) {
            float4 pixel = read_imagef(input, sampler, pos + offset);
            weighted += pixel * filter[i];
            i++;
        }
    }
    write_imagef(output, pos, weighted);
}
