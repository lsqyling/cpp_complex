cmake_minimum_required(VERSION 3.20)

project(cpp_complex VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)


if (MSVC)
    add_compile_options("/utf-8")
endif ()
if (WIN32)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif ()

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "build type" FORCE)
endif ()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

find_package(co_context QUIET)
if (co_context_DIR)
    add_compile_definitions(HAS_CO_CONTEXT)
    set(co_context_link)
endif ()

# input: D:/dev/workspace/clion/cpp_complex/oj/pat/pat_121_151_a.cpp
# output: pat_pat_121_151_a
# implement as get_better_target_name
function(get_better_target_name out path)
    string(FIND ${path} "/" index0 REVERSE)
    string(SUBSTRING ${path} 0 ${index0} rest)
    string(FIND ${rest} "/" index1 REVERSE)
    math(EXPR index1 "${index1} + 1" OUTPUT_FORMAT DECIMAL)
    string(SUBSTRING ${path} ${index1} -1 rest)
    string(REGEX REPLACE "/" "_" rest ${rest})
    get_filename_component(rest ${rest} NAME_WE)
    set(${out} ${rest} PARENT_SCOPE)
endfunction()

add_subdirectory(alg_notes)
add_subdirectory(concurrency)
add_subdirectory(cpp20_advanced_programing)
add_subdirectory(cpp_core_guide_lines)
add_subdirectory(oj)
add_subdirectory(professional_cpp)
