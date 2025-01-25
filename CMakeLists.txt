cmake_minimum_required(VERSION 3.28)
project(alui.cpp)

option(ALUI_BUILD_TESTS "Whether or not to build tests. Forced ON if building standalone" OFF)
option(ALUI_DOCS "Whether or not to build doxygen docs" OFF)

if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    message (STATUS "alui: Running standalone")
    set(ALUI_ENABLE_DEMOS ON)
    set(ALUI_BUILD_TESTS ON)
    set(ALUI_DOCS ON)
else()
    message (STATUS "alui: Running as a submodule")
    set (ALUI_ENABLE_DEMOS OFF)
endif()

set (CMAKE_CXX_STANDARD 20)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(src)

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
