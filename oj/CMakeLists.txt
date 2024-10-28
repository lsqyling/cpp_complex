
file(GLOB_RECURSE src_list LIST_DIRECTORIES false
        CONFIGURE_DEPENDS *.cpp
)

foreach (v_ ${src_list})

    get_better_target_name(target_name ${v_})
    add_executable("${target_name}" ${v_})
endforeach ()

unset(src_list)


