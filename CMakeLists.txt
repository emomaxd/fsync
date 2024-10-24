cmake_minimum_required(VERSION 3.8)

project(fsync)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE SRC
    ${CMAKE_SOURCE_DIR}/benchmarks/*.c
    ${CMAKE_SOURCE_DIR}/include/*.h
)

add_executable(fsync ${SRC})

target_include_directories(fsync PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(fsync
    pthread
)

add_compile_options(-Wall -Wextra -Wpedantic -O2 -march=native -flto)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
