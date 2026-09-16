cmake_minimum_required(VERSION 3.23...3.28 FATAL_ERROR)

project(kmsz LANGUAGES C)

include(GNUInstallDirs)                                                                                                                                                                                                                                                                                         

option(KMSZ_WITH_ASAN "Whether to enable address sanitizing for executables" OFF)
option(KMSZ_USE_KERNEL_ASSERTS "Whether to enable kernel error reporting" OFF)

set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)

if (APPLE)
    list(APPEND CMAKE_INSTALL_RPATH @loader_path/../lib)
elseif(UNIX)
    list(APPEND CMAKE_INSTALL_RPATH $ORIGIN/../lib)
endif()

add_subdirectory(assets/kernels)
add_subdirectory(src/add)
add_subdirectory(src/histogram)
add_subdirectory(src/querying)
add_subdirectory(src/rotation)
