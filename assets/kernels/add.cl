__kernel void add (const int n, global const int * a, global const int * b, global int * result) {
    int i = get_global_id(0);
    if (i < n) {
        result[i] = a[i] + b[i];
    }
}
