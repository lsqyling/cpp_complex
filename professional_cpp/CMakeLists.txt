
set(lib_src
        airline_ticket.cpp
        bank.cpp
        data_base.cpp
        employee.cpp
        logger.cpp
        spread_sheet.cpp)

add_library(common
        SHARED
        ${lib_src}
)

file(GLOB_RECURSE src_list LIST_DIRECTORIES false
        CONFIGURE_DEPENDS chapter*.cpp *test.cpp
)

foreach (v_ ${src_list})
    get_filename_component(target_name ${v_} NAME_WE)
    if (${target_name} MATCHES "test")
        add_executable(${target_name} ${v_})
        target_link_libraries(${target_name} PRIVATE common)
    else ()
        add_executable(${target_name} ${v_})
    endif ()
endforeach ()
unset(src_list)


