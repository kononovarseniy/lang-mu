cmake_minimum_required(VERSION 3.10)
project(lang_mu C)

set(CMAKE_C_STANDARD 11)

include_directories(.)
include_directories(stdlib)

add_custom_command(
        OUTPUT  ${CMAKE_CURRENT_SOURCE_DIR}/mu.scanner.c
        COMMAND flex -o ${CMAKE_CURRENT_SOURCE_DIR}/mu.scanner.c ${CMAKE_CURRENT_SOURCE_DIR}/mu.l
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/mu.l
)
add_custom_command(
        OUTPUT  ${CMAKE_CURRENT_SOURCE_DIR}/mu.parser.c
                ${CMAKE_CURRENT_SOURCE_DIR}/mu.parser.h
                ${CMAKE_CURRENT_SOURCE_DIR}/mu.parser.output
        COMMAND bison -v -d -o ${CMAKE_CURRENT_SOURCE_DIR}/mu.parser.c ${CMAKE_CURRENT_SOURCE_DIR}/mu.y
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/mu.y
)

add_executable(lang_mu
        mu.scanner.c
        mu.parser.c
        mu.parser.h
        #stdlib/longnum.c
        #stdlib/longnum.h
        stdlib/longnum_stub.c
        stdlib/longnum_stub.h
        stdlib/str.c
        stdlib/str.h
        context.c
        context.h
        exec.c
        exec.h
        exec_builtin.c
        exec_builtin.h
        exec_builtin_io.c
        exec_builtin_predicates.c
        exec_builtin_strings.c
        exec_eval.c
        exec_eval.h
        exec_make.c
        exec_make.h
        exec_math.c
        exec_math.h
        exec_predicates.c
        exec_predicates.h
        exec_types.h
        gc.c
        gc.h
        load.c
        load.h
        log.h
        main.c
        map.c
        map.h
        parser.c
        parser.h
        print.c
        print.h
        stree.c
        stree.h
        strutils.c
        strutils.h)

add_custom_command(
        TARGET lang_mu POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/stdlib/stdlib.mu
        ${CMAKE_CURRENT_BINARY_DIR}/stdlib/stdlib.mu)
