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
    alui/Layout.cpp

    alui/component/Component.cpp

    alui/components/Text.cpp
    alui/components/ImageComponent.cpp

    alui/layouts/FlexBox.cpp

    alui/style/BackgroundColourStyle.cpp
    alui/style/BorderStyle.cpp
)

target_include_directories(alui PUBLIC .)

target_link_libraries(alui PUBLIC
    allegro
    allegro_primitives
    allegro_font
    allegro_color
    allegro_image
)

# vim:ft=cmake
