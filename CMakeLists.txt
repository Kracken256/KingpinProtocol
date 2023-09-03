cmake_minimum_required(VERSION 3.22)
project(Kindpin C)

# CMAKE Do not use standard libs
set(CMAKE_CXX_IMPLICIT_LINK_LIBRARIES "")

set(CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES "")

# Project uses libeddsa found at https://github.com/phlay/libeddsa.git
find_library(LIBEDDSA_STATIC eddsa-static REQUIRED)

set(CMAKE_CXX_FLAGS "-m64 -std=c99 -g -nostdlib -fno-builtin -Wall -Wextra -Wno-unused-parameter -I${CMAKE_SOURCE_DIR}/include -O3 -nodefaultlibs -leddsa")

# Sources
file(GLOB_RECURSE SOURCES source/*.c)
file (GLOB_RECURSE HEADERS include/*.h)

include_directories(include)

# Static lib
add_library(kingpin-static ${SOURCES} ${HEADERS})

# Shared lib
add_library(kingpin SHARED ${SOURCES} ${HEADERS})

target_link_libraries(kingpin-static PRIVATE ${LIBEDDSA_STATIC})

add_executable(client-test test/client.c ${CMAKE_BUILD_DIR/libkingpin-static.a})
add_executable(server-test test/server.c ${CMAKE_BUILD_DIR/libkingpin-static.a})


target_link_libraries(client-test kingpin-static)
target_link_libraries(server-test kingpin-static)
