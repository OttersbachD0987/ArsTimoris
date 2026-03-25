cmake_minimum_required(VERSION 3.10.0)
cmake_policy(SET CMP0091 NEW)

project(ArsTimorisProject VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

if (MSVC)
    # warning level 4
    #add_compile_options(/W4)
else()
    # additional warnings
    add_compile_options(-fpermissive)
endif()

add_subdirectory(ArsTimoris)