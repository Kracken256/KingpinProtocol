cmake_minimum_required(VERSION 3.22)
project(Kindpin C)

# CMAKE Do not use standard libs
set(CMAKE_CXX_IMPLICIT_LINK_LIBRARIES "")

# CMAKE Do not use standard PATH
set(CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES "")

find_library(LIBEDDSA_STATIC eddsa-static PATHS /usr/local/lib REQUIRED)

set(CMAKE_CXX_FLAGS "-m64 -std=c99 -nostdlib -fno-builtin -Wall -Wextra -Wno-unused-parameter -I${CMAKE_SOURCE_DIR}/include -O3 -nodefaultlibs -leddsa")

# Sources
file(GLOB_RECURSE SOURCES source/*.c)
file (GLOB_RECURSE HEADERS include/*.h)

file(GLOB_RECURSE TEST_SOURCES test/*.c)

include_directories(include)

# Static lib
add_library(kingpin-static ${SOURCES} ${HEADERS})

# Shared lib
add_library(kingpin SHARED ${SOURCES} ${HEADERS})

add_executable(test ${TEST_SOURCES} libkingpin-static.a )

target_link_libraries(test kingpin-static)


target_link_libraries(kingpin-static PRIVATE ${LIBEDDSA_STATIC})