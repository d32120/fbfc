cmake_minimum_required(VERSION 4.1)
project(bfc compiler)

set(CMAKE_C_STANDARD 11)

add_executable(bfc
        main.c
        compiler/compiler.c
        compiler/option_parser.c
        compiler/mmemory.c
        include/inih/ini.c
)