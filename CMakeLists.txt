cmake_minimum_required(VERSION 3.28)
if ("${PROJECT_NAME}" STREQUAL "")
    include(FetchContent)
    FetchContent_Declare(conan
        GIT_REPOSITORY https://github.com/conan-io/cmake-conan
        GIT_TAG develop2
    )
    FetchContent_MakeAvailable(conan)
    set(CMAKE_PROJECT_TOP_LEVEL_INCLUDES ${conan_SOURCE_DIR}/conan_provider.cmake)
endif()
project(alui.cpp)

option(ALUI_ENABLE_DEMOS "Whether or not to build demos" OFF)
option(ALUI_BUILD_TESTS "Whether or not to build tests. Forced ON if building standalone" OFF)
option(ALUI_DOCS "Whether or not to build doxygen docs" OFF)
option(ALUI_SANITISE "Whether or not to -fsanitise=undefined" OFF)

option(ALUI_TEST_OPTIONS "Additional options passed to the test. Not required if directly running the test binary")

if (NOT ALUI_TEST_OPTIONS)
    set (ALUI_TEST_OPTIONS "")
endif()

if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    message (STATUS "alui: Running standalone")
    set(ALUI_STANDALONE ON)

    if (NOT WIN32)
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

message(STATUS "Alui configuration:")
message(STATUS "\t- Demos: ${ALUI_ENABLE_DEMOS}")
message(STATUS "\t- Tests: ${ALUI_BUILD_TESTS}")
message(STATUS "\t- Docs: ${ALUI_DOCS}")
message(STATUS "\t- Linting: ${ALUI_LINT}")
message(STATUS "\t- Sanitising: ${ALUI_SANITISE}")

set (CMAKE_CXX_STANDARD 20)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if (WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /FS /EHsc")
endif()

if (ALUI_SANITISE)
    # TODO: Figure out if there's a better way to dynamically-ish set sanitisers
    set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=undefined")
endif()

add_subdirectory(src)

if (ALUI_BUILD_TESTS OR ALUI_ENABLE_DEMOS)
    configure_file(tests/3p/dejavu.ttf ${CMAKE_BINARY_DIR}/dejavu.ttf COPYONLY)

    find_package(Freetype REQUIRED)
    find_package(PNG REQUIRED)
    find_package(ZLIB REQUIRED)

    set(WANT_SHADERS_GL ON CACHE STRING "" FORCE)
    set(WANT_MEMFILE OFF CACHE STRING "" FORCE)
    set(WANT_PHYSFS OFF CACHE STRING "" FORCE)
    set(WANT_NATIVE_DIALOG OFF CACHE STRING "" FORCE)
    set(WANT_VIDEO OFF CACHE STRING "" FORCE)
    set(WANT_IMAGE_WEBP OFF CACHE STRING "" FORCE)
    set(WANT_IMAGE_FREEIMAGE OFF CACHE STRING "" FORCE)
    set(WANT_DOCS OFF)
    set(WANT_DEMO OFF)
    set(WANT_EXAMPLES OFF)
    set(WANT_TESTS OFF)
    set(WANT_POPUP_EXAMPLES OFF)
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "i.86" OR CMAKE_SYSTEM_PROCESSOR MATCHES "x86|AMD64")
        set (WANT_ALLOW_SSE "avx2")
    else()
        set (WANT_ALLOW_SSE OFF)
    endif()
    set(WANT_AUDIO OFF)

    set(SHARED ON CACHE STRING "" FORCE)
    set(BUILD_SHARED_LIBS ON)

    include(FetchContent)

    FetchContent_Declare(allegro5-patch
        GIT_REPOSITORY https://github.com/LunarWatcher/allegro5-cmake
    )

    FetchContent_MakeAvailable(
        allegro5-patch
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
        COMMAND tests $<$<BOOL:${ALUI_TEST_OPTIONS}>:${ALUI_TEST_OPTIONS}>
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
            COMMENT "Generate documentation"
        )
    else()
        message(WARNING "ALUI_DOCS is ON, but Doxygen was not found. Docs will not be generated")
    endif()
endif()

# vim:ft=cmake
