static float2 unrotate_vector (__private float2 in, float angle);


__kernel void rotate (const int nrows, const int ncols, __read_only image2d_t image, __write_only image2d_t rotated, float angle) {

    float2 dst = (float2) (get_global_id(1), get_global_id(0));

    if (dst.x > ncols) return;
    if (dst.y > nrows) return;

    float2 origin = (float2) ((float) ncols / 2, (float) nrows / 2);

    // express vector `dst` relative to `origin`
    float2 dst_translated = dst - origin;

    // unrotate the translated vector `dst_translated` by `angle` around `origin`
    float2 src_translated = unrotate_vector(dst_translated, angle);

    // express the vector vector `src` relative to the image's origin
    float2 src = src_translated + origin;

    // define how image is sampled: use pixel coordinates (CLK_NORMALIZED_COORDS_FALSE), use linear
    // interpolation to estimate pixel values at a given location based on the surrounding pixels;
    // return zero for coordinates outside the bounds of the image (CLK_ADDRESS_CLAMP)
    __constant sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_FILTER_LINEAR | CLK_ADDRESS_CLAMP;

    // read location `src` from image using the sampling settings from `sampler`
    float4 pixel = read_imagef(image, sampler, src);

    // write the sampled `pixel` into `rotated` at location `dst`
    write_imagef(rotated, convert_int2(dst), pixel);
}


static float2 unrotate_vector (__private float2 dst, float angle) {
    float sin_angle = sin(angle);
    float cos_angle = cos(angle);
    return (float2) (
        dst.x * cos_angle - dst.y * sin_angle,
        dst.x * sin_angle + dst.y * cos_angle
    );
}
