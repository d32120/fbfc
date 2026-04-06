cmake_minimum_required(VERSION 4.1)
project(bfc C)

set(CMAKE_C_STANDARD 11)

add_executable(bfc
        main.c
        compiler.c
        option_parser.c
        include/inih/ini.c
)
