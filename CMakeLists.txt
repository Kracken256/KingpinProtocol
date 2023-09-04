cmake_minimum_required(VERSION 3.22)
project(Kindpin C)

# Do not use standard libs
set(CMAKE_C_IMPLICIT_LINK_LIBRARIES "")
set(CMAKE_C_IMPLICIT_INCLUDE_DIRECTORIES "")

# Project uses libeddsa found at https://github.com/phlay/libeddsa.git
find_library(LIBEDDSA_STATIC eddsa-static REQUIRED)

# Sources
file(GLOB_RECURSE SOURCES source/*.c)
file(GLOB_RECURSE HEADERS include/*.h)
file(GLOB_RECURSE TESTS test/*.c)

include_directories(include)

# Create the kingpin library
add_library(kingpin ${SOURCES} ${HEADERS})

# Set compiler flags
target_compile_options(kingpin PRIVATE -m64 -std=c99 -Wall -Wextra -Werror -Wno-unused-parameter -I${CMAKE_SOURCE_DIR}/include -O3 -ffreestanding -fno-stack-protector -nostdlib)

# Link the kingpin library with libeddsa-static
target_link_libraries(kingpin ${LIBEDDSA_STATIC})

# Combine libkingpin.a and libeddsa-static.a into one libkingpin.a
add_custom_command(
    TARGET kingpin
    POST_BUILD
    COMMAND ${CMAKE_AR} -x ${LIBEDDSA_STATIC}
    COMMAND ${CMAKE_AR} -x $<TARGET_FILE:kingpin>
    COMMAND ${CMAKE_AR} -qc $<TARGET_FILE:kingpin> *.o
    COMMAND ${CMAKE_COMMAND} -E remove *.o
    COMMENT "Combining libkingpin.a and libeddsa.a into libkingpin.a"
)

# Install
install(TARGETS kingpin DESTINATION lib)
install(DIRECTORY include/ DESTINATION include/kingpin)
install(TARGETS kingpin DESTINATION lib)



# Use default libs for tests
set(CMAKE_C_IMPLICIT_LINK_LIBRARIES "")
set(CMAKE_C_IMPLICIT_INCLUDE_DIRECTORIES "")

# set(CMAKE_C_FLAGS "-m64 -std=c99 -Wall -Wextra -Werror -Wno-unused-parameter -I${CMAKE_SOURCE_DIR}/include -O3 ")

# foreach(test ${TESTS})
#     get_filename_component(test_name ${test} NAME_WE)
#     add_executable(${test_name}-test ${test} ${CMAKE_BUILD_DIR/libkingpin.a})
#     target_link_libraries(${test_name}-test kingpin) 
    
# endforeach()

# Build tests with different compiler flags
foreach(test ${TESTS})
    get_filename_component(test_name ${test} NAME_WE)
    add_executable(${test_name}-test ${test})
    target_link_libraries(${test_name}-test kingpin) 
    target_compile_options(${test_name}-test PRIVATE -m64 -std=c99 -Wall -Wextra -Werror -Wno-unused-parameter -I${CMAKE_SOURCE_DIR}/include -O3 )
endforeach()