add_executable(tgt_exe_add)

set_property(TARGET tgt_exe_add PROPERTY OUTPUT_NAME add)
set_property(TARGET tgt_exe_add PROPERTY CMAKE_C_EXTENSIONS ON)
set_property(TARGET tgt_exe_add PROPERTY CMAKE_C_STANDARD_REQUIRED ON)

target_compile_definitions(
    tgt_exe_add
    PRIVATE
        $<$<CONFIG:Debug>:DEBUG>
)

target_compile_features(
    tgt_exe_add
    PRIVATE
        c_std_23
)

target_compile_options(
    tgt_exe_add
    PRIVATE
        -Wall
        -Wextra
        -pedantic
        -fPIE
        $<$<CONFIG:Debug>:-g>
        $<$<CONFIG:Debug>:-O0>
        $<$<BOOL:${KMSZ_WITH_ASAN}>:-fsanitize=address>
        $<$<CONFIG:Release>:-Werror>
)

target_link_libraries(
    tgt_exe_add
    PRIVATE
        OpenCL
)

target_link_options(
    tgt_exe_add
    PRIVATE
        -pie
        $<$<BOOL:${KMSZ_WITH_ASAN}>:-fsanitize=address>
)

target_sources(
    tgt_exe_add
    PRIVATE
        array.c
        context.c
        devices.c
        main.c
        kernel.c
        kernel_arg.c
        platforms.c
        program.c
        queue.c
)

install(TARGETS tgt_exe_add)
