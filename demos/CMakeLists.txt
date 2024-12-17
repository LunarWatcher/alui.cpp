# Source allegro {{{
set(WANT_TESTS OFF CACHE STRING "" FORCE)
set(WANT_EXAMPLES OFF CACHE STRING "" FORCE)
set(WANT_DEMO OFF CACHE STRING "" FORCE)
set(WANT_POPUP_EXAMPLES OFF CACHE STRING "" FORCE)
set(WANT_DOCS OFF CACHE STRING "" FORCE)
set(ALLEGRO_SDL OFF CACHE STRING "" FORCE)
set(WANT_NATIVE_DIALOG ON CACHE STRING "" FORCE)

set(WANT_SHARED ON CACHE STRING "" FORCE)

include(FetchContent)

FetchContent_Declare(allegro5
    GIT_REPOSITORY https://github.com/liballeg/allegro5
    GIT_TAG 5.2.10.0
    OVERRIDE_FIND_PACKAGE
    EXCLUDE_FROM_ALL
)

FetchContent_MakeAvailable(
    allegro5
)

# For some reason, Allegro's targets doesn't include header include dirs
# This probably isn't enough to cover everything either, it's apparently supposed to be covered by 
# `make install`.
set (ALLEGRO_INCLUDE_DIRS
    ${allegro5_SOURCE_DIR}/include/
    ${allegro5_BINARY_DIR}/include/
    ${allegro5_SOURCE_DIR}/addons/image/
    ${allegro5_SOURCE_DIR}/addons/ttf/
    ${allegro5_SOURCE_DIR}/addons/font/
    ${allegro5_SOURCE_DIR}/addons/color/
    ${allegro5_SOURCE_DIR}/addons/main/
    ${allegro5_SOURCE_DIR}/addons/primitives/
)


# Global include required for shit to cooperate. The demos run standalone anyway, so this won't pollute anything
include_directories(PUBLIC
    ${ALLEGRO_INCLUDE_DIRS}
)

target_include_directories(alui PUBLIC ${ALLEGRO_INCLUDE_DIRS})
# }}}
# Demo executables {{{
function (demo_executable name file)
    add_executable(${name} src/${file})
    target_link_libraries(${name} 
        alui 
        allegro allegro_font allegro_ttf allegro_primitives allegro_color
    )
endfunction()

demo_executable(all-widgets AllWidgets.cpp)

# }}}
