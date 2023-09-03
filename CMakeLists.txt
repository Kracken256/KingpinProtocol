cmake_minimum_required(VERSION 3.22)
project(Kindpin C)

# CMAKE Do not use standard libs
set(CMAKE_C_IMPLICIT_LINK_LIBRARIES "")

set(CMAKE_C_IMPLICIT_INCLUDE_DIRECTORIES "")

# Project uses libeddsa found at https://github.com/phlay/libeddsa.git
find_library(LIBEDDSA_STATIC eddsa-static REQUIRED)

set(CMAKE_C_FLAGS "-m64 -std=c99 -nostdlib -fno-builtin -Wall -Wextra -Werror -Wno-unused-parameter -I${CMAKE_SOURCE_DIR}/include -O3 -nodefaultlibs -leddsa")

# Sources
file(GLOB_RECURSE SOURCES source/*.c)
file (GLOB_RECURSE HEADERS include/*.h)

file(GLOB_RECURSE TESTS test/*.c)

include_directories(include)

# Static lib
add_library(kingpin-static ${SOURCES} ${HEADERS})

# Shared lib
add_library(kingpin SHARED ${SOURCES} ${HEADERS})

target_link_libraries(kingpin-static PRIVATE ${LIBEDDSA_STATIC})

# Use default libs for tests
set(CMAKE_C_IMPLICIT_LINK_LIBRARIES "")
set(CMAKE_C_IMPLICIT_INCLUDE_DIRECTORIES "")

set(CMAKE_C_FLAGS "-m64 -std=c99 -Wall -Wextra -Werror -Wno-unused-parameter -I${CMAKE_SOURCE_DIR}/include -O3")

foreach(test ${TESTS})
    get_filename_component(test_name ${test} NAME_WE)
    add_executable(${test_name}-test ${test} ${CMAKE_BUILD_DIR/libkingpin-static.a})
    target_link_libraries(${test_name}-test kingpin-static  -lc)
    
endforeach()



