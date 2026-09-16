add_executable(tgt_exe_increment)

set_property(TARGET tgt_exe_increment PROPERTY OUTPUT_NAME increment)
set_property(TARGET tgt_exe_increment PROPERTY CMAKE_C_EXTENSIONS ON)
set_property(TARGET tgt_exe_increment PROPERTY CMAKE_C_STANDARD_REQUIRED ON)

target_compile_definitions(
    tgt_exe_increment
    PRIVATE
        $<$<CONFIG:Debug>:DEBUG>
        $<$<BOOL:${KMSZ_USE_KERNEL_ASSERTS}>:KMSZ_USE_KERNEL_ASSERTS>
)

target_compile_features(
    tgt_exe_increment
    PRIVATE
        c_std_23
)

target_compile_options(
    tgt_exe_increment
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
    tgt_exe_increment
    PRIVATE
        OpenCL
)

target_link_options(
    tgt_exe_increment
    PRIVATE
        -pie
        $<$<BOOL:${KMSZ_WITH_ASAN}>:-fsanitize=address>
)

target_sources(
    tgt_exe_increment
    PRIVATE
        oclh.c
        main.c
)

install(TARGETS tgt_exe_increment)
