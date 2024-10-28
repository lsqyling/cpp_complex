file(GLOB_RECURSE test_list
        LIST_DIRECTORIES false
        CONFIGURE_DEPENDS *.cpp)


foreach (v_ ${test_list})
    get_filename_component(target_name ${v_} NAME_WE)
    add_executable("concurrency_${target_name}" ${v_})
endforeach ()


