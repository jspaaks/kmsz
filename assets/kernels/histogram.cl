__kernel void calc_histogram (const int nelems, __global const int * image, const int nbins, __global int * histogram) {

    int idim = 0;
    int gid0 = get_global_id(idim);
    int gsz0 = get_global_size(idim);
    int lid0 = get_local_id(idim);
    int lsz0 = get_local_size(idim);

    // asserts
    if (nbins != 256) return;

    // allocate shared local memory
    __local int local_histogram[256];

    // cooperative initialization
    for (int i = lid0; i < nbins; i += lsz0) {
        local_histogram[i] = 0;
    }
    barrier(CLK_LOCAL_MEM_FENCE);

    // cooperative local histogram calculation
    for (int i = gid0; i < nelems; i += gsz0) {
        int val = image[i];
        atomic_inc(&local_histogram[val]);
    }
    barrier(CLK_LOCAL_MEM_FENCE);

    // cooperative update of global histogram using local histogram
    for (int i = lid0; i < nbins; i += lsz0) {
        atomic_add(&histogram[i], local_histogram[i]);
    }
}
