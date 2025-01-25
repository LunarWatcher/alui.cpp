add_executable(tests
    src/unit/components/TextTests.cpp

    src/Runner.cpp
)

Include(FetchContent)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.8.0
)

FetchContent_MakeAvailable(Catch2)

target_link_libraries(tests 
    Catch2::Catch2
    alui
)

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

target_include_directories(tests PUBLIC ${ALLEGRO_INCLUDE_DIRS})
target_link_libraries(tests allegro_ttf)

configure_file(3p/dejavu.ttf ${CMAKE_BINARY_DIR}/dejavu.ttf COPYONLY)
# vim:ft=cmake
