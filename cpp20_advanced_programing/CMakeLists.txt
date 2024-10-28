#
#file(GLOB_RECURSE <variable> [FOLLOW_SYMLINKS]
#        [LIST_DIRECTORIES true|false] [RELATIVE <path>] [CONFIGURE_DEPENDS]
#        [<globbing-expressions>...])
file(GLOB_RECURSE src_list LIST_DIRECTORIES false
        CONFIGURE_DEPENDS *.cpp
)


foreach (v_ ${src_list})
    get_filename_component(t_ ${v_} NAME_WE)
    add_executable(${t_} ${v_})
    if (co_context_link)
        target_link_libraries(${t_} PRIVATE co_context::co_context)
    endif ()
endforeach ()
unset(src_list)
