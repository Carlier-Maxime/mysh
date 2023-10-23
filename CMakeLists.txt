cmake_minimum_required(VERSION 3.22)
project(mysh C)

set(CMAKE_C_STANDARD 11)
if (CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DDEBUG)
endif()

add_executable(mysh src/main.c
        src/Error.h
        src/Error.c
        src/CommandParser.h
        src/CommandParser.c
        src/Command.h
        src/Command.c
        src/macro.h
        src/CommandFactory.h
        src/CommandFactory.c
        src/Environment.h
        src/Environment.c
        src/TokenMapper.h
        src/TokenMapper.c
)
