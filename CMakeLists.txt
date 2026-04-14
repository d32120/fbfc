cmake_minimum_required(VERSION 4.1)
project(bfc C)

set(CMAKE_C_STANDARD 11)

add_executable(bfc
        main.c
        c/compiler.c
        c/option_parser.c
        c/mmemory.c
        include/inih/ini.c
)