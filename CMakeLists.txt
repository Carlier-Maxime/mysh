cmake_minimum_required(VERSION 3.22)
project(mysh C)

set(CMAKE_C_STANDARD 11)
if (CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DDEBUG)
endif()

add_executable(mysh src/mysh/main.c
        src/Error.h
        src/Error.c
        src/mysh/CommandParser.h
        src/mysh/CommandParser.c
        src/mysh/Command.h
        src/mysh/Command.c
        src/macro.h
        src/mysh/CommandFactory.h
        src/mysh/CommandFactory.c
        src/mysh/Environment.h
        src/mysh/Environment.c
        src/mysh/TokenMapper.h
        src/mysh/TokenMapper.c
        src/test/test.c
)

add_executable(test src/test/test.c)