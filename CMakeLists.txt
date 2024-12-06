cmake_minimum_required(VERSION 3.29)
project(untitled C)

set(CMAKE_C_STANDARD 23)

add_executable(untitled main.c
        student_library.c
        student_library.h
        file_library.h
        file_library.c
)
