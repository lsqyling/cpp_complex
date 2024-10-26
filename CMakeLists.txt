cmake_minimum_required(VERSION 3.20)

project(cpp_oj VERSION 1.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


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


add_subdirectory(oj)
add_subdirectory(alg_notes)
add_subdirectory(professional_cpp)
add_subdirectory(cpp20_advanced_programing)
add_subdirectory(cpp_core_guide_lines)
add_subdirectory(vcpkg_lib_apply)
add_subdirectory(concurrency)

#install dir
set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_LIST_DIR}/install)
install(FILES README.md LICENSE
        DESTINATION doc
)