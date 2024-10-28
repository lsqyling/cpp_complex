file(GLOB_RECURSE src_list LIST_DIRECTORIES false
        CONFIGURE_DEPENDS *.cpp
)

foreach (v_ ${src_list})
    get_filename_component(target_name ${v_} NAME_WE)
    add_executable("core_guide_${target_name}" ${v_})
endforeach ()
unset(src_list)