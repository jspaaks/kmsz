# Developer notes

## CMake

The project has been initialized with a [CMakeLists.txt](CMakeLists.txt)-based
configuration for building with CMake:

```console
# change into the build directory
$ cd build/

# generate the build files
$ cmake -DCMAKE_BUILD_TYPE=Debug ..

# build the project
$ cmake --build .

# install the project to <repo>/build/dist
$ cmake --install . --prefix dist/
```

## Address sanitizing

To use address sanitizing, you may need to install an extra dependency, e.g. like so:

```console
# (Ubuntu)
sudo apt install libasan8
```

The CMake variable `KMSZ_WITH_ASAN` can be used to enable address sanitizing on the
executables. `KMSZ_WITH_ASAN`'s value is `OFF` by default. To enable it, configure
the build via `ccmake ..`, or via a command line argument with:

```console
$ cmake -DKMSZ_WITH_ASAN=ON ..
```

## Kernel error reporting

The CMake variable `KMSZ_USE_KERNEL_ASSERTS` can be used to enable error reporting
from the kernels. `KMSZ_USE_KERNEL_ASSERTS`'s value is `OFF` by default. To enable it, configure
the build via `ccmake ..`, or via a command line argument with:

```
$ cmake -DKMSZ_USE_KERNEL_ASSERTS=ON ..
```

Inside the kernel, you can use regular `ifdef`-`endif` blocks to report errors, like so:

```opencl
__kernel void mykernel (const int nrows, const int ncols, __global int * err) {
    #ifdef KMSZ_USE_KERNEL_ASSERTS
    if (nrows % 4 != 0) {
        // conditionally report the first error
        atomic_cmpxchg(err, 0, __LINE__);
        return;
    }
    if (ncols % 4 != 0) {
        // conditionally report the first error
        atomic_cmpxchg(err, 0, __LINE__);
        return;
    }
    #endif

    /* actual kernel code here */

}
```
