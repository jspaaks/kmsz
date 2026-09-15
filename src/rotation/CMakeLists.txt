add_executable(tgt_exe_rotate)

set_property(TARGET tgt_exe_rotate PROPERTY OUTPUT_NAME rotate)
set_property(TARGET tgt_exe_rotate PROPERTY CMAKE_C_EXTENSIONS ON)
set_property(TARGET tgt_exe_rotate PROPERTY CMAKE_C_STANDARD_REQUIRED ON)

target_compile_definitions(
    tgt_exe_rotate
    PRIVATE
        $<$<CONFIG:Debug>:DEBUG>
)

target_compile_features(
    tgt_exe_rotate
    PRIVATE
        c_std_23
)

target_compile_options(
    tgt_exe_rotate
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
    tgt_exe_rotate
    PRIVATE
        m
        OpenCL
)

target_link_options(
    tgt_exe_rotate
    PRIVATE
        -pie
        $<$<BOOL:${KMSZ_WITH_ASAN}>:-fsanitize=address>
)

target_sources(
    tgt_exe_rotate
    PRIVATE
        bmp.c
        oclh.c
        main.c
        timer.c
)

install(TARGETS tgt_exe_rotate)
