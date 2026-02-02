if (ALUI_ENABLE_DEMOS)
    add_compile_definitions(
        -DALUI_DEBUG_SHAPES=1
    )
endif()

if (ALUI_BUILD_TESTS)
    add_compile_definitions(
        -DALUI_TEST=1
    )
endif()

add_library(alui STATIC
    alui/GUI.cpp
    alui/Component.cpp
    alui/Layout.cpp

    alui/components/Text.cpp
    alui/style/BackgroundColourStyle.cpp
    alui/layouts/FlexBox.cpp
)

target_include_directories(alui PUBLIC .)

target_link_libraries(alui PUBLIC
    allegro
    allegro_primitives
    allegro_font
    allegro_color
)

# vim:ft=cmake
