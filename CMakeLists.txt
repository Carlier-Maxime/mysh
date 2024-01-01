cmake_minimum_required(VERSION 3.22)
project(mysh C)

set(CMAKE_C_STANDARD 11)
if (CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DDEBUG)
endif()

add_executable(mysh src/mysh/main.c
        src/utils/Error.h
        src/utils/Error.c
        src/mysh/CommandParser.h
        src/mysh/CommandParser.c
        src/mysh/Command.h
        src/mysh/Command.c
        src/utils/macro.h
        src/mysh/CommandFactory.h
        src/mysh/CommandFactory.c
        src/mysh/Environment.h
        src/mysh/Environment.c
        src/mysh/TokenMapper.h
        src/mysh/TokenMapper.c
)

add_executable(test src/test/test.c
        src/utils/Error.h
        src/utils/Error.c
        src/utils/macro.h
)

add_executable(myls src/myls/myls.c
        src/myls/myls.h
        src/utils/Error.h
        src/utils/Error.c
        src/utils/macro.h
        src/myls/myls_sorter.h
        src/myls/myls_sorter.c
        src/myls/myls_utils.h
        src/myls/myls_utils.c
)

add_executable(myps src/myps/myps.c
        src/utils/Error.h
        src/utils/Error.c
        src/utils/macro.h
)