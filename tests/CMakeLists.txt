add_executable(tests
    src/real-world/FullScreenPartialCoverTests.cpp
    src/real-world/ImageInContainerTests.cpp
    src/real-world/ScrollTests.cpp
    
    src/unit/Initialisers.cpp

    src/unit/components/TextTests.cpp

    src/unit/layouts/FlexBoxTests.cpp

    src/util/Display.cpp
    src/Runner.cpp
)

Include(FetchContent)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.15.1
)

FetchContent_MakeAvailable(Catch2)

target_link_libraries(tests 
    Catch2::Catch2
    alui
)

target_include_directories(tests PUBLIC ${ALLEGRO_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(tests allegro_ttf allegro_image)

# vim:ft=cmake
