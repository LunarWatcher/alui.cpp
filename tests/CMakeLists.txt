add_executable(tests
    src/unit/components/TextTests.cpp
    src/unit/layouts/FlexBoxTests.cpp

    src/util/Display.cpp

    src/bench/Layouts.cpp

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

target_include_directories(tests PUBLIC ${ALLEGRO_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(tests allegro_ttf allegro_image)

# vim:ft=cmake
