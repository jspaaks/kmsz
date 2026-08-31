# kmsz

Notes and projects from Kaeli, Mistry, Schaa, and Zhang's _Heterogeneous Computing with OpenCL 2.0_

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

## `querying`

Query the device for some of its properties and print them

```console
$ ./dist/bin/querying
There is 1 OpenCL platform on the host:
   Platform 1 of 1: Intel(R) OpenCL Graphics
   There is 1 device on the platform:
      device 1 of 1
         CL_DEVICE_NAME               Intel(R) Iris(R) Xe Graphics
         CL_DEVICE_VENDOR             Intel(R) Corporation
         CL_DEVICE_VENDOR_ID          0x8086
         CL_DEVICE_VERSION            OpenCL 3.0 NEO 
         CL_DRIVER_VERSION            23.43.027642
         CL_DEVICE_OPENCL_C_VERSION   OpenCL C 1.2 
         CL_DEVICE_PROFILE            FULL_PROFILE
         CL_DEVICE_TYPE               CL_DEVICE_TYPE_GPU
```

## `add`

Vector addition using the first OpenCL capable device found.

```console
$ ./dist/bin/add
Usage: ./dist/bin/add KERNELDIR

    Vector addition using the first OpenCL capable device found

    KERNELDIR  directory that holds the OpenCL kernel
               files (can be a relative path)
```

```console
$ ./dist/bin/add ./dist/share/kmsz/assets/kernels
first 3 elements:
0 2 4
last 3 elements:
4090 4092 4094
```

## `histogram`

Use local memory and `atomic_inc` to calculate partial histograms, then use `atomic_add`
to aggregate local histograms into the global histogram. Finally, verify the result
against a serial implementation.

```console
$ ./dist/bin/histogram 
Usage: ./dist/bin/histogram KERNELDIR

    Calculate a histogram using the first OpenCL capable device found

    KERNELDIR  directory that holds the OpenCL kernel named
               'histogram.cl' (can be a relative path)
$ ./dist/bin/histogram ./dist/share/kmsz/assets/kernels/                                                                                                                                                                                                                                                     
1 platform                                                                                                                                                                                                                                                                                                      
1 device                                                                                                                                                                                                                                                                                                        
Intel(R) Iris(R) Xe Graphics                                                                                                                                                                                                                                                                                    
        96 CL_DEVICE_MAX_COMPUTE_UNITS                                                                                                                                                                                                                                                                          
        64 CL_DEVICE_PREFERRED_WORK_GROUP_SIZE_MULTIPLE                                                                                                                                                                                                                                                         
       512 CL_DEVICE_MAX_WORK_GROUP_SIZE                                                                                                                                                                                                                                                                        
   2073601 image size                                                                                                                                                                                                                                                                                           
   2074112 global work size (padded image size)                                                                                                                                                                                                                                                                 
       512 local work size (group size)                                                                                                                                                                                                                                                                         
      4051 ngroups                                                                                                                                                                                                                                                                                              
Histogram calculated successfully
```

## Address sanitizing

To use address sanitizing, you may need to install an extra dependency, e.g. like so:

```console
# (Ubuntu)
sudo apt install libasan8
```

The CMake variable `QUERYING_WITH_ASAN` can be used to enable address sanitizing on the
executable `querying`. `QUERYING_WITH_ASAN`'s value is `OFF` by default. To
enable it, configure the build via `ccmake ..`, or via a command line argument with:

```console
$ cmake -DQUERYING_WITH_ASAN=ON ..
```

## Acknowledgements

_This project was initialized using [Copier](https://pypi.org/project/copier) and the [copier-template-for-c-projects](https://github.com/jspaaks/copier-template-for-c-projects)._
