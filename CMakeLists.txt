cmake_minimum_required(VERSION 3.22)
project(Kindpin C)

# CMAKE Do not use standard libs
set(CMAKE_CXX_IMPLICIT_LINK_LIBRARIES "")

# CMAKE Do not use standard PATH
set(CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES "")

set(CMAKE_CXX_FLAGS "-m64 -nostdlib -fno-builtin -Wall -Wextra -Wno-unused-parameter -I${CMAKE_SOURCE_DIR}/include -O3 -nodefaultlibs")

# Sources
file(GLOB_RECURSE SOURCES source/*.c)
file (GLOB_RECURSE HEADERS include/*.h)

include_directories(include)

# Static lib
add_library(kingpin-static ${SOURCES} ${HEADERS})

# Shared lib
add_library(kingpin SHARED ${SOURCES} ${HEADERS})