cmake_minimum_required(VERSION 3.16)

project(malloc_lab_docker C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

add_executable(mdriver
    malloc-lab/mdriver.c
    malloc-lab/mm.c
    malloc-lab/memlib.c
    malloc-lab/fsecs.c
    malloc-lab/fcyc.c
    malloc-lab/clock.c
    malloc-lab/ftimer.c
)

target_include_directories(mdriver PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/malloc-lab
)

target_compile_options(mdriver PRIVATE
    -Wall
)

add_custom_command(TARGET mdriver POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/malloc-lab/traces
        $<TARGET_FILE_DIR:mdriver>/traces
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/malloc-lab/short1-bal.rep
        ${CMAKE_CURRENT_SOURCE_DIR}/malloc-lab/short2-bal.rep
        $<TARGET_FILE_DIR:mdriver>
)
