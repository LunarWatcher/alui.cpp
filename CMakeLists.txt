cmake_minimum_required(VERSION 3.28)
project(alui.cpp)

option(ALUI_ENABLE_DEMOS "WHether or not to build demos" OFF)
option(ALUI_BUILD_TESTS "Whether or not to build tests. Forced ON if building standalone" OFF)
option(ALUI_DOCS "Whether or not to build doxygen docs" OFF)

if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    message (STATUS "alui: Running standalone")
    set(ALUI_STANDALONE ON)

    if (WIN32)
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
    endif()
    set(ALUI_ENABLE_DEMOS ON)
    set(ALUI_BUILD_TESTS ON)
    set(ALUI_DOCS ON)
else()
    message (STATUS "alui: Running as a submodule")
    set(ALUI_STANDALONE OFF)
endif()

option(ALUI_LINT "Whether or not to run clang-tidy" OFF)
option(LINT_WARNINGS_ARE_ERRORS "Whether or not to set -warnings-as-errors" OFF)

message(INFO "-- Alui configuration:")
message(INFO "\t- Demos: ${ALUI_ENABLE_DEMOS}")
message(INFO "\t- Tests: ${ALUI_BUILD_TESTS}")
message(INFO "\t- Docs: ${ALUI_DOCS}")
message(INFO "\t- Linting: ${ALUI_LINT}")

set (CMAKE_CXX_STANDARD 20)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(src)

if (ALUI_TEST OR ALUI_ENABLE_DEMOS)
    set(WANT_TESTS OFF CACHE STRING "" FORCE)
    set(WANT_EXAMPLES OFF CACHE STRING "" FORCE)
    set(WANT_DEMO OFF CACHE STRING "" FORCE)
    set(WANT_POPUP_EXAMPLES OFF CACHE STRING "" FORCE)
    set(WANT_DOCS OFF CACHE STRING "" FORCE)
    set(ALLEGRO_SDL OFF CACHE STRING "" FORCE)
    set(WANT_NATIVE_DIALOG ON CACHE STRING "" FORCE)

    set(SHARED ON CACHE STRING "" FORCE)

    include(FetchContent)

    FetchContent_Declare(allegro5
        GIT_REPOSITORY https://github.com/liballeg/allegro5
        GIT_TAG 5.2.10.0
        OVERRIDE_FIND_PACKAGE
        EXCLUDE_FROM_ALL
    )
    FetchContent_Declare(freetype
        GIT_REPOSITORY https://gitlab.freedesktop.org/freetype/freetype
        GIT_TAG VER-2-13-3
        OVERRIDE_FIND_PACKAGE
        EXCLUDE_FROM_ALL
    )

    FetchContent_MakeAvailable(
        allegro5
        freetype
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
endif()

if (ALUI_LINT)
    find_program(
        CLANGTIDY
        NAMES "clang-tidy" "clang-tidy.exe"
        REQUIRED
    )

    set (CLANG_TIDY_COMMAND "${CLANGTIDY}" "--config-file=${CMAKE_CURRENT_SOURCE_DIR}/.clang-tidy")
    if (LINT_WARNINGS_ARE_ERRORS)
        list (APPEND CLANG_TIDY_COMMAND  -warnings-as-errors=*)
    endif()

    if (WIN32)
        list (APPEND CLANG_TIDY_COMMAND --extra-arg=/EHsc)
    endif()

    message(STATUS "Using CLANG_TIDY_COMMAND=${CLANG_TIDY_COMMAND}")
    set_target_properties(alui PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}")

endif()

if(ALUI_BUILD_TESTS)
    add_subdirectory(tests)

    add_custom_target(test
        COMMAND tests
        DEPENDS tests
        COMMENT "Test alui.cpp")
endif()

if (ALUI_ENABLE_DEMOS)
    add_subdirectory(demos)
endif()

if (ALUI_DOCS)
    find_package(Doxygen)
    if (DOXYGEN_FOUND)
        set(DOXYGEN_GENERATE_HTML YES)
        set(DOXYGEN_GENERATE_MAN NO)
        set(DOXYGEN_EXCLUDE_SYMBOLS "std::*")
        set(DOXYGEN_USE_MDFILE_AS_MAINPAGE "README.md")
        set(DOXYGEN_RECURSIVE YES)
        list(APPEND DOXYGEN_EXCLUDE_PATTERNS "*/build/*" "*/_deps/*")

        doxygen_add_docs(
            doxygen
            ${PROJECT_SOURCE_DIR}
            COMMENT "Generate Doxygen"
        )
    else()
        message(WARNING "ALUI_DOCS is ON, but Doxygen was not found. Docs will not be generated")
    endif()
endif()

# vim:ft=cmake
