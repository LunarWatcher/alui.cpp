# Source allegro {{{
target_include_directories(alui PUBLIC ${ALLEGRO_INCLUDE_DIRS})
# }}}
# Demo executables {{{
function (demo_executable name file)
    add_executable(${name} src/${file})
    target_link_libraries(${name} 
        alui 
        allegro allegro_font allegro_ttf allegro_primitives allegro_color
    )

    if (ALUI_LINT)
        set_target_properties(${name} PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}")
    endif()
endfunction()

demo_executable(all-widgets AllWidgets.cpp)
demo_executable(component-hierarchy ComponentHierarchy.cpp)

# }}}
