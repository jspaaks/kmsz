install(
    FILES
        add.cl
        convolution.cl
        histogram.cl
        increment.cl
        pass-struct.cl
        rotate.cl
    DESTINATION ${CMAKE_INSTALL_DATADIR}/kmsz/assets/kernels
)
