cmake_minimum_required(VERSION 3.10)
project(lang_mu C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/generated)

add_custom_command(
        OUTPUT ${CMAKE_SOURCE_DIR}/generated/mu.scanner.c
        COMMAND flex -o ${CMAKE_SOURCE_DIR}/generated/mu.scanner.c ${CMAKE_SOURCE_DIR}/parsing/mu.l
        DEPENDS ${CMAKE_SOURCE_DIR}/parsing/mu.l
)
add_custom_command(
        OUTPUT ${CMAKE_SOURCE_DIR}/generated/mu.parser.c
        ${CMAKE_SOURCE_DIR}/generated/mu.parser.h
        ${CMAKE_SOURCE_DIR}/generated/mu.parser.output
        COMMAND bison -v -d -o ${CMAKE_SOURCE_DIR}/generated/mu.parser.c ${CMAKE_SOURCE_DIR}/parsing/mu.y
        DEPENDS ${CMAKE_SOURCE_DIR}/parsing/mu.y
)

add_executable(lang_mu
        generated/mu.scanner.c
        generated/mu.parser.c
        generated/mu.parser.h

        parsing/stree.c
        parsing/stree.h

        types/longint.c
        types/longint.h
        types/string.c
        types/string.h

        utils/log.h
        utils/map.c
        utils/map.h
        utils/strutils.c
        utils/strutils.h

        exec/builtin/control.c
        exec/builtin/evaluation.c
        exec/builtin/functions.h
        exec/builtin/io.c
        exec/builtin/list.c
        exec/builtin/math.c
        exec/builtin/predicates.c
        exec/builtin/string.c

        exec/exec.h
        exec/types.h
        exec/constructions.h
        exec/constructions.c
        exec/context.c
        exec/context.h
        exec/eval.c
        exec/eval.h
        exec/executor.c
        exec/executor.h
        exec/function.c
        exec/function.h
        exec/gc.c
        exec/gc.h
        exec/make.c
        exec/make.h
        exec/math.c
        exec/math.h
        exec/predicates.c
        exec/predicates.h

        main.c
        parser.c
        parser.h
        load.c
        load.h
        print.c
        print.h)

add_custom_command(
        TARGET lang_mu POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/stdlib/stdlib.mu
        ${CMAKE_CURRENT_BINARY_DIR}/stdlib/stdlib.mu)